#ifndef DIGITALTYPE_H
#define DIGITALTYPE_H

#include <vector>
#include <string>
#include <map>
#include <set>

enum TypeDefine
{
  REG = 0,
  UWIRE,
  WIRE,
  TRI,
  TRI1,
  SUPPLY0,
  WAND,
  TRIAND,
  TRI0,
  SUPPLY1,
  WOR,
  TRIOR,
  TRIREG,
  INTEGER,
  PARAMETER,
  REAL,
  TIME,
  EVENT,
};

struct Signal {
    std::string _symbol;
    std::string _valStr;
    long _lineNum;
};

struct DumpOff {
  DumpOff(long long start, long long end) :
    _startTime(start), _endTime(end) {
  }

  long long _startTime;
  long long _endTime;
};

class TimeValue {
  public:
   enum TimeFlag {
     DUMPVARS = 0,
     DUMPOFF,
     DUMPON,
     DUMPALL,
     NORMAL
   };

   TimeValue(long long time) :
     _time(time), _flag(NORMAL) {
   }

   ~TimeValue();

   inline long long time() const {
     return _time;
   }

   inline void setTimeFlag(TimeFlag flag) {
     _flag = flag;
   }

   inline TimeFlag flag() {
     return _flag;
   }

   inline void addSignal(Signal* sig) {
     _signals.push_back(sig);
   }

   const std::vector<Signal*>& allSignals() const {
     return _signals;
   }

  private:
   long long _time;
   TimeFlag _flag;
   std::vector<Signal*> _signals;
};

class Scope {
  public:
    Scope(std::string name, std::string type, Scope *parent) :
      _name(name), _type(type), _parent(parent) {}
    ~Scope() {}
    void addSubScope(Scope* subScope);
    Scope* getParentScope();
    const std::vector<Scope*>& getSubScopes();
    const std::map<std::string, std::string>& getVars();
    void setSignalType(std::string, int);
    void setSignalBus(std::string, int);
    void setSignalSymbol(std::string name, std::string symbol);
    int getSignalType(std::string);
    int getSignalBus(std::string);
    std::string getSignalSymbol(std::string);
    std::string name();
    std::string type();

 private:
  std::string _name;
  std::string _type;
  Scope* _parent;
  std::vector<Scope*> _subScopes;
  std::map<std::string, int> _mapSignalBus;
  std::map<std::string, int> _mapSignalType;
  std::map<std::string, std::string> _signalSymbol;
};


#endif ///< DIGITALTYPE_H
