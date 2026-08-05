#include "Server.hpp"
#include <sstream>
#include <iostream>

struct modechange
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

static std::string channelModeReply(Channel *channel, bool isMember)
{
	std::string modes = "+";
	std::string args;

	if (channel->hasMode(MODE_INVITE_ONLY))
		modes += 'i';
	if (channel->hasMode(MODE_TOPIC_RESTRICTED))
		modes += 't';
	if (channel->hasMode(MODE_HAS_KEY))
	{
		modes += 'k';
		if (isMember)
			args += " " + channel->getKey();
	}
	if (channel->hasMode(MODE_HAS_LIMIT))
	{
		std::ostringstream oss;
		oss << channel->getUserLimit();
		modes += 'l';
		args += " " + oss.str();
	}
	return (modes + args);
}

void handleMode(Server &server, Client *client, const std::vector<std::string> &params)
{
	std::cerr << "handleMode called" << std::endl;
	const std::string nick = client->getNickname();

	if (params.empty())
		return reply(client, ":ircserv 461 " + nick + " MODE :Not enough parameters");

	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&'))
		return reply(client, ":ircserv 502 " + nick + " :Cannot change mode for other users");

	const std::string name = params[0];
	Channel *channel = server.getChannel(name);
	if (channel == NULL)
		return reply(client, ":ircserv 403 " + nick + " " + name + " :No such channel");

	if (params.size() < 2 || params[1].empty())
		return reply(client, ":ircserv 324 " + nick + " " + name + " "
			+ channelModeReply(channel, channel->isMember(client)));

	if (!channel->isMember(client))
		return reply(client, ":ircserv 442 " + nick + " " + name + " :You're not on that channel");
	else if (!channel->isOperator(client))
		return reply(client, ":ircserv 482 " + nick + " " + name + " :You're not channel operator");

	std::vector<std::string> args(params.begin() + 2, params.end());
	std::vector<modechange> vecmode = parseModeString(client, params[1], args);
	
}