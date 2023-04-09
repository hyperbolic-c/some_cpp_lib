#include <sys/stat.h>
#include <time.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <boost/dynamic_bitset.hpp>
#include <boost/algorithm/string.hpp>
#include <gperftools/profiler.h>
#include <unordered_map>
#include "DigitalType.h"
#include "WaveformReaderForCompetition.h"
#include "AllFunction.h"
using namespace std;

std::string varTypeToString(int type)
{
    if (type == REG)
        return "reg";
    else if (type == UWIRE)
        return "uwire";
    else if (type == WIRE)
        return "wire";
    else if (type == TRI)
        return "tri";
    else if (type == TRI1)
        return "tri1";
    else if (type == SUPPLY0)
        return "supply0";
    else if (type == WAND)
        return "wand";
    else if (type == TRIAND)
        return "triand";
    else if (type == TRI0)
        return "tri0";
    else if (type == SUPPLY1)
        return "supply1";
    else if (type == WOR)
        return "wor";
    else if (type == TRIOR)
        return "trior";
    else if (type == TRIREG)
        return "trireg";
    else if (type == INTEGER)
        return "integer";
    else if (type == PARAMETER)
        return "parameter";
    else if (type == REAL)
        return "real";
    else if (type == TIME)
        return "time";
    else if (type == EVENT)
        return "event";

    return std::string();
}

// get the input file
void getInputFile(int argc, const char **argv, std::string &inputFile)
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-')
        {
            inputFile = argv[i];
            break;
        }
    }
    assert(!inputFile.empty());
}

//std::map<std::string, int> signal_width; // map to save signal_symbol and its width
std::unordered_map<std::string, int> signal_width;
void dumpScopeInfo(Scope *scope, FILE *fp)
{
    assert(scope);
    
    // get first scope info
    fprintf(fp, "$scope %s %s $end\n", scope->type().c_str(), scope->name().c_str());
    
    const std::map<std::string, std::string> &vars = scope->getVars();
    std::map<std::string, std::string>::const_iterator it;
    for (it = vars.cbegin(); it != vars.cend(); ++it)
    {
        std::string varName = (*it).first;
        std::string symbol = (*it).second;
        std::string type = varTypeToString(scope->getSignalType(varName));
        int width = scope->getSignalBus(varName);
        fprintf(fp, "$var %s %d %s %s $end\n", type.data(), width, symbol.data(), varName.data());
        // get the signal width
        signal_width[symbol] = width;
    }
    // get sub scopes info
    const std::vector<Scope *> &subscopes = scope->getSubScopes();
    for (unsigned i = 0; i < subscopes.size(); ++i)
    {
        dumpScopeInfo(subscopes[i], fp);
    }
}

std::string getPredictCode(std::string value)
{
    if (value == "0")
    { // 0 -> 1
        return "01";
    }
    else
    { // 1 x z -> 0
        return "00";
    }
    return std::string();
}

std::string getOriginalCode(std::string value)
{
    if (value == "0")
    {
        return "00";
    }
    else if (value == "1")
    {
        return "01";
    }
    else if (value == "x")
    {
        return "10";
    }
    else if (value == "z")
    {
        return "11";
    }
    return std::string();
}

std::string getResultCode(bitset<2> res)
{
    if (res.none()) // 00
    {
        return "0";
    }
    else if (res.test(0) && (!res.test(1))) // 01
    {
        return "1";
    }
    else if ((!res.test(0)) && res.test(1)) // 10
    {
        return "x";
    }
    else if (res.all())
    {
        return "z";
    }

    return std::string();
}

void toFillSignal(std::string &signal, std::string symbol)
{
    int diff = 0;
    
    diff = signal_width[symbol] - signal.size() + 1;
    if (diff == 0)
    { // filled
        return;
    }
    else
    { // need to fill up
      // bx fill with x, bz fill with z, b0 or b1 fill with 0
        if (signal[1] == '1')
        {
            string stradd(diff, '0');
            signal.insert(1, stradd);
        }
        else if (signal == "r50")  //r50 to binary
        {
            signal = "r110010";
            string stradd(diff-4, '0');
            signal.insert(1, stradd);
        }
        else
        {
            string stradd(diff, signal[1]);
            signal.insert(1, stradd);
        }
    }
}

void toXorSignal(std::string &original, std::string &history, std::string &res_out)
{
    res_out.clear();
    boost::dynamic_bitset<> res;
    boost::dynamic_bitset<> predict(history.substr(1));
    predict.flip(0); // Invert the least significant bit to predict
    boost::dynamic_bitset<> current(original.substr(1));
    res = predict ^ current;
    boost::to_string(res, res_out);
    res_out = "b" + res_out;
}


