#include "Parsing.hpp"

Parsing::Parsing()
{
}

Parsing::~Parsing()
{
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
