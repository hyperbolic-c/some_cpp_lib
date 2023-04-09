#ifndef SIGNAL_CODING_H
#define SIGNAL_CODING_H

#include <math.h>

#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"

class SignalEncoding {
 private:
  // one time variable
  absl::string_view signal_info_;  // string_view(b1000 vG)
  std::string value_info_;         // from string_view(b1000 vG) to string(1000)
  absl::string_view symbol_info_;  // save the symbol string
  std::string all_signal_ID_;      // save this time all signal symbol
  std::string xor_str_;            // save the vector signal xor result

  // many times variable
  std::string all_signal_value_;  // the buffer to save some encoded value
  std::string encode_value_;      // save the value run length encode result

  std::map<std::string, int> symbol_width_map_;  // symbol: width
  std::map<absl::string_view, boost::dynamic_bitset<>>
      vector_value_map_;  // vector signal symbol: value bitset
  std::map<absl::string_view, std::bitset<2>>
      scalar_value_map_;  // scalar signal symbol: value bitset

  // vector signal encoding functions
  void addZeros(std::string &res, int width);  // add zeros in front of value
  void getSameBit(
      boost::dynamic_bitset<> &last_value_bit);  // get the same length for xor
  void predictVector(
      boost::dynamic_bitset<> last_value_bit);  // transform to bitset to xor
  void completeWidth(std::string &res,
                     int width);  // complete with signal width
  void vectorSignalEncode();

  // scalar signal encoding functions
  void getScalarBit();
  void predictScalar(std::bitset<2> last_value_bit);
  void scalarSignalEncode();

  // real signal enconding functions
  void realSignalEncode();

  // binary string run length encoding
  void binaryStrRLEncode();

 public:
  SignalEncoding(){};
  //~signalCoding();
  // get one time signal value info
  void getSignalInfo(absl::string_view signal_info);

  void encodeSignal();
};

void SignalEncoding::binaryStrRLEncode() {
  int repeat = 1;
  int count = 1;
  int len = all_signal_value_.length();
  // encode_value_.clear();  // when encode_value_ into next step then clear
  for (unsigned int i = 0; i < len; ++i) {
    count = 1;
    while (all_signal_value_[i] == all_signal_value_[i + 1] && i < len) {
      ++count;
      ++i;
    }
    absl::StrAppend(&encode_value_, count, " ");
  }
  absl::StrAppend(&encode_value_, all_signal_value_[0]);
  // (11110001111) -> (4 3 4 1) the end num as symbol save the start(1 or 0)
  all_signal_value_.clear();
}

void SignalEncoding::realSignalEncode() {
  // real signal(r50 A4)
  std::vector<absl::string_view> split_res =
      absl::StrSplit(std::string(signal_info_), " ");
  absl::StrAppend(&all_signal_ID_,
                  split_res[1]);  // add the signal_ID to a string
  value_info_ = split_res[0];
  // directly output r50
}

void SignalEncoding::encodeSignal() {
  switch (signal_info_[0]) {
    case 'b':
      // encode vector signal
      encodeVectorSignal();
      break;
    case '0':
    case '1':
    case 'x':
    case 'z':
      // encode scalar signal
      scalarSignalEncode();
      break;
    case 'r':
      // encode real signal(r50) and carry on next coding step
      /*encode form: 12 11 220 r50 r50 11 13 240
        take r50 as the run length encode ending symbol
        when decoding, use absl::StrSplit("12 11 220 r50 r50 11 13 240", " r50",
        absl::SkipWhitespace()) Uses absl::SkipWhitespace() to skip all strings
        that are either empty or contain only whitespace.
      */
      realSignalEncode();
      binaryStrRLEncode();  // get the value run length result
      absl::StrAppend(&encode_value_, " ", value_info_);
      break;
    default:
      std::cout << "This is a error value type" << std::endl;
      break;
  }

  // when achieve the buffer size then run length coding
}

#endif