#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// Decoupe une liste "a,b,c" en cibles individuelles.
static std::vector<std::string> splitComma(const std::string &str)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type comma;
	while ((comma = str.find(',', start)) != std::string::npos)
	{
		result.push_back(str.substr(start, comma - start));
		start = comma + 1;
	}
	result.push_back(str.substr(start));
	return (result);
}

// —  PRIVMSG <target>{,<target>} <message>
void	handlePrivmsg(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*dest;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	// 451: pas encore authentifie -> commande refusee tant que non enregistre
	if (!client->isAuthenticated())
		return (server.sendMessage(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	// 411: aucun parametre du tout -> pas de cible (nick/#channel) fournie
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 411 " + nick
				+ " :No recipient given (PRIVMSG)"));
	// 412: cible fournie mais message (params[1]) absent ou vide
	if (params.size() < 2 || params[1].empty())
		return (server.sendMessage(client, ":ircserv 412 " + nick + " :No text to send"));
	const std::vector<std::string> targets = splitComma(params[0]);
	const std::string message = params[1];
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i)
	{
		const std::string &target = targets[i];
		// cible = #channel : diffuser aux membres, sauf a l'expediteur
		if (!target.empty() && target[0] == '#')
		{
			channel = server.getChannel(target);
			// 401: le channel n'existe pas
			if (channel == NULL)
			{
				server.sendMessage(client, ":ircserv 401 " + nick + " " + target
					+ " :No such nick/channel");
				continue ;
			}
			// 404: l'expediteur n'est pas membre du channel
			if (!channel->isMember(client))
			{
				server.sendMessage(client, ":ircserv 404 " + nick + " " + target
					+ " :Cannot send to channel");
				continue ;
			}
			channel->broadcast(server, ":" + prefix + " PRIVMSG " + target
				+ " :" + message, client);
			continue ;
		}
		// cible = nickname : message prive direct
		dest = server.getClientByNick(target);
		// 401: aucun client connecte sous ce pseudo
		if (dest == NULL)
		{
			server.sendMessage(client, ":ircserv 401 " + nick + " " + target
				+ " :No such nick/channel");
			continue ;
		}
		server.sendMessage(dest, ":" + prefix + " PRIVMSG " + target + " :" + message);
	}
}
