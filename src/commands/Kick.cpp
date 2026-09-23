#include "Channel.hpp"
#include "Server.hpp"

// Decoupe une liste "a,b,c" en pseudos individuels.
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

// KICK <#channel> <nick>{,<nick>} [:reason]
void handleKick(Server &server, Client *client, const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();

	if (params.size() < 2)
		return reply(client, ":ircserv 461 " + nick + " KICK :Not enough parameters");
	const std::string channelName = params[0];
	const std::string reason = (params.size() > 2) ? params[2] : "No reason given";
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
		return reply(client, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
	if (!channel->isMember(client))
		return reply(client, ":ircserv 442 " + nick + " " + channelName + " :You're not on that channel");
	if (!channel->isOperator(client))
		return reply(client, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");

	const std::vector<std::string> targets = splitComma(params[1]);
	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i)
	{
		const std::string &targetNick = targets[i];
		Client *targetClient = server.getClientByNick(targetNick);

		if (targetClient == NULL)
		{
			reply(client, ":ircserv 401 " + nick + " " + targetNick + " :No such nick/channel");
			continue ;
		}
		if (!channel->isMember(targetClient))
		{
			reply(client, ":ircserv 441 " + nick + " " + targetNick + " " + channelName + " :They aren't on that channel");
			continue ;
		}
		channel->broadcast(server, ":" + nick + "!" + client->getUsername() + "@" + client->getHostname() + " KICK " + channelName + " " + targetNick + " :" + reason);
		channel->removeClient(targetClient);
	}

	if (channel->isEmpty())
		server.removeChannel(channelName);
}