template <class ContainerT>
void toSplit(const std::string &str, ContainerT &tokens)
{
    bool trimEmpty = false;
    const std::string delimiters = " ";
    std::string::size_type pos, lastPos = 0, length = str.size();
    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (lastPos < length + 1)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if (pos == std::string::npos)
        {
            pos = length;
        }
        if (pos != lastPos || !trimEmpty)
        {
            tokens.push_back(value_type(str.data()+lastPos,
            (size_type)pos-lastPos));
        }
        lastPos = pos + 1;
    }
    
}

static const char digit_pairs[201] = {
  "00010203040506070809"
  "10111213141516171819"
  "20212223242526272829"
  "30313233343536373839"
  "40414243444546474849"
  "50515253545556575859"
  "60616263646566676869"
  "70717273747576777879"
  "80818283848586878889"
  "90919293949596979899"
};

void intToString(int num, std::string &res)
{
    res.clear();
    // buffer is big enough for significant digits and extra digit, minus and null
    char buf[std::numeric_limits<long>::digits10 + 3];

    const bool isNegative = num < 0;
    char *p = buf + sizeof(buf);
    *--p = '\0';
    num = num < 0 ? -num : num;

    while (num > 99)
    {
        unsigned pos = num % 100;
        num /= 100;
        p -= 2;
        *(short *)(p) = *(short *)(digit_pairs + 2 * pos);
    }
    if (num > 9)
    {
        p -= 2;
        *(short *)(p) = *(short *)(digit_pairs + 2 * num);
    }
    else
    {
        *--p = '0' + char(num);
    }

    if (isNegative)
        *--p = '-';

    res = std::string(p);
}

void changeToAlpha(char &value)
{
    if (value == '0')
    {
        value = 'o';
    }
    else if (value == '1')
    {
        value = 'n';
    }
    else
    {
        return;
    }
}

void changeToDigit(char &value)
{
    if (value == 'o')
    {
        value = '0';
    }
    else if (value == 'n')
    {
        value = '1';
    }
    else
    {
        return;
    }
}

/* 0->o 1->n
 * to avoid the run length coding can not distinguish 01 or char
 */

void RLC_encode(std::string &inputstring, std::string &outputstring)
{
    char chan = 'o';
    // char buff[128];
    std::string res;
    int n = inputstring.length();
    for (unsigned int i = 0; i < n; i++)
    {
        int count = 1;
        while (inputstring[i] == inputstring[i + 1] && i < n)
        {
            ++count;
            ++i;
        }
        if (count <= 1)
        {
            changeToAlpha(inputstring[i]);
            outputstring += inputstring[i];
        }
        else
        {
            intToString(count, res);
            outputstring += res;
            changeToAlpha(inputstring[i]);
            outputstring += inputstring[i];
        }
    }
}

bool alpha_or_space(const char c)
{
    return isalpha(c) || c == ' ';
}

void RLC_decode(std::string &compressed, std::string &original)
{
    size_t i = 0;
    size_t repeat;
    char chan = 'o';
    while (i < compressed.length())
    {
        // normal alpha charachers
        while (alpha_or_space(compressed[i]))
        {
            changeToDigit(compressed[i]);
            original.push_back(compressed[i]);
            i++;
        }

        // repeat number
        repeat = 0;
        while (isdigit(compressed[i]))
            repeat = 10 * repeat + (compressed[i++] - '0');

        // unroll releat charachters
        changeToDigit(compressed[i]);
        auto char_to_unroll = compressed[i];
        i++;
        while (repeat--)
            original.push_back(char_to_unroll);
    }
}

