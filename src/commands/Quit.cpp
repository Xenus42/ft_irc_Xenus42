#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// —  QUIT [:reason]
// The client is only marked as closing: sendClient() disconnects it once
// ERROR has been flushed, so the message actually reaches it.
void	handleQuit(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	const std::string reason = params.empty() ? "Client Quit" : params[0];

	server.broadcastQuit(client, "Quit: " + reason);
	server.removeClientFromChannels(client);
	server.sendMessage(client, "ERROR :Closing Link: " + nick + " ("
		+ reason + ")");
	client->markClosing();
}
