#include "Parsing.hpp"
#include <cctype>

// Renvoie true si le caractere separe des mots dans une ligne IRC.
bool Parsing::isSeparator(char c) const
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

// Enleve les separateurs en debut et en fin de chaine (pas au milieu).
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

// Met une chaine en majuscules, les commandes IRC etant insensibles a la casse.
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

// Cherche la position du premier separateur a partir de la position donnee.
// Renvoie std::string::npos si aucun separateur n'est trouve avant la fin.
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

// Extrait le prefixe optionnel (":nick!user@host ") en debut de ligne et
// renvoie le reste de la ligne (commande + parametres). Leve une exception
// si le prefixe est present mais malforme (vide, ou sans commande derriere).
std::string Parsing::extractPrefix(const std::string &input,
	std::string &prefix) const
{

// Extrait le prefixe optionnel (":nick!user@host ") en debut de ligne et
// renvoie le reste de la ligne (commande + parametres). Leve une exception
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

// Decoupe une chaine en mots simples, separes par des espaces/tabulations.
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

// Decoupe la commande et ses parametres en mots. Un mot commencant par ':'
// marque le debut d'un texte libre : tout ce qui suit (espaces compris) est
// conserve tel quel comme un seul et dernier parametre.
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
