#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// —  NOTICE <target> <message>
void	handleNotice(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*dest;

	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return ;
	if (params.empty())
		return ;
	if (params.size() < 2 || params[1].empty())
		return ;
	const std::string target = params[0];
	const std::string message = params[1];
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	if (!target.empty() && target[0] == '#')
	{
		channel = server.getChannel(target);
		if (channel == NULL)
			return ;
		if (!channel->isMember(client))
			return ;
		channel->broadcast(server, ":" + prefix + " NOTICE " + target + " :" + message,
			client);
		return ;
	}
	dest = server.getClientByNick(target);
	if (dest == NULL)
		return ;
	reply(dest, ":" + prefix + " NOTICE " + target + " :" + message);
}
