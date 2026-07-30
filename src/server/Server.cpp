#include <iostream>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <stdio.h>

#include "Server.hpp"
#include "Client.hpp"
#include "Tmp_Log.hpp"
#include "Parsing.hpp"

// TODEL:
#include "Commands.hpp"
#include "Channel.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //

Server::Server()
    : _port(0),
      _password(""),
      _listenFd(-1),
      _running(false),
      _address(),
      _pollfds(),
      _clientsByFd(),
      _clientsByNick()
{
    std::memset(&_address, 0, sizeof(_address));
}

Server::Server(int port, std::string password)
    : _port(port),
      _password(password),
      _listenFd(-1),
      _running(false),
      _address(),
      _pollfds(),
      _clientsByFd(),
      _clientsByNick()
{
    std::memset(&_address, 0, sizeof(_address));
}

Server::Server(const Server &other)
    : _port(other._port),
      _password(other._password),
      _listenFd(other._listenFd),
      _running(other._running),
      _address(other._address),
      _pollfds(other._pollfds),
      _clientsByFd(other._clientsByFd),
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
    createSocket();
    bindSocket();
    listenSocket();

    _running = true;

    pollfd serverPoll;


    serverPoll.fd = _listenFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    _pollfds.push_back(serverPoll);
}

void Server::run()
{
    printServerStatus();
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
                continue;

            if (_pollfds[i].fd == _listenFd)
            {
                acceptClient();
                printServerStatus();
            }
            else
                receiveClient(_pollfds[i].fd);
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

// ************************************************************************** //
//                                   Socket
// ************************************************************************** //

void Server::createSocket()
{
    
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);

    if (_listenFd < 0)
    {
        shutdown();
        throw std::runtime_error(std::string("socket: ")
            + std::strerror(errno));
    }

    int opt = 1;

    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)) < 0)
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

    if (bind(_listenFd,
        (sockaddr *)&_address,
        sizeof(_address)))
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
        throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
    }

    std::cout << "[INFO ] Listening on port "
              << _port << std::endl;
}

// ************************************************************************** //
//                                   Client
// ************************************************************************** //

void Server::acceptClient()
{
    sockaddr_in clientAddr;
    socklen_t   len = sizeof(clientAddr);

    int clientFd = accept(_listenFd,(sockaddr *)&clientAddr, &len);

    if (clientFd < 0)
    {
        perror("accept");
        return;
    }

    addClient(clientFd, clientAddr);
}

void Server::addClient(int fd, const sockaddr_in& address)
{
    Client *client = new Client(fd, address);

    _clientsByFd[fd] = client;

    addPollfd(fd);

    std::cout << "[SERVER] Client " << _clientsByFd[fd] << " connected" << std::endl;
}

void    Server::receiveClient(int fd)
{
    char    buffer[1024];

    int bytes = recv(fd, buffer, sizeof(buffer) -1, 0);
    
    if (bytes < 0)
    {
        perror("recv");
        return;   
    }
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        _clientsByFd[fd]->appendBuffer(buffer);
        
        Parsing parser;

        handleJoin(*this, _clientsByFd[fd], parser.parse(_clientsByFd[fd]->getBuffer()).params);
        printChannels();
        std::cout << "recv: " << buffer << std::endl;
        std::cout << "_buffer: " << _clientsByFd[fd]->getBuffer() << std::endl;
    }
    if (bytes == 0)
    {
        // TODO: handle disconnect;
        close(fd);
        printLog("SERVER", "Client disconnected");
    }
}

// ************************************************************************** //
//                                   Poll
// ************************************************************************** //

void Server::addPollfd(int fd)
{
    pollfd pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    _pollfds.push_back(pfd);
}

// ************************************************************************** //
//                                   Print
// ************************************************************************** //

void Server::printPollfds() const
{
    printSection("Pollfds:");
    for (size_t i = 0; i < _pollfds.size(); i++)
    {
        std::cout << "[" << i << "]"
                  << "\t fd: " << _pollfds[i].fd
                  << "\t event: ";

        if (_pollfds[i].events & POLLIN)
            std::cout << "POLLIN";

        if (_pollfds[i].revents & POLLOUT)
            std::cout << "POLLOUT";

        if (_pollfds[i].revents & POLLERR)
            std::cout << "POLLERR";

        if (_pollfds[i].revents & POLLHUP)
            std::cout << "POLLHUP";

        std::cout << std::endl;

    }
    nl(2);
}

void Server::printClientsByFd() const
{
    printSection("Clients:");

    for (std::map<int, Client*>::const_iterator it = _clientsByFd.begin();
         it != _clientsByFd.end();
         ++it)
    {
        std::cout << "fd: " << it->first;

        if (it->second)
        {
            std::cout << "\t nick: " << it->second->getNickname()
                      << "\t user: " << it->second->getUsername();
        }
        else
        {
            std::cout << "\tClient: NULL";
        }

        std::cout << std::endl;
    }
    nl(2);
}

void Server::printChannels() const
{
    printSection("Channels:");

    for (std::map<std::string, Channel*>::const_iterator it = _channels.begin();
         it != _channels.end();
         ++it)
    {
        std::cout << "name: " << it->first;

        if (it->second)
        {
            std::cout << "\t topic: " << it->second->getTopic()
                      << "\t members: " << it->second->getClientCount();
        }
        else
        {
            std::cout << "\tChannel: NULL";
        }

        std::cout << std::endl;
    }
    nl(2);
}

void    Server::printServerStatus() const
{
        nl(1);
        printTitle("SERVER STATUS");
        printPollfds();
        printClientsByFd();
}

// ************************************************************************** //
//                                  Channel
// ************************************************************************** //

Channel *Server::createChannel(const std::string &name)
{
    Channel *channel = new Channel(name);
    _channels[name] = channel;
    return channel;
}

Channel *Server::getChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it == _channels.end())
        return NULL;
    return it->second;
}

void Server::removeChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it == _channels.end())
        return;
    delete it->second;
    _channels.erase(it);
}


// ************************************************************************** //
//                                  Command Dispatch
// ************************************************************************** //
// void Server::dispatchCommand(Client *client,
                            //  const Parsing::Command &cmd)
// {
    // if (cmd.name == "PASS")
        // handlePass(client, cmd);
// 
    // else if (cmd.name == "NICK")
        // handleNick(client, cmd);

//     else if (cmd.name == "USER")
//         handleUser(client, cmd);

//     else if (cmd.name == "JOIN")
//         handleJoin(client, cmd);

//     ...
// }