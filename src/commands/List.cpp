#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
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

// Normalise un nom selon le casemapping IRC, pour comparer avec la cle de
// _channels (deja normalisee par Server::getChannel).
static std::string toIrcUpper(const std::string &str)
{
	unsigned char	c;

	std::string result = str;
	for (std::string::size_type i = 0; i < result.size(); ++i)
	{
		c = static_cast<unsigned char>(result[i]);
		if (c >= 'a' && c <= 'z')
			result[i] = static_cast<char>(c - 'a' + 'A');
		else if (c == '{')
			result[i] = '[';
		else if (c == '}')
			result[i] = ']';
		else if (c == '|')
			result[i] = '\\';
	}
	return (result);
}

// —  LIST [#channel{,#channel}]
void	handleList(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	bool	matches;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
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
		matches = filter.empty();
		for (std::vector<std::string>::size_type i = 0; !matches
			&& i < filter.size(); ++i)
			matches = (toIrcUpper(filter[i]) == it->first);
		if (!matches)
			continue ;
		std::ostringstream count;
		count << it->second->getClientCount();
		reply(client, ":ircserv 322 " + nick + " " + it->second->getName() + " "
			+ count.str() + " :" + it->second->getTopic());
	}
	reply(client, ":ircserv 323 " + nick + " :End of LIST");
}
