#ifndef TIME_DIFF_CODING_H
#define TIME_DIFF_CODING_H

#include <string>

class TimeCoding
{
private:
    long long time_diff_, current_time_, previous_time_;
    std::string all_time_str_;

public:
    TimeCoding(long long current_);
    // ~TimeCoding();

    long long getTimeDiff();
    // void updatePrevious();  // update in getTimeDiff()
    void updateCurrent(long long current_);
    void addToAllTimeStr();
    std::string getAllTimeStr();
};

#endif