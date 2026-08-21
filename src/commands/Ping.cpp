#include "Client.hpp"
#include "Server.hpp"

// —  PING <token>
void	handlePing(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	(void)client;
	(void)params;
	// TODO: verifier params non vide, sinon 409 :No origin specified
	// TODO: repondre ":ircserv PONG ircserv :<token>" avec params[0] 
    std::cout << "test" << std::endl;
}
