#include <sys/stat.h>
#include <time.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <boost/algorithm/string.hpp>
#include <gperftools/profiler.h>
#include <unordered_map>
#include "DigitalType.h"
#include "WaveformOutputterForCompetition.h"
#include "AllFunction.h"


template <class ContainerT>
void toSplit(const std::string &str, ContainerT &tokens, const std::string &delimiters)
{
    bool trimEmpty = false;
    // const std::string delimiters = " ";
    // std::string::size_type pos, lastPos = 0, length = str.size();
    std::string::size_type pos;
    unsigned int lastPos = 0;
    unsigned int length = str.size();
    
    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (lastPos < length + 1)
    {
        //std::cout << lastPos << std::endl;
        pos = str.find_first_of(delimiters, lastPos);
        // printf("hello shit\n");
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


void outputHeader(WaveformOutputterForCompetition *outter)
{
    std::string date = "Tue Dec 10 14:57:45 2002";
    std::string version = "Sim Version 5.6el";
    std::string timescale = "1ns";
    outter->outputHead(date, version, timescale);
}

void outputDefinition(WaveformOutputterForCompetition *outter)
{
    // generate top scopes here.
    std::vector<Scope *> topScopes;
    outter->outputDefiention(topScopes);
}

// sigDatas map<time, signal_data>
void outputDataOfAllSignals(WaveformOutputterForCompetition *outter)
{
    std::map<long long, std::vector<std::string>> sigDatas;
    // just an example, you should get all transaction data here.
    sigDatas[0].push_back("b110010 !");
    sigDatas[0].push_back("b101 6");
    sigDatas[0].push_back("b1000 7");
    sigDatas[0].push_back("b1000 8");
    sigDatas[0].push_back("1#");
    sigDatas[0].push_back("0\"");
    sigDatas[25].push_back("0#");
    sigDatas[25].push_back("b0 L");
    sigDatas[25].push_back("1<");
    outter->outputTranDatas(sigDatas);
}

void stringToint(const char *p, unsigned int &x)
{
    while (*p != '\0')
    {
        x = (x * 10) + (*p - '0');
        ++p;
    }
}

std::unordered_map<std::string, int> signalWidth;
void getSignalWidth(std::ifstream &fin, std::string &buff)
{
    buff.clear();
    std::vector<std::string> signalVar;
    unsigned int width;
    while (getline(fin, buff))
    {
        if (buff.find("$var") != std::string::npos)
        {
            
            toSplit(buff, signalVar, " ");
            // stringToint(signalVar[2].c_str(), width);
            
            width = std::stoi(signalVar[2]);
            signalWidth[signalVar[3]] = width;
            signalVar.clear();
        }
        else if (buff.find("$lzw") != std::string::npos)
        {
            break;
        }
    }
    // std::cout << signalWidth.size() << std::endl;
}

struct LZW_STRUCT
{
    FILE *fout;
    unsigned int buffSize = 20000000;
    std::string buffer = "";
    unsigned int stepSize = 0;
} lzwBuffer;
typedef struct LZW_STRUCT LZWBUFFER;



int encodenum = 0;  // save the index of symHistory
long long pre = 0;  // save the difference between time
void getTime(std::string &time, long long &res)
{
    res = std::stoll(time) + pre;
    pre = res;
}

std::string getPredictCode(std::string &value)
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

std::string getOriginalCode(std::string &value)
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

std::string getResultCode(std::bitset<2> &res)
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

std::unordered_map<std::string, std::string> valHistory;  // to save previous signal_value <symbol, value>
std::unordered_map<int, std::string> symHistory;  // to save the sama signal_symbol_set <index, symbol_set>

void getScalarSignal(std::string &value, std::string &sym, std::string &res_out)
{
    // 0 1 x z
    res_out.clear();
    auto it = valHistory.find(sym);
    auto end = valHistory.end();
    if (it != end)
    {  // find previous value, encode = predict ^ original
        std::bitset<2> predict(getPredictCode(it->second)); // predict code
        std::bitset<2> original(getOriginalCode(value));     // XOR code
        std::bitset<2> res = predict ^ original;  
        res_out = getResultCode(res); // original code
        valHistory[sym] = res_out;  // update
        res_out += sym;
    }
    else
    {  // not find which mean the first symbol, add to valHistory
        valHistory[sym] = value;  // add
        res_out = value + sym;  // as signal result
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

void getValidBits(std::string &value)
{
    unsigned int n = value.length();
    if (value[1] == '1')
    {
        return;
    }

    for (unsigned int i = 1; i < n; ++i)
    {
        int count = 1;
        while (value[i] == value[i + 1] && i < n-2)
        {
            ++count;
            ++i;
        }
        value.erase(1, count);
        break;
    }
}


void getVectorSignal(std::string &value, std::string &sym, std::string &valsym)
{
    // r bx bz b01
    valsym.clear();
    if (value[0] == 'b' && toExcludeXZ(value))
    {
        auto it = valHistory.find(sym);
        auto end = valHistory.end();
        std::string res_out;
        if (it != end)
        {
            //find it
            res_out.clear();
            
            toXorSignal(value, it->second, res_out);  // xor
            // res_out = value;
            valHistory[sym] = res_out;  // updata
            getValidBits(res_out);
            valsym = res_out + ' ' + sym;  // splicing
        }
        else
        {
            valHistory[sym] = value;  // add
            getValidBits(value);
            valsym = value + ' ' + sym;  // splicing
        }
    }
    else
    {
        getValidBits(value);
        if (value == "r110010")
        {
            value = "r50";
        }
        // directly save
        valsym = value + ' ' + sym;
    }
}



void getValue(std::string &allValue, std::vector<std::string> &symbol, std::vector<std::string> &tarSym,          std::vector<std::string> &Data)
{
    unsigned int size = symbol.size();  // how many symbols are there at this moment
    int width = 0;  // siganl width
    std::string value;  // signal value
    std::string valsym;  // to save {value symbol}

    auto tarbegin = tarSym.begin();
    auto tarend = tarSym.end();

    for (unsigned int i = 0; i < size; ++i)
    {
        value.clear();
        valsym.clear();
        auto findsym = find(tarbegin, tarend, symbol[i]);
        width = signalWidth[symbol[i]];
        if (width == 1)  // scalar signal
        {
            value = allValue.substr(0, 1);
            allValue.erase(0, 1);
            if (findsym != tarend)
            {
                // symbol[i] is one of target symbol
                getScalarSignal(value, symbol[i], valsym);
                // only retain the valid bits
            }
            else
            {
                continue;
            }
        }
        else  // vector signal
        {
            value = allValue.substr(0, width+1);
            allValue.erase(0, width+1);
            if (findsym != tarend)
            {
                // symbol[i] is one of target symbol
                getVectorSignal(value, symbol[i], valsym);
                // only retain the valid bits
            }
            else
            {
                continue;
            }  
        }
        Data.emplace_back(valsym);  // waveform data in this moment   
    }
}

void getRLCDecode(std::ifstream &fin, FILE *fout, std::vector<std::string> &tarSym, std::set<int> &tarIndex)
{
    std::string buff;
    std::vector<std::string> timeValSym;  // save {time value index}
    std::vector<std::string> symbol;  // save all symbol
    std::string valResult;
    long long time = 0;
    std::vector<std::string> Data;

    auto tarend = tarIndex.end();

    while (getline(fin, buff))
    {
        timeValSym.clear();  // Forgetting to clear is a disaster shit!!!!
        symbol.clear();
        valResult.clear();
        Data.clear();
        toSplit(buff, timeValSym, " ");  // split to get the {time value symbol}

        getTime(timeValSym[0], time);
        //for symbol
        int index = std::stoi(timeValSym[2]);
        auto findIndex = tarIndex.find(index);
        if (findIndex != tarend)
        {
            // this time have the target symbol

            // for time
            
            fprintf(fout, "#%lld\n", time);

            // for value, run length coding
            RLC_decode(timeValSym[1], valResult);

            toSplit(symHistory[index], symbol, " ");  // split all symbol get single symbol
            getValue(valResult, symbol, tarSym, Data);  // split the value according to the split symbol

            // output
            auto vec = Data.size();
            for (unsigned int i = 0; i < vec; ++i)
            {
                fprintf(fout, "%s\n", Data[i].data());
            }
        }
        else
        {
            continue;
        }
    }
}

void getLzwDecode(std::ifstream &fin, std::string &buff, LZWBUFFER *lzwBuffer)
{
    
    buff.clear();
    unsigned int code;
    std::vector<unsigned int> lzwCode;
    
    // std::vector<std::string> splitRes;
    while (getline(fin, buff))
    {
        if (buff == "$lzw")
        {
            LZW_decode(lzwCode, lzwBuffer->buffer);
            fprintf(lzwBuffer->fout, "%s", lzwBuffer->buffer.c_str());
            lzwBuffer->buffer.clear();
            lzwCode.clear();
        }
        else if (buff == "$endlzw")
        {
            LZW_decode(lzwCode, lzwBuffer->buffer);
            fprintf(lzwBuffer->fout, "%s", lzwBuffer->buffer.c_str());
            lzwBuffer->buffer.clear();
            lzwCode.clear();
            return;
        }
        else
        {
            code = std::stoul(buff);
            lzwCode.emplace_back(code);
        } 
    }
}

void getSymHistory(std::ifstream &fin)
{
    std::string buff;
    std::vector<std::string> res;
    while (getline(fin, buff))
    {
        int index = std::stoi(buff.substr(0, buff.find(' ')));

        symHistory[index] = buff.substr(buff.find(' ')+1);
    }
    
}

void getTarSym(const std::string &file, std::vector<std::string> &tarSym, std::set<int> &tarIndex)
{
    tarSym.clear();
    tarIndex.clear();
    std::ifstream tar(file, std::ios::in);
    std::string buff;
    while (getline(tar, buff))
    {
        tarSym.emplace_back(buff);
    }
    auto symend = symHistory.end();
    
    auto tarsize = tarSym.size();
    
    for (unsigned int i = 0; i < tarsize; ++i)
    {
        for (auto symit = symHistory.begin(); symit != symend; symit++)
        {
            auto findsym = symit->second.find(tarSym[i]);
            if (findsym != std::string::npos)
            {
                tarIndex.insert(symit->first);
            } 
        }
    }
}


int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        printf("Usage: output_file\n");
        exit(-1);
    }
    clock_t start, finish;
    start = clock();
    const std::string inputFile = argv[1];
    std::string buff;
    buff.reserve(100);
    
    lzwBuffer.buffer.reserve(lzwBuffer.buffSize);
    
    lzwBuffer.fout = fopen("./lzwdcode.txt", "w");
    
    std::ifstream fin(inputFile, std::ios::in);  // input Impcode.txt
    getSignalWidth(fin, buff);  // reach the first "$lzw"
    
    getLzwDecode(fin, buff, &lzwBuffer);  // reach the "$endlzw"
    getSymHistory(fin);  // get the symHistory
    //std::cout << symHistory.size();  // 4964
    fin.close();  // LZW decompressed
    fclose(lzwBuffer.fout);  // output lzwdcode.txt

    // run length decoding and XOR predict decoding
    std::ifstream flzw("./lzwdcode.txt", std::ios::in);
    lzwBuffer.fout = fopen("./RLCdecode.txt", "w");
    // target symbol
    const std::string tarSymFile = argv[2];
    std::vector<std::string> tarSym;  // save target symbol
    std::set<int> tarIndex;  // save the index which has target symbol
    getTarSym(tarSymFile, tarSym, tarIndex);

    getRLCDecode(flzw, lzwBuffer.fout, tarSym, tarIndex);
    flzw.close();
    fclose(lzwBuffer.fout);
    finish = clock();
    std::cout<< "DecompressTime: " << (finish - start) / CLOCKS_PER_SEC << std::endl;
}
