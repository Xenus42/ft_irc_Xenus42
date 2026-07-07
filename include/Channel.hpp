/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: willda-s <willda-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 16:48:32 by williamguer       #+#    #+#             */
/*   Updated: 2026/07/07 10:42:01 by willda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Client;

enum ChannelModeFlags
{
    MODE_INVITE_ONLY      = 1 << 0, // +i
    MODE_TOPIC_RESTRICTED = 1 << 1, // +t
    MODE_HAS_KEY          = 1 << 2, // +k
    MODE_HAS_LIMIT        = 1 << 3  // +l
};

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

        void broadcast(const std::string &message, Client *exclude = NULL);
        std::string getMemberList() const;

    private:
        std::string        _name;
        std::string        _topic;
        std::string        _key;
        int                 _userLimit;
        int                 _modes;

        std::set<Client*>   _members;
        std::set<Client*>   _operators;
        std::set<Client*>   _invited;

        Channel();
};

#endif