/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: willda-s <willda-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:00:48 by williamguer       #+#    #+#             */
/*   Updated: 2026/08/11 13:36:57 by willda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string &name) : _name(name), _topic(""), _key(""),
	_userLimit(0), _modes(0), _status(), _invited()
{
}

Channel::~Channel()
{
}

// ---- Statut interne (privé) ----

bool Channel::hasStatus(Client *client, int flag) const
{
	std::map<Client *, int>::const_iterator it = _status.find(client);
	if (it == _status.end())
		return (false);
	return ((it->second & flag) != 0);
}

void Channel::setStatus(Client *client, int flag)
{
	_status[client] |= flag;
}

void Channel::unsetStatus(Client *client, int flag)
{
	std::map<Client *, int>::iterator it = _status.find(client);
	if (it == _status.end())
		return ;
	it->second &= ~flag;
	if (it->second == 0)
		_status.erase(it);
}

// ---- Membres ----

void Channel::addClient(Client *client)
{
	setStatus(client, STATUS_MEMBER);
}

void Channel::removeClient(Client *client)
{
	unsetStatus(client, STATUS_MEMBER);
	unsetStatus(client, STATUS_OPERATOR);
	unsetStatus(client, STATUS_INVITED);
}

bool Channel::isMember(Client *client) const
{
	return (hasStatus(client, STATUS_MEMBER));
}

// ---- Opérateurs ----

void Channel::addOperator(Client *client)
{
	setStatus(client, STATUS_OPERATOR);
}

void Channel::removeOperator(Client *client)
{
	unsetStatus(client, STATUS_OPERATOR);
}

bool Channel::isOperator(Client *client) const
{
	return (hasStatus(client, STATUS_OPERATOR));
}
// ---- Invitations ----

void Channel::addInvited(Client *client)
{
	_invited.insert(client);
	setStatus(client, STATUS_INVITED);
}

bool Channel::isInvited(Client *client) const
{
	return (_invited.find(client) != _invited.end());
}

void Channel::removeInvited(Client *client)
{
	_invited.erase(client);
	unsetStatus(client, STATUS_INVITED);
}

// ---- Topic ----

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

const std::string &Channel::getTopic() const
{
	return (_topic);
}

// ---- Key ----

void Channel::setKey(const std::string &key)
{
	_key = key;
	setMode(MODE_HAS_KEY);
}

const std::string &Channel::getKey() const
{
	return (_key);
}

void Channel::removeKey()
{
	_key.clear();
	unsetMode(MODE_HAS_KEY);
}

// ---- User limit ----

void Channel::setUserLimit(int limit)
{
	_userLimit = limit;
	setMode(MODE_HAS_LIMIT);
}

int Channel::getUserLimit() const
{
	return (_userLimit);
}

void Channel::removeUserLimit()
{
	_userLimit = 0;
	unsetMode(MODE_HAS_LIMIT);
}

// ---- Modes ----

void Channel::setMode(int flag)
{
	_modes |= flag;
}

void Channel::unsetMode(int flag)
{
	_modes &= ~flag;
}

bool Channel::hasMode(int flag) const
{
	return ((_modes & flag) != 0);
}

// ---- Utils ----

const std::string &Channel::getName() const
{
	return (_name);
}

size_t Channel::getClientCount() const
{
	size_t count = 0;
	std::map<Client *, int>::const_iterator it;
	for (it = _status.begin(); it != _status.end(); ++it)
	{
		if (it->second & STATUS_MEMBER)
			count++;
	}
	return (count);
}

bool Channel::isEmpty() const
{
	return (getClientCount() == 0);
}

// ---- Diffusion ----
// Remplacer ::send par client.sendmessage()
void Channel::broadcast(const std::string &message, Client *exclude)
{
	std::string full = message + "\r\n";
	std::map<Client *, int>::const_iterator it;
	for (it = _status.begin(); it != _status.end(); ++it)
	{
		if (!(it->second & STATUS_MEMBER))
			continue ;
		if (it->first == exclude)
			continue ;
		::send(it->first->getFd(), full.c_str(), full.size(), 0);
	}
}

std::string Channel::getMemberList() const
{
	std::string list;

	std::map<Client *, int>::const_iterator it;
	for (it = _status.begin(); it != _status.end(); ++it)
	{
		if (!(it->second & STATUS_MEMBER))
			continue ;
		if (!list.empty())
			list += " ";
		if (it->second & STATUS_OPERATOR)
			list += "@";
		list += it->first->getNickname();
	}
	return (list);
}

std::vector<Client *> Channel::getMembers() const
{
	std::vector<Client *> members;

	std::map<Client *, int>::const_iterator it;
	for (it = _status.begin(); it != _status.end(); ++it)
	{
		if (it->second & STATUS_MEMBER)
			members.push_back(it->first);
	}
	return (members);
}