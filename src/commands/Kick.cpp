#include "Channel.hpp"

void handleKick(Server &server, Client *client, const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();

	if (params.size() < 2)
		return reply(client, ":ircserv 461 " + nick + " KICK :Not enough parameters");
	const std::string channelName = params[0];
	const std::string targetNick = params[1];
	const std::string reason = (params.size() > 2) ? params[2] : "No reason given";
	Client *targetClient = server.getClientByNick(targetNick);
	if (targetClient == NULL)
		return reply(client, ":ircserv 401 " + nick + " " + targetNick + " :No such nick/channel");
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
		return reply(client, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
	if (!channel->isMember(client))
		return reply(client, ":ircserv 442 " + nick + " " + channelName + " :You're not on that channel");
	if (!channel->isOperator(client))
		return reply(client, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
	if (!channel->isMember(targetClient))
		return reply(client, ":ircserv 441 " + nick + " " + targetNick + " " + channelName + " :They aren't on that channel");
	channel->broadcast(":" + nick + "!" + client->getUsername() + "@" + client->getHostname() + " KICK " + channelName + " " + targetNick + " :" + reason);
	channel->removeMember(targetClient);

	if (channel->isEmpty())
		server.removeChannel(channelName);
}