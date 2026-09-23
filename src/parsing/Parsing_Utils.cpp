#include "Parsing.hpp"
#include <cctype>

// Returns true if the character separates words in an IRC line.
bool Parsing::isSeparator(char c) const
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

// Removes separators at the start and end of the string (not in the middle).
std::string Parsing::removeSpaceBeginningAndEndString(const std::string &str) const
{
	std::string::size_type start = 0;
	std::string::size_type end = str.size();
	while (start < end && isSeparator(str[start]))
		++start;
	while (end > start && isSeparator(str[end - 1]))
		--end;
	return (str.substr(start, end - start));
}

// Uppercases a string, since IRC commands are case-insensitive.
std::string Parsing::toUpperCase(const std::string &str) const
{
	unsigned char currentChar;
	char upperChar;

	std::string result;
	std::string::size_type i = 0;
	while (i < str.size())
	{
		currentChar = static_cast<unsigned char>(str[i]);
		upperChar = static_cast<char>(std::toupper(currentChar));
		result.push_back(upperChar);
		++i;
	}
	return (result);
}

// Finds the position of the first separator from the given position.
// Returns std::string::npos if no separator is found before the end.
std::string::size_type Parsing::findFirstSeparator(const std::string &str,
	std::string::size_type from) const
{
	std::string::size_type i = from;
	while (i < str.size() && !isSeparator(str[i]))
		++i;
	if (i == str.size())
		return (std::string::npos);
	return (i);
}

// Extracts the optional prefix (":nick!user@host ") at the start of the line
// and returns the rest of the line (command + parameters). Throws an
// exception if the prefix is present but malformed (empty, or no command after).
std::string Parsing::extractPrefix(const std::string &input,
	std::string &prefix) const
{
	std::string commandPart;
	if (input[0] != ':')
		return (input);
	std::string::size_type prefixEnd = findFirstSeparator(input, 1);
	if (prefixEnd == std::string::npos)
		throw std::runtime_error("Invalid IRC prefix: missing command");
	prefix = input.substr(1, prefixEnd - 1);
	if (prefix.empty())
		throw std::runtime_error("Invalid IRC prefix: empty prefix");
	commandPart = removeSpaceBeginningAndEndString(input.substr(prefixEnd));
	if (commandPart.empty())
		throw std::runtime_error("Invalid IRC line: empty command after prefix");
	return (commandPart);
}

// Finds the start of free text marked by ':' (either at the very start of
// the string, or right after a separator). Returns the position of the first
// character of the free text (right after the ':'), or npos if absent.
std::string::size_type Parsing::findTextParamsStart(const std::string &input) const
{
	std::string::size_type i;
	if (!input.empty() && input[0] == ':')
		return (1);
	i = 0;
	while (i < input.size())
	{
		if (isSeparator(input[i]) && i + 1 < input.size() && input[i
			+ 1] == ':')
			return (i + 2);
		++i;
	}
	return (std::string::npos);
}

// Splits a string into simple words, separated by spaces/tabs.
std::vector<std::string> Parsing::splitAndPutSimpleWordsIntoArray(const std::string &input) const
{
	std::vector<std::string> words;
	std::string currentWord;
	std::string::size_type i = 0;
	while (i < input.size())
	{
		if (isSeparator(input[i]))
		{
			if (!currentWord.empty())
			{
				words.push_back(currentWord);
				currentWord.clear();
			}
		}
		else
			currentWord += input[i];
		++i;
	}
	if (!currentWord.empty())
		words.push_back(currentWord);
	return (words);
}

// Splits the command and its parameters into words. A word starting with ':'
// marks the start of free text: everything that follows (including spaces)
// is kept as-is as a single, last parameter.
std::vector<std::string> Parsing::splitCommandAndArgsToArray(const std::string &input) const
{
	std::string::size_type textParamsStart = findTextParamsStart(input);
	std::vector<std::string> words;
	if (textParamsStart == std::string::npos)
		return (splitAndPutSimpleWordsIntoArray(input));
	words = splitAndPutSimpleWordsIntoArray(input.substr(0, textParamsStart
				- 1));
	words.push_back(input.substr(textParamsStart));
	return (words);
}
