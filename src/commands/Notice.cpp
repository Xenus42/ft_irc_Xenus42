#include "Commands.hpp"
#include "Utils.hpp"

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
