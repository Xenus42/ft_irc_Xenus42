/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 10:25:11 by lebroue           #+#    #+#             */
/*   Updated: 2026/07/29 18:00:06 by lebroue          ###   ########.fr       */
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

// Renvoie true si le caractere separe des mots dans une ligne IRC.
static bool	isSeparator(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

// Enleve les separateurs en debut et en fin de chaine (pas au milieu).
static std::string removeSpaceBeginningAndEndString(const std::string &str)
{
	std::string::size_type start = 0;
	std::string::size_type end = str.size();
	while (start < end && isSeparator(str[start]))
		++start;
	while (end > start && isSeparator(str[end - 1]))
		--end;
	return (str.substr(start, end - start));
}

// Met une chaine en majuscules, les commandes IRC etant insensibles a la casse.
static std::string toUpperCase(const std::string &str)
{
	unsigned char	currentChar;
	char			upperChar;

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

// Cherche la position du premier separateur a partir de la position donnee.
// Renvoie std::string::npos si aucun separateur n'est trouve avant la fin.
static std::string::size_type findFirstSeparator(const std::string &str,
	std::string::size_type from)
{
	std::string::size_type i = from;
	while (i < str.size() && !isSeparator(str[i]))
		++i;
	if (i == str.size())
		return (std::string::npos);
	return (i);
}

// Extrait le prefixe optionnel (":nick!user@host ") en debut de ligne et
// renvoie le reste de la ligne (commande + parametres). Leve une exception
// si le prefixe est present mais malforme (vide, ou sans commande derriere).
static std::string extractPrefix(const std::string &input, std::string &prefix)
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

// Cherche le debut du texte libre marque par ':' (soit au tout debut de la
// chaine, soit juste apres un separateur). Renvoie la position du premier
// caractere du texte libre (juste apres le ':'), ou npos si absent.
static std::string::size_type findTextParamsStart(const std::string &input)
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

// Decoupe une chaine en mots simples, separes par des espaces/tabulations.
static std::vector<std::string> splitAndPutSimpleWordsIntoArray(const std::string &input)
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

// Decoupe la commande et ses parametres en mots. Un mot commencant par ':'
// marque le debut d'un texte libre : tout ce qui suit (espaces compris) est
// conserve tel quel comme un seul et dernier parametre.
static std::vector<std::string> splitCommandAndArgsToArray(const std::string &input)
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

Parsing::Command Parsing::parse(const std::string &line) const
{
	Command cmd;
	std::string input = removeSpaceBeginningAndEndString(line);

	if (input.empty())
		return (cmd);
	if (input.size() == 1 && input[0] == ':')
		throw std::runtime_error("Invalid IRC prefix: missing command");

	input = extractPrefix(input, cmd.prefix);

	std::vector<std::string> words = splitCommandAndArgsToArray(input);
	if (words.empty())
		return (cmd);

	cmd.name = toUpperCase(words[0]);
	std::vector<std::string>::size_type i = 1;
	while (i < words.size())
	{
		cmd.params.push_back(words[i]);
		++i;
	}
	return (cmd);
}

std::string Parsing::getCommandType(const Command &cmd) const
{
	if (isKnownCommand(cmd.name))
		return (cmd.name);
	return ("UNKNOWN");
}

bool Parsing::isKnownCommand(const std::string &name) const
{
	return (name == "PASS" || name == "NICK" || name == "USER" || name == "PING"
		|| name == "PONG" || name == "JOIN" || name == "PART" || name == "QUIT"
		|| name == "PRIVMSG" || name == "NOTICE" || name == "TOPIC"
		|| name == "MODE" || name == "KICK" || name == "WHO" || name == "LIST"
		|| name == "INVITE" || name == "CAP" || name == "AUTHENTICATE"
		|| name == "WHOIS");
}

void Parsing::trimSpaces(std::string &str) const
{
	str = removeSpaceBeginningAndEndString(str);
}
