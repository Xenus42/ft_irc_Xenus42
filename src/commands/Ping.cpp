#include "Client.hpp"
#include "Server.hpp"

// —  PING <token>
void	handlePing(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	(void)server;
	const std::string nick = client->getNickname();
	if (params.empty())
		return (reply(client, ":ircserv 409 " + nick
				+ " :No origin specified"));
	reply(client, ":ircserv PONG ircserv :" + params[0]);
	// TODO: no PING keepalive sent by the server for now,
	// only handles PING received from the client
}
