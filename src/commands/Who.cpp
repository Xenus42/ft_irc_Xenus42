#include "Client.hpp"
#include "Server.hpp"

void	handleWho(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.empty())
		return (reply(client, ":ircserv 461 " + nick
				+ " WHO :Not enough parameters"));
	// TODO: chercher le channel (params[0]), lister ses membres (352),
	// puis envoyer le message de fin (315 :End of WHO list)
}
