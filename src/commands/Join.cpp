#include <sys/socket.h>
#include <string>
#include "Channel.hpp"
#include "Client.hpp"

// TODEL:
#include <iostream>

void reply(Client *client, const std::string &line)
{
	Server serv;
    std::string full = line + "\r\n";
	serv.sendMessage(client, full);
}

// —  JOIN <#channel> [key]
void handleJoin(Server &server, Client *client,
                          const std::vector<std::string> &params)
{
    const std::string nick = client->getNickname();

    if (params.empty())
        return reply(client, ":ircserv 461 " + nick + " JOIN :Not enough parameters");

    const std::string  name = params[0];
    const std::string  key  = (params.size() > 1) ? params[1] : "";

    if (name.empty() || name[0] != '#')
        return reply(client, ":ircserv 403 " + nick + " " + name + " :No such channel");

    Channel *channel = server.getChannel(name);
    bool     created = false;

    if (channel == NULL)
    {
        channel = server.createChannel(name);
        created = true;
    }

    if (channel->isMember(client))
        return;

    if (!created)
    {
        if (channel->hasMode(MODE_HAS_KEY) && key != channel->getKey())
            return reply(client, ":ircserv 475 " + nick + " " + name + " :Cannot join channel (+k)");

        if (channel->hasMode(MODE_INVITE_ONLY) && !channel->isInvited(client))
            return reply(client, ":ircserv 473 " + nick + " " + name + " :Cannot join channel (+i)");

        if (channel->hasMode(MODE_HAS_LIMIT)
            && static_cast<int>(channel->getClientCount()) >= channel->getUserLimit())
            return reply(client, ":ircserv 471 " + nick + " " + name + " :Cannot join channel (+l)");
    }

    channel->addClient(client);
    channel->removeInvited(client);
    if (created)
        channel->addOperator(client);

    const std::string prefix = nick + "!" + client->getUsername() + "@" + client->getHostname();
    channel->broadcast(server, ":" + prefix + " JOIN " + name);

    if (!channel->getTopic().empty())
        reply(client, ":ircserv 332 " + nick + " " + name + " :" + channel->getTopic());
    else
        reply(client, ":ircserv 331 " + nick + " " + name + " :No topic is set");

    reply(client, ":ircserv 353 " + nick + " = " + name + " :" + channel->getMemberList());
    reply(client, ":ircserv 366 " + nick + " " + name + " :End of /NAMES list");
}
