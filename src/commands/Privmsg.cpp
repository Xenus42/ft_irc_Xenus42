#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// —  PRIVMSG <target> <message>
void	handlePrivmsg(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*dest;

	const std::string nick = client->getNickname();
	// 451: pas encore authentifie -> commande refusee tant que non enregistre
	if (params.empty())
		return (reply(client, ":ircserv 411 " + nick
				+ " :No recipient given (PRIVMSG)"));
	// 412: cible fournie mais message (params[1]) absent ou vide
	if (params.size() < 2 || params[1].empty())
		return (reply(client, ":ircserv 412 " + nick + " :No text to send"));
	const std::string target = params[0];
	const std::string message = params[1];
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	// cible = #channel : diffuser aux membres, sauf a l'expediteur
	if (!target.empty() && target[0] == '#')
	{
		channel = server.getChannel(target);
		// 401: le channel n'existe pas
		if (channel == NULL)
			return (reply(client, ":ircserv 401 " + nick + " " + target
					+ " :No such nick/channel"));
		// 404: l'expediteur n'est pas membre du channel
		if (!channel->isMember(client))
			return (reply(client, ":ircserv 404 " + nick + " " + target
					+ " :Cannot send to channel"));
		channel->broadcast(server, ":" + prefix + " PRIVMSG " + target + " :" + message,
			client);
		return ;
	}
	// cible = nickname : message prive direct
	dest = server.getClientByNick(target);
	// 401: aucun client connecte sous ce pseudo
	if (dest == NULL)
		return (reply(client, ":ircserv 401 " + nick + " " + target
				+ " :No such nick/channel"));
	reply(dest, ":" + prefix + " PRIVMSG " + target + " :" + message);
}
