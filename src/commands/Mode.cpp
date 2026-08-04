#include "Server.hpp"

static struct modechange
{
	bool add;
	char mode;
	std::string arg;

	modechange(bool a, char m) : add(a), mode(m){}
};

static bool isValidMode(char mode)
{
	return (mode == 'i' || mode == 't' || mode == 'k' || mode == 'o' || mode == 'l');
}

static bool takesArg(char mode, bool add)
{
	if (mode == 'o' || mode == 'k')
		return true;
	else if (mode == 'l')
		return add;
	return false;
}
/*
MODE #douv +i -> modechange{add: +, mode: 'i', arg: ""}
MODE #douv +ol-k bob 42 -> modechange{+, o, "bob"}, {+, l, "42"}, {-, k, ""}
*/
static std::vector<modechange> parseModeString(Client *client, const std::string &modes, const std::vector<std::string> &args)
{
	std::vector<modechange> changes;
	std::string nick = client->getNickname();
	bool add = true;
	size_t indexArg = 0;

	for (size_t i = 0; i < modes.size(); ++i)
	{
		if (modes[i] == '+')
			add = true;
		else if (modes[i] == '-')
			add = false;
		else if (!isValidMode(modes[i]))
			reply(client, ":ircserv 472 " + nick + " " + modes[i] + " :is unknown mode char to me");
		else
		{
			modechange tmp(add, modes[i]);
			if (takesArg(tmp.mode, tmp.add))
			{
				if (indexArg < args.size())
				{
					tmp.arg = args[indexArg];
					indexArg++;
				}
				else if (!(tmp.mode == 'k' && !tmp.add))
				{
					reply(client, ":ircserv 461 " + nick + " MODE :Not enough parameters");
					continue;
				}
			}
			changes.push_back(tmp);
		}
	}
	return (changes);
}

void handleMode(Server &server, Client *client, const std::vector<std::string> &params)
{

}