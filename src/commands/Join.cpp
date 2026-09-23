#include "Channel.hpp"
#include "Client.hpp"
#include <string>
#include <sys/socket.h>

// TODEL:
#include <iostream>

void	reply(Client *client, const std::string &line)
{
	Server	serv;

	std::string full = line + "\r\n";
	serv.sendMessage(client, full);
}

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

// Rejoint une seule commande JOIN sur un channel donne (nom + cle optionnelle).
static void	joinOne(Server &server, Client *client, const std::string &nick,
		const std::string &name, const std::string &key)
{
	Channel	*channel;
	bool	created;

	if (name.empty() || name[0] != '#')
	{
		reply(client, ":ircserv 403 " + nick + " " + name
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
			reply(client, ":ircserv 475 " + nick + " " + name
				+ " :Cannot join channel (+k)");
			return ;
		}
		if (channel->hasMode(MODE_INVITE_ONLY) && !channel->isInvited(client))
		{
			reply(client, ":ircserv 473 " + nick + " " + name
				+ " :Cannot join channel (+i)");
			return ;
		}
		if (channel->hasMode(MODE_HAS_LIMIT)
			&& static_cast<int>(channel->getClientCount()) >= channel->getUserLimit())
		{
			reply(client, ":ircserv 471 " + nick + " " + name
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
	channel->broadcast(server, ":" + prefix + " JOIN " + name);
	if (!channel->getTopic().empty())
		reply(client, ":ircserv 332 " + nick + " " + name + " :"
			+ channel->getTopic());
	else
		reply(client, ":ircserv 331 " + nick + " " + name
			+ " :No topic is set");
	reply(client, ":ircserv 353 " + nick + " = " + name + " :"
		+ channel->getMemberList());
	reply(client, ":ircserv 366 " + nick + " " + name + " :End of /NAMES list");
}

// —  JOIN <#channel>{,<#channel>} [key{,key}]
void	handleJoin(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();
	if (!client->isRegistered())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.empty())
		return (reply(client, ":ircserv 461 " + nick
				+ " JOIN :Not enough parameters"));
	const std::vector<std::string> names = splitComma(params[0]);
	const std::vector<std::string> keys = (params.size() > 1) ? splitComma(params[1]) : std::vector<std::string>();
	for (std::vector<std::string>::size_type i = 0; i < names.size(); ++i)
	{
		const std::string key = (i < keys.size()) ? keys[i] : "";
		joinOne(server, client, nick, names[i], key);
	}
}
