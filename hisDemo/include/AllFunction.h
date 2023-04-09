#ifndef ALLFUNCTION_H
#define ALLFUNCTION_H

#include <vector>
#include <string>
#include <map>
#include <bitset>

//LZW compress
// std::vector<int> LZW_encode(std::string s);
void LZW_encode(std::string &str, FILE *fout);
// std::string LZW_decode(std::vector<int> code);
void LZW_decode(std::vector<unsigned int> &code, std::string &res);

//std::string getPredictCode(std::string value);
//std::string getOriginalCode(std::string value);
// std::string getResultCode(bitset<2> res);
// template <class ContainerT>
// void toSplit(const std::string &str, ContainerT &tokens);
//time info
 
//signal_value

//signal_ID

#endif
