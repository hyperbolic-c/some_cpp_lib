
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <DigitalType.h>
#include <WaveformOutputterForCompetition.h>


void outputHeader(WaveformOutputterForCompetition *outter) {
  std::string date = "Tue Dec 10 14:57:45 2002";
  std::string version = "Sim Version 5.6el";
  std::string timescale = "1ns";
  outter->outputHead(date, version, timescale);
}

void outputDefinition(WaveformOutputterForCompetition *outter) {
  //generate top scopes here.
  std::vector<Scope*> topScopes;
  outter->outputDefiention(topScopes);
}

void outputDataOfAllSignals(WaveformOutputterForCompetition *outter) {
  std::map<long long, std::vector<std::string> > sigDatas;
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

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Usage: output_file\n");
    exit(-1);
  }
  const std::string inputFile = argv[1];
  WaveformOutputterForCompetition outter(inputFile);
  // output header;
  outputHeader(&outter);

  // out definition.
  outputDefinition(&outter);
  // out all signals data.
  outputDataOfAllSignals(&outter);
}
