#include <iostream>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <climits>

bool isNumeric(std::string str)
{
    if (str.empty())
        return (false);
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!std::isdigit(str[i]))
            return (false);
    }
    return (true);
}

bool isValidPort(const std::string &str)
{
    if (!isNumeric(str))
        return false;

    char *end;
    long port = std::strtol(str.c_str(), &end, 10);
    if (*end != '\0')
        return false;
    if (port < 1024 || port > 65535)
        return false;
    return true;
}