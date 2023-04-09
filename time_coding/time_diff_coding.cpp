#include "time_diff_coding.h"

TimeCoding::TimeCoding(long long current_)
{
    current_time_ = current_;
    previous_time_ = 0;
    time_diff_ = 0;
    all_time_str_ = "";
}

long long TimeCoding::getTimeDiff()
{
    time_diff_ = current_time_ - previous_time_;
    previous_time_ = current_time_;  // update previous time
    return time_diff_;
}

// void TimeCoding::updatePrevious()
// {
//     previous_time_ = current_time_;
// }

void TimeCoding::updateCurrent(long long current_)
{
    current_time_ = current_;
}

void TimeCoding::addToAllTimeStr()
{
    all_time_str_ += std::to_string(time_diff_);
    all_time_str_ += " ";
}

std::string TimeCoding::getAllTimeStr()
{
    return all_time_str_;
}



