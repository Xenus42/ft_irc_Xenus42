/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 10:25:11 by lebroue           #+#    #+#             */
/*   Updated: 2026/07/28 19:12:09 by lebroue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"
#include <cctype>

Parsing::Parsing()
{
}

Parsing::~Parsing()
{
}

static std::string removeSpaces(const std::string &str)
{
	std::string::size_type start = 0;
	std::string::size_type end = str.size();
	while (start < end && (str[start] == ' ' || str[start] == '\t'
			|| str[start] == '\r' || str[start] == '\n'))
		++start;
	while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t'
			|| str[end - 1] == '\r' || str[end - 1] == '\n'))
		--end;
	return (str.substr(start, end - start));
}

static bool	isSeparator(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static std::string toUpperCase(const std::string &str)
{
	std::string result;
	std::string::size_type i = 0;
	while (i < str.size())
	{
		result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(str[i]))));
		++i;
	}
	return (result);
}

Parsing::Command Parsing::parse(const std::string &line) const
{
	Command cmd;
	std::string input = removeSpaces(line);

	if (input.empty())
		return (cmd);
	if (input[0] == ':')
	{
		std::string::size_type prefixEnd = input.find_first_of(" \t\r\n", 1);
		if (prefixEnd == std::string::npos)
			throw std::runtime_error("Invalid IRC prefix: missing command");
		cmd.prefix = input.substr(1, prefixEnd - 1);
		input = removeSpaces(input.substr(prefixEnd));
		if (input.empty())
			throw std::runtime_error("Invalid IRC line: empty command after prefix");
	}

	std::string currentWord;
	std::vector<std::string> words;
	bool textParams = false;
	std::string::size_type i = 0;

	while (i < input.size())
	{
		if (textParams)
		{
			currentWord += input[i];
		}
		else if (input[i] == ':' && currentWord.empty())
		{
			textParams = true;
		}
		else if (isSeparator(input[i]))
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

	if (textParams)
		words.push_back(currentWord);
	else if (!currentWord.empty())
		words.push_back(currentWord);

	if (words.empty())
		return (cmd);

	cmd.name = toUpperCase(words[0]);
	std::vector<std::string>::size_type j = 1;
	while (j < words.size())
	{
		cmd.params.push_back(words[j]);
		++j;
	}

	return (cmd);
}

std::string Parsing::getCommandType(const Command &cmd) const
{
	if (cmd.name == "PASS" || cmd.name == "NICK" || cmd.name == "USER"
		|| cmd.name == "PING" || cmd.name == "JOIN" || cmd.name == "PART"
		|| cmd.name == "QUIT" || cmd.name == "PRIVMSG" || cmd.name == "NOTICE"
		|| cmd.name == "TOPIC" || cmd.name == "MODE" || cmd.name == "KICK"
		|| cmd.name == "WHO" || cmd.name == "LIST" || cmd.name == "INVITE")
		return (cmd.name);
	return ("UNKNOWN");
}

void Parsing::trimSpaces(std::string &str) const
{
	str = removeSpaces(str);
}