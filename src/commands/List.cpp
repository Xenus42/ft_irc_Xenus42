#include "Client.hpp"
#include "Server.hpp"

// —  LIST [#channel]
void	handleList(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	(void)params;
	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	// TODO: envoyer 321 (debut de liste), parcourir server.getChannels(),
	// filtrer sur params[0] si fourni, envoyer une 322 par channel,
	// puis 323 (fin de liste)
}
