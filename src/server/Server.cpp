#include <iostream>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <stdio.h>

#include "Server.hpp"
#include "Client.hpp"
#include "Tmp_Log.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //

Server::Server() : _port(0), _password(""), _socketFd(-1), _run(false), _pollfds()
{
    // std::cout << "[DEBUG] Server default constructor called" << std::endl;

    std::memset(&_address, 0, sizeof(_address));
}

Server::Server(int port, std::string password) : _port(port), _password(password), _socketFd(0), _run(false), _pollfds() 
{
    // std::cout << "[DEBUG] Server constructor with parameters called" << std::endl;
    
    std::memset(&_address, 0, sizeof(_address));
}

Server::Server(const Server &other) : _port(other._port), _password(other._password), _socketFd(other._socketFd), _address(other._address)
{
    // std::cout << "[DEBUG] Server copy constructor called" << std::endl;
    
    std::memset(&_address, 0, sizeof(_address));
}

Server &Server::operator=(const Server &other) 
{
    // std::cout << "[DEBUG] Server assignation operator called" << std::endl;

    if (this != &other)
    {
        _port = other._port;
        _password = other._port;
        _socketFd = other._socketFd;
        _run = other._run;
        _address = other._address;
    }
    return (*this);
}

Server::~Server()
{
    // std::cout << "[DEBUG] Server destructor called\n";
}

// ************************************************************************** //
//                             Life Cycle
// ************************************************************************** //

void    Server::init()
{
    // std::cout << "[DEBUG] Initializing server...\n";

    createSocket();
    bindSocket();
    listenSocket();
    _run = true;

    pollfd serverPoll;
    serverPoll.fd = _socketFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0; 

    _pollfds.push_back(serverPoll);
}

void    Server::run()
{
    // std::cout << "[DEBUG] Server is running...\n";

    while (_run)
    {
        if (poll(&_pollfds[0], _pollfds.size(), -1) < 0)
        {
            shutdown();
            perror("poll");
        }
        for (size_t i = 0; i < _pollfds.size(); i++)
        {
            if (_pollfds[i].revents & POLLIN)
            {
                // same socketFd == > ready to connect
                if (_pollfds[i].fd == _socketFd)
                    acceptClient();
                // else receiveMessage()
            }
        }
    }
}

void    Server::shutdown()
{
    // std::cout << "[DEBUG] Server is shutting down...\n";

    if (_socketFd != -1)
    {
        close(_socketFd);
        _socketFd = -1;
    }
    _run = false;
}


// ************************************************************************** //
//                             Socket
// ************************************************************************** //

void    Server::createSocket()
{
    // std::cout << "[DEBUG] Creating socket...\n";

    _socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (_socketFd < 0)
    {
        shutdown();
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
    }

    int opt = 1;

    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        shutdown();
        throw std::runtime_error(std::string("setsockopt: ") + std::strerror(errno));
    }
}

void    Server::bindSocket()
{
    // std::cout << "[DEBUG] Binding IP and port to new socket\n";
    
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(_port);

    if (bind(_socketFd, (sockaddr *)&_address, sizeof(_address)))
    {
        shutdown();
        throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
    }
}

void    Server::listenSocket()
{
    // std::cout << "[DEBUG] Server is now listening socket\n";

    if (listen(_socketFd, SOMAXCONN))
    {
        shutdown();
        throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
    }
    std::cout << "[INFO ] Listening on port " << _port << std::endl;
}


// ************************************************************************** //
//                             Client
// ************************************************************************** //

void    Server::acceptClient(void)
{
    // std::cout << "[DEBUG] Server is accepting client...\n";

    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    int clientFd = accept(_socketFd, (sockaddr *)&clientAddr, &len);
    if (clientFd < 0)
    {
        shutdown();
        perror("accept");
    }

    std::cout << "[SERVER] Client connected!\n";
}
