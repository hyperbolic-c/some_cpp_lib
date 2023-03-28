#include "binaryRL.h"

int main()
{
    std::string inputstring = "0000111000";
    std::string outputstring = "";
    std::string original = "";
    clock_t start, finish;
    start = clock();
    RLencode(inputstring, outputstring);
    std::cout << outputstring << std::endl;
    RLdecode(outputstring, original);
    std::cout << original << std::endl;
    finish = clock();
    std::cout << "time: " << finish - start << std::endl;
    return 0;
}