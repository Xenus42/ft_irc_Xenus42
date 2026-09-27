#include "Commands.hpp"

void handleInvite(Server &server, Client *client, const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();

	if (params.size() < 2)
		return server.sendMessage(client, ":ircserv 461 " + nick + " INVITE :Not enough parameters");
	const std::string targetNick = params[0];
	const std::string channelName = params[1];
	Client *targetClient = server.getClientByNick(targetNick);
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
		return server.sendMessage(client, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
	if (targetClient == NULL)
		return server.sendMessage(client, ":ircserv 401 " + nick + " " + targetNick + " :No such nick/channel");
	if (!channel->isMember(client))
		return server.sendMessage(client, ":ircserv 442 " + nick + " " + channelName + " :You're not on that channel");
	if (channel->hasMode(MODE_INVITE_ONLY) && !channel->isOperator(client))
		return server.sendMessage(client, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
	if (channel->isMember(targetClient))
		return server.sendMessage(client, ":ircserv 443 " + nick + " " + targetNick + " " + channelName + " :is already on channel");
	channel->addInvited(targetClient);
	server.sendMessage(client, ":ircserv 341 " + nick + " " + targetNick + " " + channelName);
	server.sendMessage(targetClient, ":" + nick + "!" + client->getUsername() + "@" + client->getHostname() + " INVITE " + targetNick + " :" + channelName);
}
