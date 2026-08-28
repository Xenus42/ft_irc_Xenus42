#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <sstream>

// —  LIST [#channel]
void	handleList(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	reply(client, ":ircserv 321 " + nick + " Channel :Users  Name");
	const std::map<std::string, Channel *> &channels = server.getChannels();
	std::map<std::string, Channel *>::const_iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		// filtre optionnel : LIST #chan ne montre que ce channel
		if (!params.empty() && params[0] != it->first)
			continue ;
		std::ostringstream count;
		count << it->second->getClientCount();
		reply(client, ":ircserv 322 " + nick + " " + it->first + " "
			+ count.str() + " :" + it->second->getTopic());
	}
	reply(client, ":ircserv 323 " + nick + " :End of LIST");
}
