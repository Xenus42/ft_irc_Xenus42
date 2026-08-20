#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	handleKick(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;

	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.size() < 2)
		return (reply(client, ":ircserv 461 " + nick
				+ " KICK :Not enough parameters"));
	const std::string name = params[0];
	channel = server.getChannel(name);
	if (channel == NULL)
		return (reply(client, ":ircserv 403 " + nick + " " + name
				+ " :No such channel"));
	if (!channel->isMember(client))
		return (reply(client, ":ircserv 442 " + nick + " " + name
				+ " :You're not on that channel"));

}
