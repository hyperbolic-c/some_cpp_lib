#include <iostream>

#include "time_diff_coding.h"

int main()
{
    TimeCoding example(10);  // the first time

    long long diff = example.getTimeDiff();
    example.addToAllTimeStr();
    std::cout << "time difference 1: " << diff << std::endl;
    
    example.updateCurrent(20);
    diff = example.getTimeDiff();
    std::cout << "time difference 2: " << diff << std::endl;
    example.addToAllTimeStr();
    std::cout << "time string: " << example.getAllTimeStr() << std::endl;
    return 0;
}