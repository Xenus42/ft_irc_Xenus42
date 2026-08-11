#include "Channel.hpp"
#include "Client.hpp"

void handleInvite(Server &server, Client *client, const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();

	if (params.size() < 2)
		return reply(client, ":ircserv 461 " + nick + " INVITE :Not enough parameters");
	const std::string targetNick = params[0];
	const std::string channelName = params[1];
	Client *targetClient = server.getClientByNick(targetNick);
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
		return reply(client, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
	if (targetNick == nick)
		return reply(client, ":ircserv 442 " + nick + " " + channelName + " :You cannot invite yourself");
	if (targetClient == NULL)
		return reply(client, ":ircserv 401 " + nick + " " + targetNick + " :No such nick/channel");
	if (channel->hasMode(MODE_INVITE_ONLY) && !channel->isOperator(client))
		return reply(client, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
	if (channel->isMember(targetClient))
		return reply(client, ":ircserv 443 " + nick + " " + targetNick + " " + channelName + " :is already on channel");
	channel->addInvited(targetClient);
	reply(client, ":ircserv 341 " + nick + " " + targetNick + " " + channelName);
	reply(targetClient, ":" + nick + "!" + client->getUsername() + "@" + client->getHostname() + " INVITE " + targetNick + " :" + channelName);
}