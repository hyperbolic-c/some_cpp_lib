#include "signal_coding.h"

SignalEncoding::getSignalInfo(absl::string_view signal_info) {
  signal_info_ = signal_info;
}

/*vector signal functions*/

void SignalEncoding::addZeros(std::string &res, int width) {
  std::string addZero(width, '0');  // a string with "0"
  res = addZero + res;
}

void SignalEncoding::completeWidth(std::string &res, int width) {
  // complete and then output
  int len = res.length();
  if (len < width) {
    addZeros(res, width - len);
  }
}

void SignalEncoding::getSameBit(boost::dynamic_bitset<> &last_value_bit) {
  int last_len = last_value_bit.size();

  int current_len = value_info_.size();
  if (current_len > last_len) {
    // add 0 bit to the head of dynamic_bitset
    for (int i = 0; i < current_len - last_len; i++) {
      last_value_bit.push_back(false);
    }

  } else if (current_len < last_len) {
    addZeros(value_info_, last_len - current_len);
  }
}

void SignalEncoding::predictVector(boost::dynamic_bitset<> last_value_bit) {
  getSameBit(last_value_bit);
  xor_str_.clear();
  // invert the least significant bit as predict
  last_value_bit.flip(0);  // invert
  boost::dynamic_bitset<> current(value_info_);  // current value -> bitset
  boost::to_string(predict ^ current, xor_str_);  // get the xor string
}

void SignalEncoding::vectorSignalEncode() {
  std::vector<absl::string_view> split_res =
      absl::StrSplit(std::string(signal_info_), " ");
  absl::StrAppend(&all_signal_ID_,
                  split_res[1]);  // add the signal_ID to a string
  value_info_ = std::string(
      split_res[0].substr(1));  // string_view(b1000) to string(1000)

  auto end = vector_value_map_.end();
  auto value_item = vector_value_map_.find(split_res[1]);

  if (value_item != end) {
    // find the last signal value
    predictVector(value_item->second);  // predict coding
    boost::dynamic_bitset<> last_value_bit(value_info_);
    last_value_map_[split_res[1]] = last_value_bit;  // update the last value
    completeWidth(
        xor_str_,
        symbol_width_map_[split_res[1]]);  // get signal width to complete
    // xor_str_ have been completed as output
    absl::StrAppend(&all_signal_value_, xor_str_);  // output
    xor_str_.clear();
  } else {
    // not find, the signal firstly appear
    boost::dynamic_bitset<> last_value_bit(value_info_);
    last_value_map_[split_res[1]] = last_value_bit;  // add to the map
    completeWidth(value_info_, symbol_width_map_[split_res[1]]);
    // value_info_ have been completed as output
    absl::StrAppend(&all_signal_value_, value_info_);  // output
    value_info_.clear();
  }
}

/*scalar signal functions*/

void SignalEncoding::getScalarBit() {
  switch (signal_info_[0]) {
    case '0':
      value_info_ = "00";
      break;
    case '1':
      value_info_ = "01";
      break;
    case 'x':
      value_info_ = "10";
      break;
    case 'z':
      value_info_ = "11";
      break;
    default:
      std::cout << "This is not scalar signal type" << std::endl;
      break;
  }
}

void SignalEncoding::predictScalar(std::bitset<2> last_value_bit) {
  xor_str_.clear();
  std::bitset<2> current_value_bit(value_info_);
  // get xor result and then convert to string
  xor_str_ = (current_value_bit ^ last_value_bit).to_string;
}

void SignalEncoding::scalarSignalEncode() {
  // signal_info_ form as (0vG)
  symbol_info_ = signal_info_.substr(1);
  absl::StrAppend(&all_signal_ID_, symbol_info_);  // add the signal_ID to a string

  auto end = scalar_value_map_.end();
  auto value_item = scalar_value_map_.find(symbol_info_);
  if (value_item != end) {
    // find the last signal value
    predictScalar(value_item->second)
    // output
  } else {
    // not find
    getScalarBit();
    std::bitset<2> last_value_bit(value_info_);
    scalar_value_map_[symbol_info_] = last_value_bit;
    // output (value_info_)
  }
}