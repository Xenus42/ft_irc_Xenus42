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

// —  NOTICE <target>{,<target>} <message>
void	handleNotice(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*dest;

	const std::string nick = client->getNickname();
	if (params.empty())
		return ;
	if (params.size() < 2 || params[1].empty())
		return ;
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
				continue ;
			if (!channel->isMember(client))
				continue ;
			channel->broadcast(server, ":" + prefix + " NOTICE " + channel->getName() + " :"
				+ message, client);
			continue ;
		}
		dest = server.getClientByNick(target);
		if (dest == NULL)
			continue ;
		server.sendMessage(dest, ":" + prefix + " NOTICE " + target + " :" + message);
	}
}
