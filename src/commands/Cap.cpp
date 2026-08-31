#include "Client.hpp"
#include "Server.hpp"

// CAP arrive avant NICK/USER : pas de nickname assigne, on utilise "*".
void	handleCap(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (reply(client, ":ircserv 461 " + nick
				+ " CAP :Not enough parameters"));
	const std::string subCommand = params[0];
	if (subCommand == "LS")
		return (reply(client, ":ircserv CAP " + nick + " LS :"));
	// CAP END: fin de negociation, aucune reponse attendue.
}
