#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <time.h>

void RLencode(std::string &inputstring, std::string &outputstring);

void RLdecode(std::string &compressed, std::string &original);