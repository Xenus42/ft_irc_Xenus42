/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 10:25:11 by lebroue           #+#    #+#             */
/*   Updated: 2026/07/28 14:46:07 by lebroue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

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

Parsing::Command Parsing::parse(const std::string &line) const
{
	Command cmd;
	std::string input = removeSpaces(line);

	if (input.empty())
		return (cmd);

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

	if (!currentWord.empty())
		words.push_back(currentWord);

	if (words.empty())
		return (cmd);

	cmd.name = words[0];
	std::vector<std::string>::size_type j = 1;
	while (j < words.size())
	{
		cmd.params.push_back(words[j]);
		++j;
	}

	return (cmd);
}

void Parsing::trimSpaces(std::string &str) const
{
	str = removeSpaces(str);
}