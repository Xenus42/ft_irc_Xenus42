#include "Client.hpp"
#include "Server.hpp"

// Decoupe une liste "a,b,c" en pseudos individuels.
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

// —  WHOIS <nickname>{,<nickname>}
void	handleWhois(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Client	*target;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	if (params.empty())
		return (reply(client, ":ircserv 431 " + nick + " :No nickname given"));
	const std::vector<std::string> targetNicks = splitComma(params[0]);
	for (std::vector<std::string>::size_type i = 0; i < targetNicks.size(); ++i)
	{
		const std::string &targetNick = targetNicks[i];

		target = server.getClientByNick(targetNick);
		if (target == NULL)
		{
			reply(client, ":ircserv 401 " + nick + " " + targetNick
				+ " :No such nick/channel");
			continue ;
		}
		// format 311 : <nick> <user> <host> * :<realname>
		reply(client, ":ircserv 311 " + nick + " " + targetNick + " "
			+ target->getUsername() + " " + target->getHostname() + " * :"
			+ target->getUsername());
		// format 312 : <nick> <server> :<info>
		reply(client, ":ircserv 312 " + nick + " " + targetNick
			+ " ircserv :ft_irc server");
	}
	// une seule ligne 318 de fin, apres tous les pseudos demandes
	reply(client, ":ircserv 318 " + nick + " " + params[0]
		+ " :End of WHOIS list");
}
