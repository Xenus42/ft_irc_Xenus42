#include "Server.hpp"

// ************************************************************************** //
//                            Channel lifecycle
// ************************************************************************** //

Channel *Server::createChannel(const std::string &name)
{
	Channel	*channel = new Channel(name);
	
    _channels[name] = channel;
	return (channel);
}

Channel *Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	
    if (it == _channels.end())
		return (NULL);

	return (it->second);
}

void Server::removeChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	
    if (it == _channels.end())
		return ;
	
    delete it->second;
	_channels.erase(it);
}

const std::map<std::string, Channel *> &Server::getChannels() const
{
    return _channels;
}