#include "Server.hpp"

// ServerNetwork.cpp
// ├── Socket
// │   ├── createSocket()
// │   ├── bindSocket()
// │   └── listenSocket()
// │
// ├── Poll
// │   ├── addPollfd()
// │   └── removePollfd()
// │
// └── Client I/O
//     ├── acceptClient()
//     └── receiveClient()

// ************************************************************************** //
//                                   Socket
// ************************************************************************** //

void Server::createSocket()
{
	int	opt;

	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0)
	{
		shutdown();
		throw std::runtime_error(std::string("socket: ")
			+ std::strerror(errno));
	}
	opt = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		shutdown();
		throw std::runtime_error(std::string("setsockopt: ")
			+ std::strerror(errno));
	}
}

void Server::bindSocket()
{
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
	if (bind(_listenFd, (sockaddr *)&_address, sizeof(_address)))
	{
		shutdown();
		throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
	}
}

void Server::listenSocket()
{
	if (listen(_listenFd, SOMAXCONN))
	{
		shutdown();
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
		perror("accept");
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
		perror("recv");
		return ;
	}

	if (bytes == 0)
	{
		disconnectClient(_clientsByFd[fd]);
		return ;
	}
	
	Client *client = _clientsByFd[fd];
	
	client->appendBuffer(buffer, bytes);
	
	std::string line;
	
	while (client->extractLine(line))
	{
	    Parsing::Command cmd = _parser.parse(line);
	
	    dispatchCommand(client, cmd);
	}
}

// void Server::processPollEvents()