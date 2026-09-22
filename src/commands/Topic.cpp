#include "Server.hpp"
#include "Channel.hpp"

void handleTopic(Server &server, Client *client,const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();
	if (params.empty())
		 return reply(client, ":ircserv 461 " + nick + " TOPIC :Not enough parameters");

	const std::string name = params[0];

	Channel *channel = server.getChannel(name);
	if (channel == NULL)
		return reply(client, ":ircserv 403 " + nick + " " + name + " :No such channel");

	if (!channel->isMember(client))
		return reply(client, ":ircserv 442 " + nick + " " + name + " :You're not on that channel");

	if (params.size() == 1)
	{
		const std::string &topic = channel->getTopic();
		if (topic.empty())
			return reply(client, ":ircserv 331 " + nick + " " + name + " :No topic is set");
		else
			return reply(client, ":ircserv 332 " + nick + " " + name + " :" + topic);
	}

	if (channel->hasMode(MODE_TOPIC_RESTRICTED) && !channel->isOperator(client))
		return reply(client, ":ircserv 482 " + nick + " " + name + " :You're not channel operator");

	channel->setTopic(params[1]);
	const std::string prefix = nick + "!" + client->getUsername() + "@" + client->getHostname();
	const std::string msg = ":" + prefix + " TOPIC " + name + " :" + params[1];
	channel->broadcast(server, msg);
}
