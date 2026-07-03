#include <iostream>

#include "Client.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //
Client::Client() : _socketFd(0)
{
    // std::cout << "[DEBUG] Client default constructor called" << std::endl;
}

Client::Client(const Client &other) : _socketFd(other._socketFd)
{
    // std::cout << "[DEBUG] Client copy constructor called" << std::endl;
}

Client &Client::operator=(const Client &other) 
{
    // std::cout << "[DEBUG] Client assignation operator called" << std::endl;

    if (this != &other)
    {
        _socketFd = other._socketFd;
    }
    return (*this);
}

Client::~Client()
{
    // std::cout << "[DEBUG] Client destructor called" << std::endl;
}
