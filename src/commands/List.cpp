#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <algorithm>
#include <sstream>

// Decoupe une liste "#a,#b,#c" en noms individuels.
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

// —  LIST [#channel{,#channel}]
void	handleList(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string nick = client->getNickname();
	if (!client->isAuthenticated())
		return (reply(client, ":ircserv 451 " + nick
				+ " :You have not registered"));
	reply(client, ":ircserv 321 " + nick + " Channel :Users  Name");
	const std::vector<std::string> filter = params.empty() ? std::vector<std::string>() : splitComma(params[0]);
	const std::map<std::string, Channel *> &channels = server.getChannels();
	std::map<std::string, Channel *>::const_iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		// filtre optionnel : LIST #a,#b ne montre que ces channels
		if (!filter.empty() && std::find(filter.begin(), filter.end(),
				it->first) == filter.end())
			continue ;
		std::ostringstream count;
		count << it->second->getClientCount();
		reply(client, ":ircserv 322 " + nick + " " + it->first + " "
			+ count.str() + " :" + it->second->getTopic());
	}
	reply(client, ":ircserv 323 " + nick + " :End of LIST");
}
