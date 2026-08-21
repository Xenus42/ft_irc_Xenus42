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
	reply(client, ":ircserv 311 " + nick + " " + targetNick + " "
		+ target->getUsername() + " " + target->getHostname() + " * :"
		+ target->getUsername());
	reply(client, ":ircserv 312 " + nick + " " + targetNick
		+ " ircserv :ft_irc server");
	reply(client, ":ircserv 318 " + nick + " " + targetNick
		+ " :End of WHOIS list");
}
