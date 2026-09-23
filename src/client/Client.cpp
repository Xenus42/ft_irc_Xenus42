#include "Client.hpp"

// ************************************************************************** //
//                             Orthodox Canonical Form
// ************************************************************************** //

Client::Client() : _fd(-1), _address(), _nickname(""), _username(""),
	_hostname(""), _inBuffer(""), _authenticated(false)
{
}

Client::Client(int fd, const sockaddr_in &address) : _fd(fd), _address(address),
	_nickname(""), _username(""), _hostname(""), _inBuffer(""),
	_authenticated(false)
{
}

Client::Client(const Client &other) : _fd(other._fd), _address(other._address),
	_nickname(other._nickname), _username(other._username),
	_hostname(other._hostname), _inBuffer(other._inBuffer),
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
		_inBuffer = other._inBuffer;
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

const std::string &Client::getInBuffer() const
{
	return (_inBuffer);
}

std::string &Client::getOutBuffer()
{
	return (_outBuffer);
}

bool Client::isAuthenticated() const
{
	return (_authenticated);
}

bool Client::isRegistered() const
{
	return (!_nickname.empty() && !_username.empty() && !_hostname.empty());
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


// ************************************************************************** //
//                              Buffer
// ************************************************************************** //

static const size_t	MAX_LINE_LENGTH = 512;
// RFC 1459: max IRC line length, CRLF included

void Client::appendInBuffer(const char *data, size_t len)
{
	_inBuffer.append(data, len);
	if (_inBuffer.size() > MAX_LINE_LENGTH
		&& _inBuffer.find('\n') == std::string::npos)
		_inBuffer.erase(MAX_LINE_LENGTH);
	// no line ending beyond the limit: bound memory usage
}

void Client::appendOutBuffer(const std::string &message)
{
	_outBuffer += message;
}

void Client::clearInBuffer()
{
	_inBuffer.clear();
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