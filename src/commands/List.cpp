#include "Commands.hpp"
#include "Utils.hpp"
#include <sstream>

// —  LIST [#channel{,#channel}]
void	handleList(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	bool	matches;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	server.sendMessage(client, ":ircserv 321 " + nick + " Channel :Users  Name");
	const std::vector<std::string> filter = params.empty() ? std::vector<std::string>() : splitComma(params[0]);
	const std::map<std::string, Channel *> &channels = server.getChannels();
	std::map<std::string, Channel *>::const_iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		matches = filter.empty();
		for (std::vector<std::string>::size_type i = 0; !matches
			&& i < filter.size(); ++i)
			matches = (toIrcUpper(filter[i]) == it->first);
		if (!matches)
			continue ;
		std::ostringstream count;
		count << it->second->getClientCount();
		server.sendMessage(client, ":ircserv 322 " + nick + " " + it->second->getName() + " "
			+ count.str() + " :" + it->second->getTopic());
	}
	server.sendMessage(client, ":ircserv 323 " + nick + " :End of LIST");
}
