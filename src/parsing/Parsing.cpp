/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 10:25:11 by lebroue           #+#    #+#             */
/*   Updated: 2026/07/28 13:39:47 by lebroue          ###   ########.fr       */
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
	while (start < end && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n'))
		++start;
	while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t' || str[end - 1] == '\r' || str[end - 1] == '\n'))
		--end;
	return (str.substr(start, end - start));
}

Parsing::Command Parsing::parse(const std::string &line) const
{
	Command cmd;
	std::string input = removeSpaces(line);

	if (input.empty())
		return (cmd);

	std::string currentWord;
	std::vector<std::string> words;

	for (std::string::size_type i = 0; i < input.size(); ++i)
	{
		if (input[i] == ' ' || input[i] == '\t' || input[i] == '\r' || input[i] == '\n')
		{
			if (!currentWord.empty())
			{
				words.push_back(currentWord);
				currentWord.clear();
			}
		}
		else
			currentWord += input[i];
	}

	if (!currentWord.empty())
		words.push_back(currentWord);

	if (words.empty())
		return (cmd);

	cmd.name = words[0];
	for (std::vector<std::string>::size_type i = 1; i < words.size(); ++i)
		cmd.params.push_back(words[i]);

	return (cmd);
}

void Parsing::trimSpaces(std::string &str) const
{
	str = removeSpaces(str);
}