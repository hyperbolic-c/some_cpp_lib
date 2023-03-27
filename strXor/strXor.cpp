#include "strXor.h"

// Function to insert n 0s in the
// beginning of the given string
void addZeros(std::string &str, int n)
{
    // for (int i = 0; i < n; i++)
    // {
    //     str = "0" + str;
    // }
    std::string addStr (n, '0');
    str = addStr + str;

}

// Function to return the XOR
// of the given strings
void getXOR(std::string &a, std::string &b, std::string &res)
{

    // Lengths of the given strings
    int aLen = a.length();
    int bLen = b.length();

    // Make both the strings of equal lengths
    // by inserting 0s in the beginning
    if (aLen > bLen)
    {
        addZeros(b, aLen - bLen);
    }
    else if (bLen > aLen)
    {
        addZeros(a, bLen - aLen);
    }

    // Updated length
    int len = std::max(aLen, bLen);

    // To store the resultant XOR
    // std::string res = "";
    for (int i = 0; i < len; i++)
    {
        if (a[i] == b[i])
            res += "0";
        else
            res += "1";
    }

    // return res;
}