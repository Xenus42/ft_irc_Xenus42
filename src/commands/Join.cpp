#include "Channel.hpp"
#include "Client.hpp"
#include <string>
#include <sys/socket.h>

// TODEL:
#include <iostream>

// Decoupe une liste "a,b,c" en elements individuels.
static std::vector<std::string> splitComma(const std::string &str)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type comma;
	while ((comma = str.find(',', start)) != std::string::npos)
	{
		result.push_back(str.substr(start, comma - start));
		start = comma + 1;
	}
	result.push_back(str.substr(start));
	return (result);
}

static bool isValidChannelName(const std::string &name)
{
	if (name.size() < 2 || name.size() > 50 || name[0] != '#')
		return (false);
	for (size_t i = 1; i < name.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(name[i]);
		if (c <= 32 || c == 127 || c == ',' || c == ':')
			return (false);
	}
	return (true);
}

// Joins a single channel for one JOIN command (name + optional key).
static void	joinOne(Server &server, Client *client, const std::string &nick,
		const std::string &name, const std::string &key)
{
	Channel	*channel;
	bool	created;

	if (!isValidChannelName(name))
	{
		server.sendMessage(client, ":ircserv 403 " + nick + " " + name
			+ " :No such channel");
		return ;
	}
	channel = server.getChannel(name);
	created = false;
	if (channel == NULL)
	{
		channel = server.createChannel(name);
		created = true;
	}
	if (channel->isMember(client))
		return ;
	if (!created)
	{
		if (channel->hasMode(MODE_HAS_KEY) && key != channel->getKey())
		{
			server.sendMessage(client, ":ircserv 475 " + nick + " " + name
				+ " :Cannot join channel (+k)");
			return ;
		}
		if (channel->hasMode(MODE_INVITE_ONLY) && !channel->isInvited(client))
		{
			server.sendMessage(client, ":ircserv 473 " + nick + " " + name
				+ " :Cannot join channel (+i)");
			return ;
		}
		if (channel->hasMode(MODE_HAS_LIMIT)
			&& static_cast<int>(channel->getClientCount()) >= channel->getUserLimit())
		{
			server.sendMessage(client, ":ircserv 471 " + nick + " " + name
				+ " :Cannot join channel (+l)");
			return ;
		}
	}
	channel->addClient(client);
	channel->removeInvited(client);
	if (created)
		channel->addOperator(client);
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	channel->broadcast(server, ":" + prefix + " JOIN " + channel->getName());
	if (!channel->getTopic().empty())
		server.sendMessage(client, ":ircserv 332 " + nick + " " + channel->getName() + " :"
			+ channel->getTopic());
	else
		server.sendMessage(client, ":ircserv 331 " + nick + " " + channel->getName()
			+ " :No topic is set");
	server.sendMessage(client, ":ircserv 353 " + nick + " = " + channel->getName() + " :"
		+ channel->getMemberList());
	server.sendMessage(client, ":ircserv 366 " + nick + " " + channel->getName() + " :End of /NAMES list");
}

// —  JOIN <#channel>{,<#channel>} [key{,key}]
void	handleJoin(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 461 " + nick
				+ " JOIN :Not enough parameters"));
	const std::vector<std::string> names = splitComma(params[0]);
	const std::vector<std::string> keys = (params.size() > 1) ? splitComma(params[1]) : std::vector<std::string>();
	for (std::vector<std::string>::size_type i = 0; i < names.size(); ++i)
	{
		const std::string key = (i < keys.size()) ? keys[i] : "";
		joinOne(server, client, nick, names[i], key);
	}
}
