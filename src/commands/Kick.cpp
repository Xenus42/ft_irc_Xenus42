#include "Client.hpp"
#include "Server.hpp"

void	handleKick(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.size() < 2) // a verifier encore... 
		return (reply(client, ":ircserv 461 " + nick
				+ " KICK :Not enough parameters"));
}
