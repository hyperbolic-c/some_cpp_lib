#ifndef READERDEMO_H
#define READERDEMO_H

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <time.h>
#include "DigitalType.h"
#include "WaveformReaderForCompetition.h"

class SignalCoding
{
private:
    std::map<std::string, int> map_symbol_width_;
public:
    SignalCoding(){};
    //~SignalCoding();
    void addSymbolWidth(std::string &symbol, int width);
    int getMapSize();
};

std::string varTypeToString(int type);

void getInputFile(int argc, const char **argv, std::string &inputFile);

std::map<std::string, int> sym_wid;
// void dumpScopeInfo(Scope *scope, FILE *fp);
void dumpScopeInfo(Scope *scope, FILE *fp, SignalCoding *signal_coding);

void dumpTimeVCDatas(FILE *pOut, long long time, const std::vector<std::string> &vcDatas);  

// void dumpHeadInfo(FILE *pOut, WaveformReaderForCompetition &reader);
void dumpHeadInfo(FILE *pOut, WaveformReaderForCompetition &reader, SignalCoding *signal_coding);

void printReadTime(FILE *pOut, clock_t start, clock_t finish);


#endif