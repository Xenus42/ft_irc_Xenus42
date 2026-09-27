#include "Commands.hpp"

void	handlePass(Server &server, Client *client, const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();

	if (params.empty())
		return server.sendMessage(client, ":ircserv 461 " + nick + " PASS :Not enough parameters");

	if (client->isRegistered())
		return server.sendMessage(client, ":ircserv 462 " + nick + " :You may not reregister");

	if (params[0] != server.getPassword())
	{
		client->setAuthenticated(false);
		return server.sendMessage(client, ":ircserv 464 " + nick + " :Password incorrect");
	}

	client->setAuthenticated(true);
	if (client->isRegistered())
		server.sendWelcome(client);
}
