
#include "readerDemo.h"

void SignalCoding::addSymbolWidth(std::string &symbol, int width)
{
    map_symbol_width_[symbol] = width;
}

int SignalCoding::getMapSize()
{
    return map_symbol_width_.size();
}

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


void dumpScopeInfo(Scope *scope, FILE *fp, SignalCoding *signal_coding)
{
    assert(scope);

    fprintf(fp, "$scope %s %s $end\n", scope->type().c_str(), scope->name().c_str());
    const std::map<std::string, std::string> &vars = scope->getVars();
    std::map<std::string, std::string>::const_iterator it;
    for (it = vars.cbegin(); it != vars.cend(); ++it)
    {
        std::string varName = (*it).first;
        std::string symbol = (*it).second;
        std::string type = varTypeToString(scope->getSignalType(varName));
        int width = scope->getSignalBus(varName);
        
        signal_coding->addSymbolWidth(symbol, width);  // add
        
        fprintf(fp, "$var %s %d %s %s $end\n", type.data(), width, symbol.data(),
                varName.data());
    }
    // get sub socpes info
    
    const std::vector<Scope *> &subscopes = scope->getSubScopes();
    for (unsigned i = 0; i < subscopes.size(); ++i)
    {
        dumpScopeInfo(subscopes[i], fp, signal_coding);
    }
}

void dumpTimeVCDatas(FILE *pOut, long long time,
                     const std::vector<std::string> &vcDatas)
{
    fprintf(pOut, "#%lld\n", time);
    for (unsigned j = 0; j < vcDatas.size(); ++j)
    {
        fprintf(pOut, "%s\n", vcDatas[j].c_str());
    }
}

void printReadTime(FILE *pOut, clock_t start, clock_t finish)
{
    if (pOut)
    {
        std::cout << "Read And Out Time: " << (finish - start) / CLOCKS_PER_SEC << std::endl;
    }
    else
    {
        std::cout << "Read Time: " << (finish - start) / CLOCKS_PER_SEC << std::endl;
    }
}

void dumpHeadInfo(FILE *pOut, WaveformReaderForCompetition &reader, SignalCoding *signal_coding)
{
    if (pOut)
    {
        fprintf(pOut, "$date\n %s\n$end\n", reader.getDate().data());
        fprintf(pOut, "$version\n %s\n$end\n", reader.getVersion().data());
        fprintf(pOut, "$timescale\n %s\n $end\n", reader.getTimeScale().data());

        std::vector<Scope *> topscopes;
        reader.topScopes(topscopes);
        for (unsigned i = 0; i < topscopes.size(); ++i)
        {
            dumpScopeInfo(topscopes[i], pOut, signal_coding);
        }
        fprintf(pOut, "$enddefinitions $end\n\n");
    }
}


