#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <string>

bool	isNumeric(std::string str)
{
	if (str.empty())
		return (false);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return (false);
	}
	return (true);
}

bool	isValidPort(const std::string &str)
{
	char	*end;
	long	port;

	if (!isNumeric(str))
		return (false);
	port = std::strtol(str.c_str(), &end, 10);
	if (*end != '\0')
		return (false);
	if (port < 1024 || port > 65535)
		return (false);
	return (true);
}

std::string toIrcUpper(const std::string &str)
{
	std::string result = str;

	for (std::string::size_type i = 0; i < result.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(result[i]);

		if (c >= 'a' && c <= 'z')
			result[i] = static_cast<char>(c - 'a' + 'A');
		else if (c == '{')
			result[i] = '[';
		else if (c == '}')
			result[i] = ']';
		else if (c == '|')
			result[i] = '\\';
	}
	return (result);
}

bool isValidPassword(const std::string &password)
{
    if (password.empty() || password.size() > 255)
        return (false);
    if (password.find(' ') != std::string::npos)
        return (false);
    if (password.find('\r') != std::string::npos)
        return (false);
    if (password.find('\n') != std::string::npos)
        return (false);
    return (true);
}