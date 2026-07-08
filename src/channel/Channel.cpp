/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: williamguerreiro <williamguerreiro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:00:48 by williamguer       #+#    #+#             */
/*   Updated: 2026/07/08 17:01:11 by williamguer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string &name)
    : _name(name), _topic(""), _key(""), _userLimit(0), _modes(0)
{
}

Channel::~Channel()
{
}

// ---- Statut interne (privé) ----

bool Channel::hasStatus(Client *client, int flag) const
{
    std::map<Client*, int>::const_iterator it = _status.find(client);
    if (it == _status.end())
        return false;
    return (it->second & flag) != 0;
}

void Channel::setStatus(Client *client, int flag)
{
    _status[client] |= flag;
}

void Channel::unsetStatus(Client *client, int flag)
{
    std::map<Client*, int>::iterator it = _status.find(client);
    if (it == _status.end())
        return;
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
    return hasStatus(client, STATUS_MEMBER);
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
    return hasStatus(client, STATUS_OPERATOR);
}

// ---- Invitations ----

void Channel::addInvited(Client *client)
{
    setStatus(client, STATUS_INVITED);
}

bool Channel::isInvited(Client *client) const
{
    return hasStatus(client, STATUS_INVITED);
}

void Channel::removeInvited(Client *client)
{
    unsetStatus(client, STATUS_INVITED);
}

// ---- Topic ----

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

const std::string &Channel::getTopic() const
{
    return _topic;
}

// ---- Key ----

void Channel::setKey(const std::string &key)
{
    _key = key;
    setMode(MODE_HAS_KEY);
}

const std::string &Channel::getKey() const
{
    return _key;
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
    return _userLimit;
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
    return (_modes & flag) != 0;
}

// ---- Utils ----

const std::string &Channel::getName() const
{
    return _name;
}

size_t Channel::getClientCount() const
{
    size_t count = 0;
    std::map<Client*, int>::const_iterator it;
    for (it = _status.begin(); it != _status.end(); ++it)
    {
        if (it->second & STATUS_MEMBER)
            count++;
    }
    return count;
}

bool Channel::isEmpty() const
{
    return getClientCount() == 0;
}

//reste broadcast et getmemberslist