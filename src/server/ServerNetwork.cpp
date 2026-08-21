#include "Server.hpp"

// ************************************************************************** //
//                                   Socket
// ************************************************************************** //

void Server::createSocket()
{
	_listenFd = socket(AF_INET, SOCK_STREAM, 0);

	if (_listenFd < 0)
	{
		throw std::runtime_error(std::string("socket: ")
			+ std::strerror(errno));
	}
	
	int opt = 1;
	
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		throw std::runtime_error(std::string("setsockopt: ")
			+ std::strerror(errno));
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

// void Server::processPollEvents()
// {
// 	for (size_t i = 0; i < _pollfds.size();)
// 	{
// 		short revents = _pollfds[i].revents;

// 		// Le FD est invalide.
// 		if (revents & POLLNVAL)
// 		{
// 			if (_pollfds[i].fd == _listenFd)
// 				shutdown();
// 			else
// 				disconnectClient(getClientByFd(_pollfds[i].fd));
// 			continue;
// 		}

// 		// Erreur sur le FD.
// 		if (revents & POLLERR)
// 		{
// 			if (_pollfds[i].fd == _listenFd)
// 				shutdown();
// 			else
// 				disconnectClient(getClientByFd(_pollfds[i].fd));
// 			continue;
// 		}

// 		// Socket d'écoute : nouvelle connexion.
// 		if (_pollfds[i].fd == _listenFd)
// 		{
// 			if (revents & POLLIN)
// 				acceptClient();

// 			++i;
// 			continue;
// 		}

// 		// Client : données disponibles.
// 		if (revents & POLLIN)
// 		{
// 			int fd = _pollfds[i].fd;

// 			receiveClient(fd);

// 			// Le client existe toujours ?
// 			// Si oui, on passe au suivant.
// 			if (i < _pollfds.size() && _pollfds[i].fd == fd)
// 				++i;

// 			continue;
// 		}

// 		// Connexion fermée sans données à lire.
// 		if (revents & POLLHUP)
// 		{
// 			disconnectClient(getClientByFd(_pollfds[i].fd));
// 			continue;
// 		}

// 		++i;
// 	}
// }

void Server::processPollEvents()
{
	for (size_t i = 0; i < _pollfds.size();)
	{
    	if (!(_pollfds[i].revents & POLLIN))
    	{
			++i;
    	    continue;
    	}

    	if (_pollfds[i].fd == _listenFd)
    	{
    	    acceptClient();
    	    ++i;
    	}
    	else
    	{
        	int fd = _pollfds[i].fd;
        	receiveClient(fd);
		
        	if (i < _pollfds.size() && _pollfds[i].fd == fd)
        	    ++i;
		}
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

	addClient(clientFd, clientAddr);
}

void Server::receiveClient(int fd)
{
    char buffer[1024];

    ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);

    if (bytes < 0)
    {
        std::cerr << "[ERROR] recv: " << std::strerror(errno) << std::endl;
        return;
    }

    if (bytes == 0)
    {
        disconnectClient(getClientByFd(fd));
        return;
    }

    Client *client = getClientByFd(fd);

    if (!client)
        return;

    client->appendBuffer(buffer, bytes);

    std::string line;

    while (client->extractLine(line))
    {
        Parsing::Command cmd = _parser.parse(line);
        dispatchCommand(client, cmd);
    }
}