#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// Splits a "a,b,c" list into individual targets.
static std::vector<std::string> splitComma(const std::string &str)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type comma;
	while ((comma = str.find(',', start)) != std::string::npos)
	{
		result.push_back(str.substr(start, comma - start));
		start = comma + 1;
	}
	result.push_back(str.substr(start));
	return (result);
}

// —  PRIVMSG <target>{,<target>} <message>
void	handlePrivmsg(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*dest;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	// 411: no parameter at all -> no target (nick/#channel) given
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 411 " + nick
				+ " :No recipient given (PRIVMSG)"));
	// 412: target given but message (params[1]) missing or empty
	if (params.size() < 2 || params[1].empty())
		return (server.sendMessage(client, ":ircserv 412 " + nick + " :No text to send"));
	const std::vector<std::string> targets = splitComma(params[0]);
	const std::string message = params[1];
	const std::string prefix = nick + "!" + client->getUsername() + "@"
		+ client->getHostname();
	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i)
	{
		const std::string &target = targets[i];
		// target = #channel: broadcast to members, except sender
		if (!target.empty() && target[0] == '#')
		{
			channel = server.getChannel(target);
			// 401: channel does not exist
			if (channel == NULL)
			{
				server.sendMessage(client, ":ircserv 401 " + nick + " " + target
					+ " :No such nick/channel");
				continue ;
			}
			// 404: sender is not a member of the channel
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
		// target = nickname: direct private message
		dest = server.getClientByNick(target);
		// 401: no client connected under this nickname
		if (dest == NULL)
		{
			server.sendMessage(client, ":ircserv 401 " + nick + " " + target
				+ " :No such nick/channel");
			continue ;
		}
		server.sendMessage(dest, ":" + prefix + " PRIVMSG " + target + " :" + message);
	}
}
