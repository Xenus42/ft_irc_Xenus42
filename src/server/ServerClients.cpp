#include "Server.hpp"

// ServerClients.cpp
// ├── Client lifecycle
// │   ├── addClient()
// │   └── disconnectClient()
// │
// ├── Client identity
// │   ├── changeNickname()
// │   ├── getClientByNick()
// │   └── getClientByFd()
// │
// ├── Client indexes
// │   ├── addClientByFd()
// │   ├── removeClientByFd()
// │   ├── addClientByNick()
// │   └── removeClientByNick()
// │
// └── Channel cleanup
//     └── removeClientfromChannels()

// ************************************************************************** //
//                                  Client lifecycle
// ************************************************************************** //

void Server::addClient(int fd, const sockaddr_in &address)
{
	Client	*client;

	client = new Client(fd, address);
	_clientsByFd[fd] = client;
	addPollfd(fd);
	std::cout << "[SERVER] Client " << _clientsByFd[fd] << " connected" << std::endl;
}

void Server::disconnectClient(Client *client)
{
	if (!client)
		return;

	int fd = client->getFd();
	
	_clientsByFd.erase(fd);

	if (client->isAuthenticated())
	{
		std::string nickname = client->getNickname();
		_clientsByNick.erase(nickname);
	}

	removePollfd(fd);

	close(fd);

	delete client;

	printLog("SERVER", "Client disconnected and cleaned up");
}

// ************************************************************************** //
//                                  Client Identity
// ************************************************************************** //

void Server::changeNickname(Client *client, const std::string &newNick)
{
    const std::string &oldNick = client->getNickname();

    if (oldNick == newNick)
        return;

    if (!oldNick.empty())
        _clientsByNick.erase(oldNick);

    client->setNickname(newNick);

    _clientsByNick[newNick] = client;
}

Client *Server::getClientByNick(const std::string &nick)
{
	std::map<std::string, Client *>::iterator it = _clientsByNick.find(nick);

	if (it == _clientsByNick.end())
    	return NULL;

	return it->second;;
}

void Server::removeClientfromChannels(Client *client)
{
    for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel *channel = it->second;
        if (channel->isInvited(client))
            channel->removeInvited(client);
        if (channel->isMember(client))
        {
            channel->removeClient(client);
            if (channel->isEmpty())
                removeChannel(channel->getName());
        }
    }
}

// ************************************************************************** //
//                                  Client Indexes
// ************************************************************************** //

// void Server::addClientByFd(Client *client);
// void Server::removeClientByFd(Client *client);

// void Server::addClientByNick(Client *client);
// void Server::removeClientByNick(Client *client);