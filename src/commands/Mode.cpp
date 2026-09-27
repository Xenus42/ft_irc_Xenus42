#include "Commands.hpp"
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sstream>
struct		ModeChange
{
	bool	add;
	char	mode;
	std::string arg;

	ModeChange(bool a, char m) : add(a), mode(m)
	{
	}
};

static bool	isValidMode(char mode)
{
	return (mode == 'i' || mode == 't' || mode == 'k' || mode == 'o'
		|| mode == 'l');
}

static bool	takesArg(char mode, bool add)
{
	if (mode == 'o' || mode == 'k')
		return (true);
	else if (mode == 'l')
		return (add);
	return (false);
}

//MODE #douv +i -> ModeChange{add: +, mode: 'i', arg: ""}
//MODE #douv +ol-k bob 42 -> ModeChange{+, o, "bob"}, {+, l, "42"}, {-, k, ""}
static std::vector<ModeChange> parseModeString(Server &server, Client *client,
	const std::string &modes, const std::vector<std::string> &args)
{
	bool	add;
	size_t	indexArg;

	std::vector<ModeChange> changes;
	std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	add = true;
	indexArg = 0;
	for (size_t i = 0; i < modes.size(); ++i)
	{
		if (modes[i] == '+')
			add = true;
		else if (modes[i] == '-')
			add = false;
		else if (!isValidMode(modes[i]))
			server.sendMessage(client, ":ircserv 472 " + nick + " " + modes[i]
				+ " :is unknown mode char to me");
		else
		{
			ModeChange tmp(add, modes[i]);
			if (takesArg(tmp.mode, tmp.add))
			{
				if (indexArg < args.size())
				{
					tmp.arg = args[indexArg];
					indexArg++;
				}
				else if (!(tmp.mode == 'k' && !tmp.add))
				{
					server.sendMessage(client, ":ircserv 461 " + nick
						+ " MODE :Not enough parameters");
					continue ;
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

static bool	applyMode(Server &server, Client *client, Channel *channel,
		const ModeChange &mc)
{
	char	*end;
	long	limit;
	Client	*target;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	switch (mc.mode)
	{
	case 't':
		if (mc.add)
			channel->setMode(MODE_TOPIC_RESTRICTED);
		else
			channel->unsetMode(MODE_TOPIC_RESTRICTED);
		return (true);
	case 'i':
		if (mc.add)
			channel->setMode(MODE_INVITE_ONLY);
		else
			channel->unsetMode(MODE_INVITE_ONLY);
		return (true);
	case 'l':
	{
		if (!mc.add)
			return (channel->removeUserLimit(), true);
		if (mc.arg.empty())
			return (server.sendMessage(client, ":ircserv 461 " + nick
					+ " MODE :Not enough parameters"), false);
		for (size_t i = 0; i < mc.arg.size(); i++)
			if (!isdigit(static_cast<unsigned char>(mc.arg[i])))
				return (server.sendMessage(client, ":ircserv 461 " + nick
						+ " MODE :Not enough parameters"), false);
		errno = 0;
		end = NULL;
		limit = std::strtol(mc.arg.c_str(), &end, 10);
		if (errno == ERANGE || limit <= 0 || limit > INT_MAX)
			return (false);
		return (channel->setUserLimit(static_cast<int>(limit)), true);
	}
	case 'k':
		if (!mc.add)
			return (channel->removeKey(), true);
		if (mc.arg.empty())
			return (server.sendMessage(client, ":ircserv 461 " + nick
					+ " MODE :Not enough parameters"), false);
		if (channel->hasMode(MODE_HAS_KEY))
			return (server.sendMessage(client, ":ircserv 467 " + nick + " "
					+ channel->getName() + " :Channel key already set"), false);
		return (channel->setKey(mc.arg), true);
	case 'o':
	{
		target = server.getClientByNick(mc.arg);
		if (target == NULL)
			return (server.sendMessage(client, ":ircserv 401 " + nick + " " + mc.arg
					+ " :No such nick/channel"), false);
		if (!channel->isMember(target))
			return (server.sendMessage(client, ":ircserv 441 " + nick + " " + mc.arg + " "
					+ channel->getName() + " :They aren't on that channel"),
				false);
		if (mc.add)
			return (channel->addOperator(target), true);
		return (channel->removeOperator(target), true);
	}
	}
	server.sendMessage(client, ":ircserv 472 " + nick + " " + std::string(1, mc.mode)
		+ " :is unknown mode char to me");
	return (false);
}

void	handleMode(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	char	sign;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 461 " + nick
				+ " MODE :Not enough parameters"));
	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&'))
		return (server.sendMessage(client, ":ircserv 502 " + nick
				+ " :Cannot change mode for other users"));
	const std::string name = params[0];
	channel = server.getChannel(name);
	if (channel == NULL)
		return (server.sendMessage(client, ":ircserv 403 " + nick + " " + name
				+ " :No such channel"));
	if (params.size() < 2 || params[1].empty())
		return (server.sendMessage(client, ":ircserv 324 " + nick + " " + name + " "
				+ channelModeReply(channel, channel->isMember(client))));
	if (!channel->isMember(client))
		return (server.sendMessage(client, ":ircserv 442 " + nick + " " + name
				+ " :You're not on that channel"));
	else if (!channel->isOperator(client))
		return (server.sendMessage(client, ":ircserv 482 " + nick + " " + name
				+ " :You're not channel operator"));
	std::vector<std::string> args(params.begin() + 2, params.end());
	std::vector<ModeChange> modeChanges = parseModeString(server, client, params[1], args);
	std::string modes, modeArgs;
	sign = '\0';
	for (size_t i = 0; i < modeChanges.size(); i++)
	{
		const ModeChange &mc = modeChanges[i];
		const char cur = (mc.add ? '+' : '-');

		if (!applyMode(server, client, channel, mc))
			continue ;
		if (sign != cur)
		{
			sign = cur;
			modes += sign;
		}
		modes += mc.mode;
		if (mc.mode == 'o' || (mc.add && (mc.mode == 'k' || mc.mode == 'l')))
			modeArgs += " " + mc.arg;
	}
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	if (!modes.empty())
		channel->broadcast(server, ":" + prefix + " MODE " + channel->getName()
			+ " " + modes + modeArgs);
}
