#include "Commands.hpp"
#include "Utils.hpp"

// —  PART <#channel>{,<#channel>} [:reason]
void	handlePart(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;

	const std::string nick = client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 461 " + nick
				+ " PART :Not enough parameters"));
	const std::string reason = (params.size() > 1) ? params[1] : "";
	const std::vector<std::string> names = splitComma(params[0]);
	for (std::vector<std::string>::size_type i = 0; i < names.size(); ++i)
	{
		const std::string &name = names[i];
		channel = server.getChannel(name);
		if (channel == NULL)
		{
			server.sendMessage(client, ":ircserv 403 " + nick + " " + name
				+ " :No such channel");
			continue ;
		}
		if (!channel->isMember(client))
		{
			server.sendMessage(client, ":ircserv 442 " + nick + " " + name
				+ " :You're not on that channel");
			continue ;
		}
		const std::string prefix = nick + "!" + client->getUsername() + "@"
			+ client->getHostname();
		std::string msg = ":" + prefix + " PART " + channel->getName();
		if (!reason.empty())
			msg += " :" + reason;
		channel->broadcast(server, msg);
		channel->removeClient(client);
		if (channel->isEmpty())
			server.removeChannel(name);
	}
}
