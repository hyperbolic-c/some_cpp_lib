#ifndef OUTPUTTER_WAVEFORMOUTPUTTERFORCOMPETITION_H_
#define OUTPUTTER_WAVEFORMOUTPUTTERFORCOMPETITION_H_

#include <string>
#include <map>
#include <set>

class WaveFormOutter;
class Scope;

class WaveformOutputterForCompetition {
 public:
  WaveformOutputterForCompetition(const std::string& fileName);   

  ~WaveformOutputterForCompetition();

  void outputHead(const std::string& date,
                  const std::string& version,
                  const std::string& timeScale);

  void outputDefiention(const std::vector<Scope*> &topScopes);

  void outputTranDatas(const std::map<long long, std::vector<std::string> > &sigDatas);

 private:
  WaveFormOutter * _wavePtr;
};


#endif  /// OUTPUTTER_WAVEFORMOUTPUTTERFORCOMPETITION_H__
