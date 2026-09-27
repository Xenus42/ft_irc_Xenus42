#include "Commands.hpp"
#include "Utils.hpp"

// — KICK <#channel> <nick>{,<nick>} [:reason]
void handleKick(Server &server, Client *client, const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();

	if (params.size() < 2)
		return server.sendMessage(client, ":ircserv 461 " + nick + " KICK :Not enough parameters");
	const std::string channelName = params[0];
	const std::string reason = (params.size() > 2) ? params[2] : "No reason given";
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
		return server.sendMessage(client, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
	if (!channel->isMember(client))
		return server.sendMessage(client, ":ircserv 442 " + nick + " " + channelName + " :You're not on that channel");
	if (!channel->isOperator(client))
		return server.sendMessage(client, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");

	const std::vector<std::string> targets = splitComma(params[1]);
	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i)
	{
		const std::string &targetNick = targets[i];
		Client *targetClient = server.getClientByNick(targetNick);

		if (!channel->isOperator(client))
			break;
		if (targetClient == NULL)
		{
			server.sendMessage(client, ":ircserv 401 " + nick + " " + targetNick + " :No such nick/channel");
			continue ;
		}
		if (!channel->isMember(targetClient))
		{
			server.sendMessage(client, ":ircserv 441 " + nick + " " + targetNick + " " + channelName + " :They aren't on that channel");
			continue ;
		}
		channel->broadcast(server, ":" + nick + "!" + client->getUsername() + "@" + client->getHostname() + " KICK " + channel->getName() + " " + targetClient->getNickname() + " :" + reason);
		channel->removeClient(targetClient);
	}

	if (channel->isEmpty())
		server.removeChannel(channelName);
}
