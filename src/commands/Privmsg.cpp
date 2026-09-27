#include "Commands.hpp"
#include "Utils.hpp"

// —  PRIVMSG <target>{,<target>} <message>
void	handlePrivmsg(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*dest;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 411 " + nick
				+ " :No recipient given (PRIVMSG)"));
	if (params.size() < 2 || params[1].empty())
		return (server.sendMessage(client, ":ircserv 412 " + nick + " :No text to send"));
	const std::vector<std::string> targets = splitComma(params[0]);
	const std::string message = params[1];
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i)
	{
		const std::string &target = targets[i];
		if (!target.empty() && target[0] == '#')
		{
			channel = server.getChannel(target);
			if (channel == NULL)
			{
				server.sendMessage(client, ":ircserv 401 " + nick + " " + target
					+ " :No such nick/channel");
				continue ;
			}
			if (!channel->isMember(client))
			{
				server.sendMessage(client, ":ircserv 404 " + nick + " " + target
					+ " :Cannot send to channel");
				continue ;
			}
			channel->broadcast(server, ":" + prefix + " PRIVMSG " + channel->getName()
				+ " :" + message, client);
			continue ;
		}
		dest = server.getClientByNick(target);
		if (dest == NULL)
		{
			server.sendMessage(client, ":ircserv 401 " + nick + " " + target
				+ " :No such nick/channel");
			continue ;
		}
		server.sendMessage(dest, ":" + prefix + " PRIVMSG " + target + " :" + message);
	}
}
