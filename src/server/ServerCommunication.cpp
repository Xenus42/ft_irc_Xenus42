#include "Server.hpp"

// ************************************************************************** //
//                                   Communication
// ************************************************************************** //

void Server::sendMessage(Client *client, const std::string &message)
{
	if (!client)
		return ;

	client->appendOutBuffer(message);

	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == client->getFd())
		{
			_pollfds[i].events |= POLLOUT;
			break ;
		}
	}
}

void Server::sendClient(int fd)
{
	Client *client = getClientByFd(fd);

	if (!client)
		return ;

	std::string &buffer = client->getOutBuffer();

	if (buffer.empty())
		return ;

	ssize_t bytes = send(fd, buffer.c_str(), buffer.size(), 0);

	if (bytes > 0)
	{
		buffer.erase(0, bytes);

		if (buffer.empty())
		{
			for (size_t i = 0; i < _pollfds.size(); ++i)
			{
				if (_pollfds[i].fd == fd)
				{
					_pollfds[i].events &= ~POLLOUT;
					break ;
				}
			}
		}
	}
	else if (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		disconnectClient(client);
}

void Server::sendWelcome(Client *client)
{
    const std::string nick = client->getNickname();

    sendMessage(client,
        ":ircserv 001 " + nick +
        " :Welcome to the IRC Network\r\n");

    sendMessage(client,
        ":ircserv 002 " + nick +
        " :Your host is ircserv\r\n");

    sendMessage(client,
        ":ircserv 003 " + nick +
        " :This server was created today\r\n");

	// TODO: check message excepted
    sendMessage(client,
        ":ircserv 004 " + nick +
        " ircserv 1.0 io\r\n");
}