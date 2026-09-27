#include "Parsing.hpp"
#include "Client.hpp"
#include <arpa/inet.h>

// === Orthodox Canonical Form ===

Client::Client() : _fd(-1), _authenticated(false), _closing(false)
{
}

Client::Client(int fd, const sockaddr_in &address) : _fd(fd),
	_hostname(inet_ntoa(address.sin_addr)), _authenticated(false), _closing(false)
{
}

Client::Client(const Client &other) : _fd(other._fd), _nickname(other._nickname),
	_username(other._username), _hostname(other._hostname),
	_realname(other._realname), _inBuffer(other._inBuffer),
	_outBuffer(other._outBuffer), _authenticated(other._authenticated),
	_closing(other._closing)
{
}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_nickname = other._nickname;
		_username = other._username;
		_hostname = other._hostname;
		_realname = other._realname;
		_inBuffer = other._inBuffer;
		_outBuffer = other._outBuffer;
		_authenticated = other._authenticated;
		_closing = other._closing;
	}
	return (*this);
}

Client::~Client()
{
}

// === Getters ===

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

const std::string &Client::getRealname() const
{
	return (_realname);
}

std::string &Client::getOutBuffer()
{
	return (_outBuffer);
}

bool Client::isAuthenticated() const
{
	return (_authenticated);
}

bool Client::isClosing() const
{
	return (_closing);
}

bool Client::isRegistered() const
{
	return (_authenticated && !_nickname.empty() && !_username.empty());
}

// === Setters ===

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	_username = username;
}

void Client::setRealname(const std::string &realname)
{
	_realname = realname;
}

void Client::setAuthenticated(bool authenticated)
{
	_authenticated = authenticated;
}

void Client::markClosing()
{
	_closing = true;
}


// === Buffer ===


void Client::appendInBuffer(const char *data, size_t len)
{
	_inBuffer.append(data, len);
	if (_inBuffer.size() > Parsing::MAX_LINE_LENGTH
		&& _inBuffer.find('\n') == std::string::npos)
		_inBuffer.erase(Parsing::MAX_LINE_LENGTH);
}

void Client::appendOutBuffer(const std::string &message)
{
	_outBuffer += message;
}

bool Client::extractLine(std::string &line)
{
	size_t pos = _inBuffer.find('\n');

	if (pos == std::string::npos)
		return (false);

	size_t end = pos;
	if (end > 0 && _inBuffer[end - 1] == '\r') // \r is optional (nc case)
		--end;
	line = _inBuffer.substr(0, end);
	_inBuffer.erase(0, pos + 1);
	return (true);
}
