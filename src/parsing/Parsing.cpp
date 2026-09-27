#include "Parsing.hpp"

Parsing::Parsing()
{
}

Parsing::Parsing(const Parsing &other)
{
	(void)other;
}

Parsing &Parsing::operator=(const Parsing &other)
{
	(void)other;
	return (*this);
}

Parsing::~Parsing()
{
}

Parsing::Command Parsing::parse(const std::string &line) const
{
	Command cmd;

	std::string input = line;
	if (input.empty())
		return (cmd);
	if (input.size() > MAX_LINE_LENGTH - 2)
		return (cmd);
	if (input.size() == 1 && input[0] == ':')
		throw std::runtime_error("Invalid IRC prefix: missing command");
	input = extractPrefix(input, cmd.prefix);
	std::vector<std::string> words = splitCommandAndArgsToArray(input);
	if (words.empty())
		return (cmd);
	cmd.name = toUpperCase(words[0]);
	std::vector<std::string>::size_type i = 1;
	while (i < words.size() && cmd.params.size() < MAX_PARAMS)
	{
		cmd.params.push_back(words[i]);
		++i;
	}
	return (cmd);
}
