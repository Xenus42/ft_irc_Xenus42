#include "Client.hpp"
#include "Server.hpp"

// —  WHOIS <nickname>
void	handleWhois(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Client	*target;

	const std::string nick = client->getNickname();
	if (params.empty())
		return (reply(client, ":ircserv 431 " + nick + " :No nickname given"));
	const std::string targetNick = params[0];
	target = server.getClientByNick(targetNick);
	if (target == NULL)
		return (reply(client, ":ircserv 401 " + nick + " " + targetNick
				+ " :No such nick/channel"));
	// format 311 : <nick> <user> <host> * :<realname>
	reply(client, ":ircserv 311 " + nick + " " + targetNick + " "
		+ target->getUsername() + " " + target->getHostname() + " * :"
		+ target->getUsername());
	// format 312 : <nick> <server> :<info>
	reply(client, ":ircserv 312 " + nick + " " + targetNick
		+ " ircserv :ft_irc server");
	// une seule ligne 318 de fin, apres les deux reponses precedentes
	reply(client, ":ircserv 318 " + nick + " " + targetNick
		+ " :End of WHOIS list");
}
