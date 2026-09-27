#include "Commands.hpp"
#include "Utils.hpp"

// —  WHOIS <nickname>{,<nickname>}
void	handleWhois(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Client	*target;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 431 " + nick + " :No nickname given"));
	const std::vector<std::string> targetNicks = splitComma(params[0]);
	for (std::vector<std::string>::size_type i = 0; i < targetNicks.size(); ++i)
	{
		const std::string &targetNick = targetNicks[i];

		target = server.getClientByNick(targetNick);
		if (target == NULL)
		{
			server.sendMessage(client, ":ircserv 401 " + nick + " " + targetNick
				+ " :No such nick/channel");
			continue ;
		}
		// format 311 : <nick> <user> <host> * :<realname>
		server.sendMessage(client, ":ircserv 311 " + nick + " " + target->getNickname() + " "
			+ target->getUsername() + " " + target->getHostname() + " * :"
			+ target->getRealname());
		// format 312 : <nick> <server> :<info>
		server.sendMessage(client, ":ircserv 312 " + nick + " " + target->getNickname()
			+ " ircserv :ft_irc server");
	}
	server.sendMessage(client, ":ircserv 318 " + nick + " " + params[0]
		+ " :End of WHOIS list");
}
