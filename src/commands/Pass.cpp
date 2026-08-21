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
	if (isValidPassword(server.getPassword()) == false)
	{
		reply(client, ":ircserv 464 :Password incorrect");
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
		reply(client, ":ircserv 464 :Password incorrect");
	}
}