#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	handleWho(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;

	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.empty())
		return (reply(client, ":ircserv 461 " + nick
				+ " WHO :Not enough parameters"));
	const std::string name = params[0];
	channel = server.getChannel(name);
	if (channel == NULL)
		return (reply(client, ":ircserv 315 " + nick + " " + name
				+ " :End of WHO list"));
	// TODO: lister les membres du channel (352),
	// puis envoyer le message de fin (315 :End of WHO list)
}
