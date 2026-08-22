#include "Client.hpp"
#include "Server.hpp"

// —  QUIT [:reason]
void	handleQuit(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	(void)client;
	(void)params;
	// TODO: recuperer la raison (params[0] ou "Client Quit" par defaut)
	// TODO: parcourir server.getChannels(), diffuser un message QUIT
	// aux channels dont le client est membre
	// TODO: appeler server.disconnectClient(client) pour fermer proprement
}
