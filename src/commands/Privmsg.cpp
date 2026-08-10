#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// —  PRIVMSG <target> <message>
// TODO: gerer les channels et les messages prives.
void	handlePrivmsg(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string nick = client->getNickname();
	// 451: pas encore authentifie -> commande refusee tant que non enregistre
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	// 411: aucun parametre du tout -> pas de cible (nick/#channel) fournie
	if (params.empty())
		return (reply(client, ":ircserv 411 " + nick
				+ " :No recipient given (PRIVMSG)"));
	// 412: cible fournie mais message (params[1]) absent ou vide
	if (params.size() < 2 || params[1].empty())
		return (reply(client, ":ircserv 412 " + nick + " :No text to send"));
}
