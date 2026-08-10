#include "Client.hpp"
#include "Server.hpp"

// —  PRIVMSG <target> <message>
// TODO: valider les params, gerer les channels et les messages prives.
void	handlePrivmsg(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	(void)client;
	(void)params;
}
