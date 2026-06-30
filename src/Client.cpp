#include <iostream>

#include "Client.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //
Client::Client() : _socketFd(0)
{
    std::cout << "Client default constructor called" << std::endl;
}

Client::Client(const Client &other) : _socketFd(other._socketFd)
{
    std::cout << "Client copy constructor called" << std::endl;
}

Client &Client::operator=(const Client &other) 
{
    std::cout << "Client assignation operator called" << std::endl;

    if (this != &other)
    {
        _socketFd = other._socketFd;
    }
    return (*this);
}

Client::~Client()
{
    std::cout << "Client destructor called" << std::endl;
}
// ************************************************************************** //
//                             Exceptions
// ************************************************************************** //


// ************************************************************************** //
//                             Member function
// ************************************************************************** //


// ************************************************************************** //
//                             Operators
// ************************************************************************** //
