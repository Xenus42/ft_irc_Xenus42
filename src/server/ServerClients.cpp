#include "Server.hpp"

// ************************************************************************** //
//                                  Client lifecycle
// ************************************************************************** //

void Server::addClient(int fd, const sockaddr_in &address)
{
	Client	*client = new Client(fd, address);

	addClientByFd(client);
	addPollfd(fd);
	
	std::cout << "[SERVER] Client " << fd << " connected" << std::endl;
}

void Server::disconnectClient(Client *client)
{
	if (!client)
		return;

	int fd = client->getFd();

	removeClientFromChannels(client);
	removeClientByNick(client);
	removeClientByFd(client);
	removePollfd(fd);

	close(fd);
	delete client;

	printLog("SERVER", "Client disconnected and cleaned up");
}

void Server::disconnectAllClients()
{
	while (!_clientsByFd.empty())
    {
        Client *client = _clientsByFd.begin()->second;
        disconnectClient(client);
    }
}

// ************************************************************************** //
//                                  Client Channels
// ************************************************************************** //

void Server::removeClientFromChannels(Client *client)
{
    if (!client)
        return;

    std::map<std::string, Channel *>::iterator it = _channels.begin();

    while (it != _channels.end())
    {
        Channel *channel = it->second;
        ++it;

        if (channel->isInvited(client))
            channel->removeInvited(client);

        if (channel->isMember(client))
        {
            channel->removeClient(client);

            if (channel->isEmpty())
            {
                const std::string name = channel->getName();
                removeChannel(name);
            }
        }
    }
}

// ************************************************************************** //
//                                  Client Identity
// ************************************************************************** //

void Server::changeNickname(Client *client, const std::string &newNick)
{
    if (!client)
        return;

    const std::string &oldNick = client->getNickname();

    if (oldNick == newNick)
        return;

    removeClientByNick(client);

    client->setNickname(newNick);

    addClientByNick(client);
}

// ************************************************************************** //
//                                  Client Indexes
// ************************************************************************** //

void Server::addClientByFd(Client *client)
{
	if (!client)
		return;

	_clientsByFd[client->getFd()] = client;
}

Client *Server::getClientByFd(int fd)
{
	std::map<int, Client *>::iterator it = _clientsByFd.find(fd);

	if (it == _clientsByFd.end())
		return NULL;

	return it->second;
}

void Server::removeClientByFd(Client *client)
{
	if (!client)
		return;

	_clientsByFd.erase(client->getFd());
}

void Server::addClientByNick(Client *client)
{
	if (!client)
		return;

	const std::string &nick = client->getNickname();
	
	if (!nick.empty())
		_clientsByNick[nick] = client;
}

Client *Server::getClientByNick(const std::string &nick)
{
	std::map<std::string, Client *>::iterator it = _clientsByNick.find(nick);

	if (it == _clientsByNick.end())
		return NULL;

	return it->second;;
}

void Server::removeClientByNick(Client *client)
{
	if (!client)
		return;

	const std::string &nick = client->getNickname();
	
	if (!nick.empty())
		_clientsByNick.erase(nick);
}