#include "Server.hpp"
#include <fcntl.h>

// ************************************************************************** //
//                                   Socket
// ************************************************************************** //

void Server::createSocket()
{
	int opt;

	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0)
	{
		throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
	}

	if (fcntl(_listenFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error(std::string("fcntl: ") + std::strerror(errno));

	opt = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		throw std::runtime_error(std::string("setsockopt: ") + std::strerror(errno));
	}
}

void Server::bindSocket()
{
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
	if (bind(_listenFd, (sockaddr *)&_address, sizeof(_address)) < 0)
	{
		throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
	}
}

void Server::listenSocket()
{
	if (listen(_listenFd, SOMAXCONN) < 0)
	{
		throw std::runtime_error(std::string("listen: ")
			+ std::strerror(errno));
	}
	std::cout << "[INFO  ] Listening on port " << _port << std::endl;
}

// ************************************************************************** //
//                                   Poll
// ************************************************************************** //

void Server::addPollfd(int fd)
{
	pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
}

void Server::processPollEvents()
{
	short	revents;
	int		fd;

	for (size_t i = 0; i < _pollfds.size();)
	{
		revents = _pollfds[i].revents;
		fd = _pollfds[i].fd;

		// Fd not open
		if (revents & POLLNVAL)
		{
			if (fd == _listenFd)
			{
				_running = false;
				return ;
			}
			disconnectClient(getClientByFd(fd));
			continue ;
		}

		// Error on fd
		if (revents & POLLERR)
		{
			if (fd == _listenFd)
			{
				_running = false;
				return ;
			}
			disconnectClient(getClientByFd(fd));
			continue ;
		}

		// New client connection
		if (fd == _listenFd)
		{
			if (revents & POLLIN)
				acceptClient();

			++i;
			continue ;
		}

		// Read client data
		if (revents & POLLIN)
		{
			receiveClient(fd);

			// if client is disconnect, continue to the next one
			if (i >= _pollfds.size() || _pollfds[i].fd != fd)
				continue ;
		}

		// Send pending data
		if (revents & POLLOUT)
		{
			sendClient(fd);

			// sendClient may disconnect the client
			if (i >= _pollfds.size() || _pollfds[i].fd != fd)
				continue ;
		}

		// Connection closed
		if (revents & POLLHUP)
		{
			disconnectClient(getClientByFd(fd));
			continue ;
		}

		++i;
	}
}

void Server::removePollfd(int fd)
{
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			return ;
		}
	}
}

// ************************************************************************** //
//                                   Client I/O
// ************************************************************************** //

void Server::acceptClient()
{
	sockaddr_in	clientAddr;
	socklen_t	len;
	int			clientFd;

	len = sizeof(clientAddr);
	clientFd = accept(_listenFd, (sockaddr *)&clientAddr, &len);
	
	if (clientFd < 0)
	{
		std::cerr << "[ERROR] accept: " << std::strerror(errno) << std::endl;
		return ;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "[ERROR] fcntl: " << std::strerror(errno) << std::endl;
		close(clientFd);
		return ;
	}

	addClient(clientFd, clientAddr);
}

void Server::receiveClient(int fd)
{
	char buffer[1024];

	ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);

	if (bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
	
		std::cerr << "[ERROR] recv: "
			<< std::strerror(errno) << std::endl;
	
		disconnectClient(getClientByFd(fd));
		return ;
	}

	if (bytes == 0)
	{
		disconnectClient(getClientByFd(fd));
		return ;
	}

	Client *client = getClientByFd(fd);

	if (!client)
		return ;

	client->appendInBuffer(buffer, bytes);

	std::string line;

	while (client->extractLine(line))
	{
		try
		{
			Parsing::Command cmd = _parser.parse(line);
			dispatchCommand(client, cmd);
			if (getClientByFd(fd) == NULL)
				return;
		}
		catch (const std::exception &e)
		{
			const std::string target = client->getNickname().empty() ? "*" : client->getNickname();
			sendMessage(client, ":ircserv 421 " + target + " :" + e.what()
				+ "\r\n");
		}
	}
}