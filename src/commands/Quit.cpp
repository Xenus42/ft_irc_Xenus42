#include "Client.hpp"
#include "Server.hpp"

// —  QUIT [:reason]
void	handleQuit(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	(void)client;
	const std::string reason = params.empty() ? "Client Quit" : params[0];
	(void)reason;
	// TODO: parcourir server.getChannels(), diffuser un message QUIT
	// aux channels dont le client est membre
	// TODO: appeler server.disconnectClient(client) pour fermer proprement
}