bool toExcludeXZ(std::string &value)
{ // exclude signal b with x z
    if (value.find('x') == std::string::npos && value.find('z') == std::string::npos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::unordered_map<std::string, std::string> valHistory;  // to save previous signal_value <symbol, value>
std::unordered_map<std::string, int> symHistory;  // to save the sama signal_symbol_set <symbol_set, index>
long long pre = 0;  // save the difference between time
int encodenum = 0;  // save the index of symHistory

void dumpTimeVCDatas(const std::vector<std::string> &vcDatas, std::string &allValue, std::string &allSymbol)
{
    /* vsData[j] include signal_value and signal_ID
     * so we need to operate vcDatas to get what we need
     * for(b r) use space to split
     * for(0 1 x z) signal_value is the first char
     * because involve string split, consider to implement c++11 string_view
     */

    allValue.clear();
    allSymbol.clear();
    std::vector<std::string> oneValSym;
    std::string res_out;
    for (unsigned j = 0; j < vcDatas.size(); ++j)
    {
        auto end = valHistory.end();
        if (vcDatas[j][0] == 'b' || vcDatas[j][0] == 'r') // VECTOR signal b r
        {
            oneValSym.clear();
            toSplit(vcDatas[j], oneValSym);
            // value = oneValSym[0];
            // symbol = oneValSym[1];
            auto it = valHistory.find(oneValSym[1]);
            if (oneValSym[0][0] == 'b' && toExcludeXZ(oneValSym[0]))
            { // fill up the VECTOR signal b
                if (it != end)
                {                                                       // find previous value
                    toFillSignal(oneValSym[0], oneValSym[1]);           // fill up
                    
                    toXorSignal(oneValSym[0], it->second, res_out);  // xor
                    valHistory[oneValSym[1]] = oneValSym[0];  // update

                    allValue += res_out;
                }
                else
                { // not find
                    toFillSignal(oneValSym[0], oneValSym[1]);
                    valHistory[oneValSym[1]] = oneValSym[0]; // add
                    allValue += oneValSym[0];
                }
            }
            else
            { // real signal or bz bx, directly save
                toFillSignal(oneValSym[0], oneValSym[1]);
                allValue += oneValSym[0];
            }
            // save all symbol and use space to split
            allSymbol += oneValSym[1];
            allSymbol += ' ';
        }
        else // SCALAR signal 0 1 x z
        {
            string sval;
            sval = vcDatas[j][0];
            string sym = vcDatas[j].substr(1);

            auto it = valHistory.find(sym);
            if (it != end)
            {  // find previous value, encode = predict ^ original
                bitset<2> predict(getPredictCode(it->second)); // predict code
                bitset<2> original(getOriginalCode(sval));     // original code
                bitset<2> res = predict ^ original;
                allValue += getResultCode(res); // save to run length encode
                valHistory[sym] = sval;         // update
            }
            else
            {  // not find which mean the first symbol, add to valHistory
                allValue += sval;
                valHistory[sym] = sval; // add
            }
            // save all symbol and use space to split
            allSymbol += sym;
            allSymbol += ' ';
        }
        
    }
    /* signal_value include scalar(0 1 x z) and vector(b r)
     * (0 1 x z) use predict encode which (0 -> 1), (1 x z -> 0)
     * and use boost::dynamic_bitset<> or bitset<> to xor (00 01 10 11)
     * (b) use the signal_width to fill up
     * and then current signal_vlaue xor previous signal_value
     * for (0 1 x z) signal width is 1 so use signal_width to split
     * for (b, r), fill up use signal_width too
     */
    
}

void encodeAndOut(long long time, std::string &allValue, std::string &allSymbol, std::string &timeResult)
{
    /*for time part
     * save the difference between current time and previous time
     * encode{
     *     diff = time -pre;
     *     pre = time;
     * }
     * decode{
     *     diff = time + pre;
     *     pre = diff;
     * }
     */

    /* splicing and than use RLE to compress, after that use LZW to compress secondly
     * signal_ID use a dictionary to save the same set with an index
     * definition: vector<time, symbol_index, signal_value>
     */
    long long diff = 0;
    diff = time - pre;
    pre = time;

    int index = 0;

    auto symend = symHistory.end();
    auto symit = symHistory.find(allSymbol);

    std::string encodeResult;
    encodeResult.reserve(allValue.size());
    RLC_encode(allValue, encodeResult);

    // std::string timeString;
    // timeResult.clear();
    intToString(diff, timeResult);
    timeResult += ' ' + encodeResult;
    std::string indexResult;
    if (symit != symend)
    { // find the same symbol_set
        index = symit->second;
        intToString(index, indexResult);
        timeResult += ' ' + indexResult + '\n';
        // fprintf(fp, "#%s %s %d\n", timeResult.data(), encodeResult.data(), index);
    }
    else
    {
        symHistory[allSymbol] = encodenum;
        intToString(encodenum, indexResult);
        timeResult += ' ' + indexResult + '\n';
        ++encodenum;
    }

}

// #define LZWBUFFER 20000000
// allocate memory to save string for LZW
struct LZW_STRUCT
{
    unsigned int buffSize = 20000000;
    // char *buffer = NULL;
    std::string buffer = "";
    unsigned int stepSize = 0;
    // unsigned int lzwIndex = 0;
} lzwBuffer;
typedef struct LZW_STRUCT LZWBUFFER;

void lzwAndOut(FILE *fp, std::string &timeResult, LZWBUFFER *lzwBuffer)
{
    lzwBuffer->stepSize += timeResult.size();
    // if (lzwBuffer->stepSize % 10000 == 0)
    // {
    //     printf("%u\n", lzwBuffer->stepSize);
    // }

    if (lzwBuffer->stepSize >= lzwBuffer->buffSize)
    {  // reaches the buffSize lZW_encode
        fprintf(fp, "$lzw\n");  // consider whether need a flag for lzw_encode
        LZW_encode(lzwBuffer->buffer, fp);
        lzwBuffer->buffer.clear();  // reset
        lzwBuffer->stepSize = 0;
        lzwBuffer->buffer += timeResult;  // take the current
    }
    else
    {
        lzwBuffer->buffer += timeResult;
    }
}
int main(int argc, const char **argv)
{
    // ProfilerStart("./read_test.prof");
    if (argc < 2)
    {
        printf("Please input a VCD waveform file\n");
        exit(-1);
    }

    /*bool dump = false;
    for(int i = 1; i < argc; ++i) {
      std::string argvs = argv[i];
      if (argvs.find("-dump") != std::string::npos) {
        dump = true;
      }
    }*/

    std::string inputFile;
    getInputFile(argc, argv, inputFile);
    WaveformReaderForCompetition reader(inputFile);
    std::string outFile = inputFile + ".out";
    FILE *fOut;
    fOut = fopen(outFile.data(), "w");
    /*std::string outFile = inputFile + ".out";
    FILE* pOut = nullptr;
    if (dump) {
      pOut = fopen(outFile.data(), "w");
      if (pOut == nullptr) {
        std::cout << " cant' open file " << outFile << std::endl;
      }
    }*/

    // read header.
    clock_t start, finish;
    start = clock();
    reader.readHeader(); // header include data, version, time scale, and var
                         // definition
    if (fOut)
    {
        fprintf(fOut, "$date\n %s\n$end\n", reader.getDate().data());
        fprintf(fOut, "$version\n %s\n$end\n", reader.getVersion().data());
        fprintf(fOut, "$timescale\n %s\n$end\n", reader.getTimeScale().data());

        // var definition
        std::vector<Scope *> topscopes;
        reader.topScopes(topscopes); // get first scope and then get sub scopes
        for (unsigned i = 0; i < topscopes.size(); ++i)
        {
            dumpScopeInfo(topscopes[i], fOut);
        }
        fprintf(fOut, "$enddefinitions $end\n\n");
    }

    // read all signals time block data.
    std::string allValue;
    std::string allSymbol;
    std::string stringResult;
    allValue.reserve(1000);
    allSymbol.reserve(1000);
    stringResult.reserve(2000);
    lzwBuffer.buffer.reserve(lzwBuffer.buffSize);
    // lzwBuffer.buffer = new char[lzwBuffer.buffSize];
    // if (lzwBuffer.buffer == NULL)
    // {
    //     fprintf(stderr, "can not allocate memory for LZWBUFFER");
    //     perror(NULL);
    //     exit(1);
    // }

    
    reader.beginReadTimeData(); // must call this function before read VC data.
    while (!reader.isDataFinished())
    {
        // in this part, we need to handle time info, signal_value and signal_ID
        // for a loop, we get one times info
        // so, consider how to store the previous and current info
        reader.readNextPointData();
        // data.first = reader.getVCTime();
        long long time = reader.getVCTime();

        const std::vector<std::string> &vcDatas = reader.getVCValues();
        // data.second = vcDatas;
        // process data here.
        dumpTimeVCDatas(vcDatas, allValue, allSymbol);
        encodeAndOut(time, allValue, allSymbol, stringResult);
        lzwAndOut(fOut, stringResult, &lzwBuffer);
        stringResult.clear();
    }
    fprintf(fOut, "$lzw\n");
    LZW_encode(lzwBuffer.buffer, fOut);  // the remaining part
    fprintf(fOut, "$endlzw\n");

    // printf("%lu\n", symHistory.size());
    for(auto symSave = symHistory.begin(), end = symHistory.end();symSave != end;++symSave)
    {
        fprintf(fOut, "%d %s\n", symSave->second, symSave->first.data());
    }
    // free memory
    // delete []lzwBuffer.buffer;
    // lzwBuffer.buffer = NULL;

    finish = clock();
    if (fOut)
    {
      std::cout<< "ReadAndOutTime: " << (finish - start) / CLOCKS_PER_SEC << std::endl;
      fclose(fOut);
    }
    else
    {
        std::cout<< "ReadTime: " << (finish - start) / CLOCKS_PER_SEC << std::endl;
        // 37s
    }
    //ProfilerStop();
    
    /* g++ readerDemo.cpp -std=c++11 -o read -L../lib/ -lwavereader -l profiler
     * ./read ~/waveform/test1.vcd
     * pprof --pdf ./read read_test.prof > t1vcd.pdf
     * g++ ImprovedRead.cpp AllFunction.cpp -std=c++11 -o Impro -L../lib/ -lwavereader -lprofiler
     * ./Impro ~/waveform/test1.vcd
     * pprof --pdf ./Impro improved.prof > impro.pdf
     */
}
