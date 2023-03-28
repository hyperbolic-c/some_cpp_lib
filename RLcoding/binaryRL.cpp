#include "binaryRL.h"

void RLencode(std::string &inputstring, std::string &outputstring)
{
    
    std::string res;
    int n = inputstring.length();
    outputstring = inputstring[0];  // the first char is 0 or 1
    for (unsigned int i = 0; i < n; i++)
    {
        // counting binary number
        int count = 1;
        while (inputstring[i] == inputstring[i + 1] && i < n)
        {
            ++count;
            ++i;
        }
        // directly save the count like 04 3 3(0000111000)
        outputstring += std::to_string(count);  
        outputstring += " ";
    }

    if (!outputstring.empty())
    {
        outputstring.pop_back();  // delete the last space
    }
}


void RLdecode(std::string &compressed, std::string &original)
{
    int repeat = 1;
    int binaryNum = *(compressed.begin()) - '0';
    
    compressed.erase(compressed.begin());  // delete the first flag(0 or 1)
    //compressed = compressed.substr(1);
    std::vector<std::string> splitRes;
    boost::split(splitRes, compressed, boost::is_any_of(" "), boost::token_compress_on);
    
    for (auto itBegin = splitRes.begin(); itBegin != splitRes.end(); ++itBegin)
    {
        if ((*itBegin).empty())
        {
            continue;
        }
        repeat = std::stoi(*itBegin);
        //std::string reString (repeat, binaryNum + '0');
        //original += reString;
        auto res = binaryNum + '0';
        while (repeat--)
        {
            original.push_back(res);
        }
        binaryNum = 1 + (-1)*binaryNum;
        // if (*itBegin != "\0")
        // {
            
        // }
        // repeat = *itBegin - '0';
        
    }
    
    // size_t pos = compressed.find(" ");
    // while (pos != compressed.npos)
    // {
    //     std::string temp = compressed.substr(0, pos);
    //     repeat = std::stoi(temp);
    //     while (repeat--)
    //     {
    //         original.push_back(binaryNum + '0');
    //     }
    //     binaryNum = 1 + (-1)*binaryNum;
    //     compressed = compressed.substr(pos + 1, compressed.size());
    //     pos = compressed.find(" ");
    // }
}
