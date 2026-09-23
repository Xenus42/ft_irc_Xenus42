#include "Client.hpp"
#include "Server.hpp"

// —  PING <token>
void	handlePing(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 409 " + nick
				+ " :No origin specified"));
	server.sendMessage(client, ":ircserv PONG ircserv :" + params[0]);
	// TODO: pas de PING keepalive envoye par le serveur pour l'instant,
	// gere seulement le PING recu du client
}
