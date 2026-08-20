#include "Client.hpp"
#include "Server.hpp"

void	handleWhois(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.empty())
		return (reply(client, ":ircserv 431 " + nick + " :No nickname given"));
	// TODO: chercher le client cible par pseudo (getClientByNick)
	// si absent : 401 :No such nick/channel
	// sinon : envoyer 311 (whoisuser), 312 (whoisserver), 318 (end of whois)
}
