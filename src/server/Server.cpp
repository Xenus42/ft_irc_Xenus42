#include "Server.hpp"

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
	try
	{
		createSocket();
		bindSocket();
		listenSocket();
	}
	catch (const std::exception &e)
	{
		std::cerr << "[ERROR ] " << e.what() << std::endl;
		shutdown();
		throw;
	}

	_running = true;
	addPollfd(_listenFd);
}

void Server::run()
{
	while (_running)
	{
		if (poll(&_pollfds[0], _pollfds.size(), -1) < 0)
		{
			if (errno == EINTR)
				continue;
			shutdown();
			std::cerr << "[ERROR] poll: " << std::strerror(errno) << std::endl;
		}
		processPollEvents();
	}
}

void Server::shutdown()
{
	if (_listenFd != -1)
	{
		disconnectAllClients();
		_listenFd = -1;
	}
	_running = false;
}


// ************************************************************************** //
//                                   Getters
// ************************************************************************** //

std::string Server::getPassword() const
{
	return (_password);
}