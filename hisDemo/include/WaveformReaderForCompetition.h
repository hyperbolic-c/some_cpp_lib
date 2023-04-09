#ifndef LIBREADER_WAVEFORMREADERFORCOMPETITION_H_
#define LIBREADER_WAVEFORMREADERFORCOMPETITION_H_

#include <string>
#include <map>
#include <set>

typedef std::set<std::string> SignalSymbolSet;

class DigitalWaveForm;
class Scope;
class TimeValue;

class WaveformReaderForCompetition {
 public:
  WaveformReaderForCompetition(const std::string& fileName);   

  ~WaveformReaderForCompetition();

  void readHeader();

  // reade specified signal value changed data, this only set the 
  // pointer to the begin time, it need to call readNextPointData(),
  //  getVCTime(), getVCValues() to get value changed data.
  void read(const std::set<std::string> &symbols);

  void beginReadTimeData();

  bool isDataFinished();

  void readNextPointData();

  long long getVCTime();

  const std::vector<std::string> & getVCValues();

  void topScopes(std::vector<Scope*> &scopes); 

  std::string getVersion();

  std::string getDate();
  std::string getTimeScale();

 private:
  DigitalWaveForm * _wavePtr;
};


#endif  /// LIBREADER_WAVEFORMREADERFORCOMPETITION_H_
