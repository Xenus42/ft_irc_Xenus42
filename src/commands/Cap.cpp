#include "Client.hpp"
#include "Server.hpp"

// CAP arrives before NICK/USER: no nickname assigned yet, use "*".
void	handleCap(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 461 " + nick
				+ " CAP :Not enough parameters"));
	const std::string subCommand = params[0];
	if (subCommand == "LS")
		return (server.sendMessage(client, ":ircserv CAP " + nick + " LS :"));
	// CAP END: fin de negociation, aucune reponse attendue.
}
