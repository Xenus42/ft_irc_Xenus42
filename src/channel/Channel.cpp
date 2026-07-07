/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: willda-s <willda-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:48:28 by williamguer       #+#    #+#             */
/*   Updated: 2026/07/07 10:41:57 by willda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name) : name(name), topic(""), key(""), userLimit(0), modeFlags(0)
{}

Channel::~Channel()
{}

Channel::Channel()
{
}

/* ******************** Gestion des membres *********************************** */
void Channel::addClient(Client *client)
{
	members.insert(client);
}

void Channel::removeClient(Client *client)
{
	members.erase(client);
}

void Channel::isMember(Client *client) const
{
	return members.find(client) != members.end();
}

/* ******************** Gestion des operateurs *********************************** */
void Channel::addOperator(Client *client)
{
	operators.insert(client);
}

void Channel::removeOperator(Client *client)
{
	operators.erase(client);
}

void Channel::isOperator(Client *client) const
{
	return operators.find(client) != operators.end();
}

/* ******************** Gestion des invités *********************************** */
void Channel::addInvited(Client *client)
{
	invited.insert(client);
}

void Channel::removeInvited(Client *client)
{
	invited.erase(client);
}

void Channel::isInvited(Client *client) const
{
	return invited.find(client) != invited.end();
}

/* ******************** Topic *********************************** */
void Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

const std::string &Channel::getTopic() const
{
	return topic;
}

/* ******************** Key *********************************** */
void Channel::setKey(const std::string &key)
{
	this->key = key;
}

const std::string &Channel::getKey() const
{
	return key;
}

void Channel::removeKey()
{
	key.clear();
}

/* ******************** User limit *********************************** */
void Channel::setUserLimit(int limit)
{
	userLimit = limit;
}

void Channel::getUserLimit() const
{
	return userLimit;
}

void Channel::removeUserLimit()
{
	userLimit = 0;
}

/* ******************** Gestion des modes *********************************** */
void Channel::setMode(int flag)
{
	modeFlags |= flag;
}
void Channel::unsetMode(int flag)
{
	modeFlags &= ~flag;
}

bool Channel::hasMode(int flag) const
{
	return (modeFlags & flag) != 0;
}

/* ******************** Utils *********************************** */
const std::string &Channel::getName() const
{
	return name;
}

size_t Channel::getClientCount() const
{
	return members.size();
}

bool Channel::isEmpty() const
{
	return members.empty();
}

void Channel::broadcast(const std::string &message, Client *exclude)
{
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if (*it != exclude)
		{
			(*it)->sendMessage(message);
		}
	}
}
std::string Channel::getMemberList() const
{
	std::string memberList;
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		memberList += (*it)->getNickname() + " ";
	}
	return memberList;
}