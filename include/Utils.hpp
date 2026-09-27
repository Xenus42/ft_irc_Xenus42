#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

bool						isValidPort(const std::string &str);
bool						isValidPassword(const std::string &password);
std::string					toIrcUpper(const std::string &str);
std::vector<std::string>	splitComma(const std::string &str);

#endif
