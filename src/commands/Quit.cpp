#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// —  QUIT [:reason]
void	handleQuit(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string reason = params.empty() ? "Client Quit" : params[0];
	const std::string prefix = client->getNickname() + "!"
		+ client->getUsername() + "@" + client->getHostname();
	const std::string msg = ":" + prefix + " QUIT :" + reason;
	const std::map<std::string, Channel *> &channels = server.getChannels();
	std::map<std::string, Channel *>::const_iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->second->isMember(client))
			it->second->broadcast(msg, client);
	}
	// TODO: appeler server.disconnectClient(client) pour fermer proprement
}
