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
	shutdown();
}

// ************************************************************************** //
//                                 Life Cycle
// ************************************************************************** //

void Server::init()
{
	_signal = false;
	try
	{
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGPIPE, SIG_IGN);
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

volatile sig_atomic_t Server::_signal = 0;

void	Server::signalHandler(int sig)
{
	(void)sig;
	_signal = 1;
}

void Server::run()
{
    while (_running && !_signal)
    {
        if (_pollfds.empty())
            break;

        int result = poll(&_pollfds[0], _pollfds.size(), 1000);

        if (result < 0)
        {
            if (errno == EINTR)
                continue;

            std::cerr << "[ERROR] poll: " << std::strerror(errno) << std::endl;
            break;
        }

        if (_signal)
            break;

        if (result > 0)
            processPollEvents();
    }

    shutdown();
}

void Server::shutdown()
{
    _running = false;

    disconnectAllClients();

    while (!_channels.empty())
    {
        const std::string name = _channels.begin()->first;
        removeChannel(name);
    }

    _pollfds.clear();

    if (_listenFd != -1)
    {
        close(_listenFd);
        _listenFd = -1;
    }
}

// ************************************************************************** //
//                                   Getters
// ************************************************************************** //

std::string Server::getPassword() const
{
	return (_password);
}