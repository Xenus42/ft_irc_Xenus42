#include "Client.hpp"
#include "Server.hpp"

void	handleWhois(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Client	*target;

	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.empty())
		return (reply(client, ":ircserv 431 " + nick + " :No nickname given"));
	const std::string targetNick = params[0];
	target = server.getClientByNick(targetNick);
	if (target == NULL)
		return (reply(client, ":ircserv 401 " + nick + " " + targetNick
				+ " :No such nick/channel"));
	// TODO: envoyer 311 (whoisuser), 312 (whoisserver), 318 (end of whois)
}
