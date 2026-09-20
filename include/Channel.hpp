/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wdauser <wdauser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:01:31 by williamguer       #+#    #+#             */
/*   Updated: 2026/08/22 02:02:09 by wdauser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <string>
#include <map>
#include <set>
#include <vector>

enum	ChannelModeFlags
{
	MODE_INVITE_ONLY = 1 << 0,      // +i
	MODE_TOPIC_RESTRICTED = 1 << 1, // +t
	MODE_HAS_KEY = 1 << 2,          // +k
	MODE_HAS_LIMIT = 1 << 3         // +l
};
enum	ClientChannelStatus
{
	STATUS_MEMBER = 1 << 0,
	STATUS_OPERATOR = 1 << 1
};

class	Client;
class	Server;

class Channel
{
  public:
	Channel(const std::string &name);
	~Channel();
	// Gestion des membres
	void addClient(Client *client);
	void removeClient(Client *client);
	bool isMember(Client *client) const;
	// Gestion des opérateurs
	void addOperator(Client *client);
	void removeOperator(Client *client);
	bool isOperator(Client *client) const;
	// Invitations
	void addInvited(Client *client);
	bool isInvited(Client *client) const;
	void removeInvited(Client *client);
	// Topic
	void setTopic(const std::string &topic);
	const std::string &getTopic() const;
	// Key
	void setKey(const std::string &key);
	const std::string &getKey() const;
	void removeKey();
	// User limit
	void setUserLimit(int limit);
	int getUserLimit() const;
	void removeUserLimit();
	// Modes
	void setMode(int flag);
	void unsetMode(int flag);
	bool hasMode(int flag) const;
	// Utils
	const std::string &getName() const;
	size_t getClientCount() const;
	bool isEmpty() const;
	void broadcast(Server &server, const std::string &message, Client *exclude = NULL);
	std::string getMemberList() const;
	std::vector<Client *> getMembers() const;

  private:
	// Statut interne d'un client (bits STATUS_*)
	bool hasStatus(Client *client, int flag) const;
	void setStatus(Client *client, int flag);
	void unsetStatus(Client *client, int flag);

    std::string             _name;
    std::string             _topic;
    std::string             _key;
    int                     _userLimit;
    int                     _modes;
    std::map<Client*, int>  _status;
	std::set<Client*> 		_invited;
    Channel();
};
void	reply(Client *client, const std::string &line);

#endif
