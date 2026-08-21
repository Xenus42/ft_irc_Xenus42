/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 14:48:40 by lebroue           #+#    #+#             */
/*   Updated: 2026/08/21 15:43:02 by lebroue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Parsing.hpp"
#include "Server.hpp"
#include "Tmp_Log.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

// TODEL:
#include "Channel.hpp"
#include "Commands.hpp"

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
	pollfd	serverPoll;

	createSocket();
	bindSocket();
	listenSocket();
	_running = true;
	serverPoll.fd = _listenFd;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollfds.push_back(serverPoll);
}

void Server::run()
{
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
				continue ;
			if (_pollfds[i].fd == _listenFd)
			{
				acceptClient();
				// printServerStatus();
			}
			else
			{
				receiveClient(_pollfds[i].fd);
				printServerStatus();
			}
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
//                                   Communication
// ************************************************************************** //

void Server::sendMessage(Client *client, const std::string &message)
{
	send(client->getFd(), message.c_str(), message.size(), 0);
}

void Server::sendWelcome(Client *client)
{
	const std::string nick = client->getNickname();
	sendMessage(client, ":ircserv 001 " + nick
		+ " :Welcome to the IRC Network\r\n");
	sendMessage(client, ":ircserv 002 " + nick + " :Your host is ircserv\r\n");
	sendMessage(client, ":ircserv 003 " + nick
		+ " :This server was created today\r\n");
	// TODO: check message excepted
	sendMessage(client, ":ircserv 004 " + nick + " ircserv 1.0 io\r\n");
}
// ************************************************************************** //
//                                   Socket
// ************************************************************************** //

void Server::createSocket()
{
	int	opt;

	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0)
	{
		shutdown();
		throw std::runtime_error(std::string("socket: ")
			+ std::strerror(errno));
	}
	opt = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
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
	if (bind(_listenFd, (sockaddr *)&_address, sizeof(_address)))
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
		throw std::runtime_error(std::string("listen: ")
			+ std::strerror(errno));
	}
	std::cout << "[INFO  ] Listening on port " << _port << std::endl;
}

// ************************************************************************** //
//                                   Client
// ************************************************************************** //

void Server::acceptClient()
{
	sockaddr_in	clientAddr;
	socklen_t	len;
	int			clientFd;

	len = sizeof(clientAddr);
	clientFd = accept(_listenFd, (sockaddr *)&clientAddr, &len);
	if (clientFd < 0)
	{
		perror("accept");
		return ;
	}
	addClient(clientFd, clientAddr);
}

void Server::addClient(int fd, const sockaddr_in &address)
{
	Client	*client;

	client = new Client(fd, address);
	_clientsByFd[fd] = client;
	addPollfd(fd);
	std::cout << "[SERVER] Client " << _clientsByFd[fd] << " connected" << std::endl;
}

void Server::receiveClient(int fd)
{
	char	buffer[1024];
	ssize_t	bytes;
	Client	*client;

	bytes = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes < 0)
	{
		perror("recv");
		return ;
	}
	if (bytes == 0)
	{
		disconnectClient(_clientsByFd[fd]);
		return ;
	}
	client = _clientsByFd[fd];
	client->appendBuffer(buffer, bytes);
	std::string line;
	while (client->extractLine(line))
	{
		Parsing::Command cmd = _parser.parse(line);
		dispatchCommand(client, cmd);
	}
}

void Server::disconnectClient(Client *client)
{
	int		fd;
	delete	client;

	if (!client)
		return ;
	fd = client->getFd();
	_clientsByFd.erase(fd);
	if (client->isAuthenticated())
	{
		std::string nickname = client->getNickname();
		_clientsByNick.erase(nickname);
	}
	removePollfd(fd);
	close(fd);
	printLog("SERVER", "Client disconnected and cleaned up");
}

void Server::changeNickname(Client *client, const std::string &newNick)
{
	const std::string &oldNick = client->getNickname();
	if (oldNick == newNick)
		return ;
	if (!oldNick.empty())
		_clientsByNick.erase(oldNick);
	client->setNickname(newNick);
	_clientsByNick[newNick] = client;
}

// ************************************************************************** //
//                                   Poll
// ************************************************************************** //

void Server::addPollfd(int fd)
{
	pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
}

void Server::removePollfd(int fd)
{
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			return ;
		}
	}
}

// ************************************************************************** //
//                                   Getters
// ************************************************************************** //

std::string Server::getPassword() const
{
	return (_password);
}

Client *Server::getClientByNick(std::string nick)
{
	std::map<std::string, Client *>::iterator it = _clientsByNick.find(nick);
	if (it == _clientsByNick.end())
		return (NULL);
	return (it->second);
	;
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
					<< "\t fd: " << _pollfds[i].fd << "\t event: ";

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

	for (std::map<int,
		Client *>::const_iterator it = _clientsByFd.begin(); it != _clientsByFd.end(); ++it)
	{
		std::cout << "fd: " << it->first;

		if (it->second)
		{
			std::cout << "\t nick: " << it->second->getNickname() << "\t user: " << it->second->getUsername();
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

	for (std::map<std::string,
		Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		std::cout << "name: " << it->first;

		if (it->second)
		{
			std::cout << "\t topic: " << it->second->getTopic() << "\t members: " << it->second->getClientCount();
		}
		else
		{
			std::cout << "\tChannel: NULL";
		}

		std::cout << std::endl;
	}
	nl(2);
}

void Server::printServerStatus() const
{
	nl(1);
	printTitle("SERVER STATUS");
	printPollfds();
	printClientsByFd();
	printChannels();
}

// ************************************************************************** //
//                                  Channel
// ************************************************************************** //

Channel *Server::createChannel(const std::string &name)
{
	Channel	*channel;

	channel = new Channel(name);
	_channels[name] = channel;
	return (channel);
}

Channel *Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it == _channels.end())
		return (NULL);
	return (it->second);
}

void Server::removeChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it == _channels.end())
		return ;
	delete it->second;
	_channels.erase(it);
}
// ************************************************************************** //
//                                  Command Dispatch
// ************************************************************************** //

void Server::dispatchCommand(Client *client, const Parsing::Command &cmd)
{
	if (cmd.name == "JOIN")
		handleJoin(*this, client, cmd.params);
	else if (cmd.name == "TOPIC")
		handleTopic(*this, client, cmd.params);
	else if (cmd.name == "PASS")
		handlePass(*this, client, cmd.params);
	else if (cmd.name == "NICK")
		handleNick(*this, client, cmd.params);
	else if (cmd.name == "USER")
		handleUser(*this, client, cmd.params);
	else if (cmd.name == "MODE")
		handleMode(*this, client, cmd.params);
	else if (cmd.name == "PRIVMSG")
		handlePrivmsg(*this, client, cmd.params);
	else if (cmd.name == "NOTICE")
		handleNotice(*this, client, cmd.params);
	else if (cmd.name == "WHO")
		handleWho(*this, client, cmd.params);
	else if (cmd.name == "WHOIS")
		handleWhois(*this, client, cmd.params);
	else if (cmd.name == "LIST")
		handleList(*this, client, cmd.params);
	else
	{
		std::cout << "[SERVER] Unknown command: " << cmd.name << std::endl;
	}
}