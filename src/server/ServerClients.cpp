/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerClients.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: willda-s <willda-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 17:47:41 by lebroue           #+#    #+#             */
/*   Updated: 2026/09/24 15:52:12 by willda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Utils.hpp"
#include <set>

// ************************************************************************** //
//                                  Client lifecycle
// ************************************************************************** //

void Server::addClient(int fd, const sockaddr_in &address)
{
	Client	*client;

	client = new Client(fd, address);
	addClientByFd(client);
	addPollfd(fd);
	std::cout << "[SERVER] Client " << fd << " connected" << std::endl;
}

void Server::disconnectClient(Client *client, const std::string &reason)
{
	int		fd;

	if (!client)
		return ;
	fd = client->getFd();
	broadcastQuit(client, reason);
	removeClientFromChannels(client);
	removeClientByNick(client);
	removeClientByFd(client);
	removePollfd(fd);
	close(fd);
	delete	client;
	std::cout << "[SERVER] Client disconnected and cleaned up" << std::endl;
}

void Server::disconnectAllClients()
{
	Client	*client;

	while (!_clientsByFd.empty())
	{
		client = _clientsByFd.begin()->second;
		disconnectClient(client, "Server shutting down");
	}
}

// ************************************************************************** //
//                                  Client Channels
// ************************************************************************** //

void Server::removeClientFromChannels(Client *client)
{
	Channel	*channel;

	if (!client)
		return ;
	std::map<std::string, Channel *>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		channel = it->second;
		++it;
		if (channel->isInvited(client))
			channel->removeInvited(client);
		if (channel->isMember(client))
		{
			channel->removeClient(client);
			if (channel->isEmpty())
			{
				const std::string name = channel->getName();
				removeChannel(name);
			}
		}
	}
}

// Sends QUIT once to every client sharing at least one channel with client.
void Server::broadcastQuit(Client *client, const std::string &reason)
{
	std::set<Client *>	targets;

	if (!client)
		return ;
	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (!it->second->isMember(client))
			continue ;
		std::vector<Client *> members = it->second->getMembers();
		targets.insert(members.begin(), members.end());
	}
	targets.erase(client);
	const std::string msg = ":" + client->getNickname() + "!" + client->getUsername()
		+ "@" + client->getHostname() + " QUIT :" + reason;
	for (std::set<Client *>::iterator it = targets.begin(); it != targets.end(); ++it)
		sendMessage(*it, msg);
}

// ************************************************************************** //
//                                  Client Identity
// ************************************************************************** //

void Server::changeNickname(Client *client, const std::string &newNick)
{
	if (!client)
		return ;
	const std::string &oldNick = client->getNickname();
	if (oldNick == newNick)
		return ;
	removeClientByNick(client);
	client->setNickname(newNick);
	addClientByNick(client);
}

// ************************************************************************** //
//                                  Client Indexes
// ************************************************************************** //

void Server::addClientByFd(Client *client)
{
	if (!client)
		return ;
	_clientsByFd[client->getFd()] = client;
}

Client *Server::getClientByFd(int fd)
{
	std::map<int, Client *>::iterator it = _clientsByFd.find(fd);
	if (it == _clientsByFd.end())
		return (NULL);
	return (it->second);
}

void Server::removeClientByFd(Client *client)
{
	if (!client)
		return ;
	_clientsByFd.erase(client->getFd());
}

void Server::addClientByNick(Client *client)
{
	if (!client)
		return ;
	const std::string &nick = client->getNickname();
	if (!nick.empty())
		_clientsByNick[toIrcUpper(nick)] = client;
}

Client *Server::getClientByNick(const std::string &nick)
{
	std::map<std::string,
		Client *>::iterator it = _clientsByNick.find(toIrcUpper(nick));
	if (it == _clientsByNick.end())
		return (NULL);
	return (it->second);
	;
}

void Server::removeClientByNick(Client *client)
{
	if (!client)
		return ;

	const std::string &nick = client->getNickname();

	if (!nick.empty())
		_clientsByNick.erase(toIrcUpper(nick));
}
