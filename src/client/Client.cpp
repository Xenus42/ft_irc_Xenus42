#include <iostream>

#include "Client.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //

Client::Client()
    : _fd(-1),
      _address(),
      _nickname(""),
      _username(""),
      _hostname(""),
      _buffer(""),
      _authenticated(false)
{
}

Client::Client(int fd, const sockaddr_in& address)
    : _fd(fd),
      _address(address),
      _nickname(""),
      _username(""),
      _hostname(""),
      _buffer(""),
      _authenticated(false)
{
}

Client::Client(const Client &other)
    : _fd(other._fd),
      _address(other._address),
      _nickname(other._nickname),
      _username(other._username),
      _hostname(other._hostname),
      _buffer(other._buffer),
      _authenticated(other._authenticated)
{
}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _fd = other._fd;
        _address = other._address;
        _nickname = other._nickname;
        _username = other._username;
        _hostname = other._hostname;
        _buffer = other._buffer;
        _authenticated = other._authenticated;
    }
    return (*this);
}

Client::~Client()
{
}

// ************************************************************************** //
//                              Getters
// ************************************************************************** //

int Client::getFd() const
{
    return (_fd);
}

const std::string &Client::getNickname() const
{
    return (_nickname);
}

const std::string &Client::getUsername() const
{
    return (_username);
}

const std::string &Client::getHostname() const
{
    return (_hostname);
}

const std::string &Client::getBuffer() const
{
    return (_buffer);
}

bool Client::isAuthenticated() const
{
    return (_authenticated);
}

// ************************************************************************** //
//                              Setters
// ************************************************************************** //

void Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
    _username = username;
}

void Client::setHostname(const std::string &hostname)
{
    _hostname = hostname;
}

void Client::setAuthenticated(bool authenticated)
{
    _authenticated = authenticated;
}

void Client::setServerName(const std::string &serverName)
{
    _servername = serverName;
}

void Client::setRealName(const std::string &realName)
{
    _realname = realName;
}
void Client::setPassword(const std::string &password)
{
    _password = password;
}


// ************************************************************************** //
//                              Buffer
// ************************************************************************** //

void Client::appendBuffer(const char *data, size_t len)
{
    _buffer.append(data, len);
}

void Client::clearBuffer()
{
    _buffer.clear();
}

bool Client::extractLine(std::string &line)
{
    size_t pos = _buffer.find("\r\n");

    if (pos == std::string::npos)
        return false;

    line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2);
    return true;
}