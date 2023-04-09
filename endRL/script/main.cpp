#include "readerDemo.h"


int main(int argc, const char **argv)
{
    if (argc < 2) {
        printf("Please input a VCD waveform file\n");
        exit(-1);
    }

    bool dump = false;
    for (int i = 1; i < argc; ++i) {
        std::string argvs = argv[i];
        if (argvs.find("-dump") != std::string::npos) {
            dump = true;
        }
    }

    std::string inputFile;
    getInputFile(argc, argv, inputFile);
    // a reader object to read the VCD input file
    WaveformReaderForCompetition reader(inputFile);
    std::string outFile = inputFile + ".out";
    FILE *pOut = nullptr;

    if (dump) {
        pOut = fopen(outFile.data(), "w");
        if (pOut == nullptr) {
            std::cout << " cant' open file " << outFile << std::endl;
        }
    }

    // read header.
    clock_t start, finish;
    start = clock();
    reader.readHeader(); // header include data, version, time scale, and var definition
    // std::map<std::string, int> map_symbol_width;
    SignalCoding signal_coding;
    dumpHeadInfo(pOut, reader, &signal_coding);
    // std::string sym = "aaa";
    // signal_coding.addSymbolWidth(sym, 2);

    reader.beginReadTimeData(); //must call this function before read VC data.
    //read all signals time block data
    while (!reader.isDataFinished()) {
        reader.readNextPointData();
        // time and signal value information
        long long time = reader.getVCTime();
        const std::vector<std::string> &vcDatas = reader.getVCValues();
        // process data here.
        if (pOut) {
            dumpTimeVCDatas(pOut, time, vcDatas);
        }
    }
    finish = clock();
    printReadTime(pOut, start, finish);
    std::cout << signal_coding.getMapSize() << std::endl;
    //std::cout << sym_wid.size() << std::endl;
}
