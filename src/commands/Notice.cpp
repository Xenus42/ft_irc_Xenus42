#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// —  NOTICE <target> <message>
void	handleNotice(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	(void)client;
	(void)params;
	return ;
}
