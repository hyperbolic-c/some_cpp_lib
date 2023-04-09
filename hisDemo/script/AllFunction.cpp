#include <sys/stat.h>
#include <time.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include "AllFunction.h"
using namespace std;


// vector<int> LZW_encode(string s)
// {
//     /* input: a string to encode
//      * output: the encode num in a vector
//      */
    
//     // initial
//     map<string, int> dict;
//     for (unsigned i = 0; i < 128; ++i)
//     {
//         string ch = "";
//         ch += char(i);
//         dict[ch] = i;
//     }
//     int encodenum = 128;

//     string p = "";  //previous
//     string c = "";  //current
//     vector<int> EncodeResult;  //the vector to save encode result

//     for (unsigned i = 0; i < s.length(); ++i)
//     {
//         c = s[i];
//         string tempStr = p + c;
//         // find the string in the dict map
//         map<string, int>::iterator iter = dict.find(tempStr);
//         if (iter != dict.end())
//         {  //fint it
//             p = tempStr;
//         }
//         else
//         {  //not find
//             EncodeResult.push_back(dict[p]);  //save the index of p
//             dict[tempStr] = encodenum;  // set up a new index of p+c
//             encodenum++;
//             p = c;  //update
//         }
//         c = "";
//     }

//     EncodeResult.push_back(dict[p]);  // the end
//     return EncodeResult;
// }

void LZW_encode(std::string &str, FILE *fout)
{
    /* input: a string to encode
     * output: the encode num in a vector
     */
    
    // initial
    map<string, unsigned int> dict;
    for (unsigned int i = 0; i < 128; ++i)
    {
        string ch = "";
        ch += char(i);
        dict[ch] = i;
    }
    unsigned int encodenum = 128;

    string p = "";  //previous
    string c = "";  //current
    // vector<int> EncodeResult;  //the vector to save encode result

    for (unsigned i = 0; i < str.length(); ++i)
    {
        c = str[i];
        string tempStr = p + c;
        // find the string in the dict map
        // map<string, int>::iterator iter = dict.find(tempStr);
        auto iter = dict.find(tempStr);
        if (iter != dict.end())
        {  //fint it
            p = tempStr;
        }
        else
        {  //not find
            // EncodeResult.push_back(dict[p]);  //save the index of p
            // printf("%u\n", dict[p]);
            fprintf(fout, "%u\n", dict[p]);
            dict[tempStr] = encodenum;  // set up a new index of p+c
            encodenum++;
            p = c;  //update
        }
        c = "";
    }

    // EncodeResult.push_back(dict[p]);  // the end
    fprintf(fout, "%u\n", dict[p]);
    // return EncodeResult;
}


void LZW_decode(std::vector<unsigned int> &code, std::string &res)
{
    /* input: the encode num in a vector
     * output: the decode string
     */
    res.clear();
    map<unsigned int, string> resdict;
    for (unsigned int i = 0; i < 128; ++i)
    {
        string ch = "";
        ch += char(i);
        resdict[i] = ch;
    }
    
    
    unsigned int count = 128;
    unsigned int old = code[0];  //first notation
	unsigned int n;
	string s = resdict[old];  //first symbol
	string c = "";
	c += s[0];

    // string res = "";
    res += s; 
	for (unsigned int i = 1; i < code.size(); ++i)
	{
		n = code[i];  //next notation
        auto iter = resdict.find(n);

        if (iter != resdict.end())
		{  // find
            s = resdict[n];
		}
		else
		{  // not find
           // mean the current index should be encode in this step
           // so current string the first char is previous string the first char
            s = resdict[old];
			s = s + c;
		}

		//cout << s;
        res += s;
		c = "";
		c += s[0];
		resdict[count] = resdict[old] + c;  //restore the encoding process
		count++;
		old = n;
	}
    // return res;
}

// std::string getPredictCode(std::string value)
// {
//     if (value == "0")
//     { // 0 -> 1
//         return "01";
//     }
//     else
//     { // 1 x z -> 0
//         return "00";
//     }
//     return std::string();
// }

// std::string getOriginalCode(std::string value)
// {
//     if (value == "0")
//     {
//         return "00";
//     }
//     else if (value == "1")
//     {
//         return "01";
//     }
//     else if (value == "x")
//     {
//         return "10";
//     }
//     else if (value == "z")
//     {
//         return "11";
//     }
//     return std::string();
// }

// std::string getResultCode(bitset<2> res)
// {
//     if (res.none()) // 00
//     {
//         return "0";
//     }
//     else if (res.test(0) && (!res.test(1))) // 01
//     {
//         return "1";
//     }
//     else if ((!res.test(0)) && res.test(1)) // 10
//     {
//         return "x";
//     }
//     else if (res.all())
//     {
//         return "z";
//     }

//     return std::string();
// }
