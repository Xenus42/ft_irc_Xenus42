#include "Server.hpp"

static bool	isValidPassword(const std::string &password)
{
	if (password.empty() || password.size() > 255)
		return (false);
	if (password.find(' ') != std::string::npos)
		return (false);
	if (password.find('\r') != std::string::npos)
		return (false);
	if (password.find('\n') != std::string::npos)
		return (false);
	return (true);
}

void	handlePass(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();

	if (params.empty())
	{
		server.sendMessage(client, ":ircserv 461 " + nick + " PASS :Not enough parameters");
		return ;
	}
	if (client->isRegistered())
	{
		server.sendMessage(client, ":ircserv 462 " + nick + " :You may not reregister");
		return ;
	}
	if (isValidPassword(server.getPassword()) == false)
	{
		server.sendMessage(client, ":ircserv 464 " + nick + " :Password incorrect");
		return ;
	}

	const std::string &providedPassword = params[0];

	if (providedPassword == server.getPassword())
	{
		client->setAuthenticated(true);
		if (client->isRegistered())
			server.sendWelcome(client);
	}
	else
	{
		server.sendMessage(client, ":ircserv 464 " + nick + " :Password incorrect");
		client->setAuthenticated(false);
	}
}
