#include <iostream>

#include "Server.hpp"
#include "Client.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //

Server::Server() : _socketFd(0) // TODO: init other var;
{
    // std::cout << "[DEBUG] Server default constructor called" << std::endl;
}

Server::Server(const Server &other) : _socketFd(other._socketFd), _address(other._address)
{
    // std::cout << "[DEBUG] Server copy constructor called" << std::endl;
}

Server &Server::operator=(const Server &other) 
{
    // std::cout << "[DEBUG] Server assignation operator called" << std::endl;

    if (this != &other)
    {
        _socketFd = other._socketFd;
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
    createSocket();
    bindSocket();
    listenSocket();
}


// ************************************************************************** //
//                             Members functions
// ************************************************************************** //

void    Server::createSocket()
{
    std::cout << "[DEBUG] creating socket...\n";

    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    // TODO: Handle error 
}

void    Server::bindSocket()
{
    std::cout << "[DEBUG] Binding IP and port to new socket\n";
    bind(_socketFd, (sockaddr *)&_address, sizeof(_address));
    // TODO: Handle error
}

void    Server::listenSocket()
{
    std::cout << "[DEBUG] Server is now listening socket\n";
    listen(_socketFd, 10);
    //TODO: Handle error
}

// ************************************************************************** //
//                             Getters
// ************************************************************************** //

// ************************************************************************** //
//                             Setters
// ************************************************************************** //

// ************************************************************************** //
//                             Exceptions
// ************************************************************************** //


// ************************************************************************** //
//                             Member function
// ************************************************************************** //


// ************************************************************************** //
//                             Operators
// ************************************************************************** //
