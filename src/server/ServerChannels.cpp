#include "Server.hpp"

// Normalise un nom de channel selon le casemapping IRC (RFC 1459) : insensible
// a la casse, avec le scrambling []\ <-> {}|.
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

// ************************************************************************** //
//                            Channel lifecycle
// ************************************************************************** //

Channel *Server::createChannel(const std::string &name)
{
	Channel	*channel;

	channel = new Channel(name);
	_channels[toIrcUpper(name)] = channel;
	return (channel);
}

Channel *Server::getChannel(const std::string &name)
{
	std::map<std::string,
		Channel *>::iterator it = _channels.find(toIrcUpper(name));
	if (it == _channels.end())
		return (NULL);
	return (it->second);
}

void Server::removeChannel(const std::string &name)
{
	std::map<std::string,
		Channel *>::iterator it = _channels.find(toIrcUpper(name));
	if (it == _channels.end())
		return ;
	delete it->second;
	_channels.erase(it);
}

const std::map<std::string, Channel *> &Server::getChannels() const
{
	return (_channels);
}