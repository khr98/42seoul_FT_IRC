#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>

bool checkNick(std::string str);
bool checkChannelName(std::string str);
bool isInclude(char c, std::string str);
std::vector<std::string> splitNewLine(Client cli, std::string input);
std::vector<std::string> tokenizeCmd(std::string input);

#endif
