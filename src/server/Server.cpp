#include "Server.hpp"

// Server.cpp
// ├── OCF
// │   ├── Server()
// │   ├── Server(int, string)
// │   ├── Server(const Server&)
// │   ├── operator=()
// │   └── ~Server()
// │
// └── Life cycle
//     ├── init()
//     ├── run()
//     └── shutdown()

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //

Server::Server() : _port(0), _password(""), _listenFd(-1), _running(false),
	_address(), _pollfds(), _clientsByFd(), _clientsByNick()
{
	std::memset(&_address, 0, sizeof(_address));
}

Server::Server(int port, std::string password) : _port(port),
	_password(password), _listenFd(-1), _running(false), _address(), _pollfds(),
	_clientsByFd(), _clientsByNick()
{
	std::memset(&_address, 0, sizeof(_address));
}

Server::Server(const Server &other) : _port(other._port),
	_password(other._password), _listenFd(other._listenFd),
	_running(other._running), _address(other._address),
	_pollfds(other._pollfds), _clientsByFd(other._clientsByFd),
	_clientsByNick(other._clientsByNick)
{
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_port = other._port;
		_password = other._password;
		_listenFd = other._listenFd;
		_running = other._running;
		_address = other._address;
		_pollfds = other._pollfds;
		_clientsByFd = other._clientsByFd;
		_clientsByNick = other._clientsByNick;
	}
	return (*this);
}

Server::~Server()
{
}

// ************************************************************************** //
//                                 Life Cycle
// ************************************************************************** //

void Server::init()
{
	pollfd	serverPoll;

	createSocket();
	bindSocket();
	listenSocket();
	_running = true;
	serverPoll.fd = _listenFd;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollfds.push_back(serverPoll);
}

void Server::run()
{
	while (_running)
	{
		if (poll(&_pollfds[0], _pollfds.size(), -1) < 0)
		{
			shutdown();
			perror("poll");
		}
		// TODEL: Pour chaque port sur ecoute:
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (!(_pollfds[i].revents & POLLIN))
				continue ;
			if (_pollfds[i].fd == _listenFd)
			{
				acceptClient();
				// printServerStatus();
			}
			else
			{
				receiveClient(_pollfds[i].fd);
				printServerStatus();
			}
		}
		// TODEL: print functions
	}
}

void Server::shutdown()
{
	if (_listenFd != -1)
	{
		// close all clients;
		close(_listenFd);
		_listenFd = -1;
	}
	_running = false;
}