#include "Server.hpp"
#include <set>

static bool	isSpecial(unsigned char c)
{
	return (c == '[' || c == ']' || c == '\\' || c == '^' || c == '_'
		|| c == '`' || c == '{' || c == '|' || c == '}');
}

static bool	isValidNickname(const std::string &nick)
{
	unsigned char	first;
	unsigned char	c;

	if (nick.empty() || nick.size() > 9)
		return (false);
	first = static_cast<unsigned char>(nick[0]);
	if (!isalpha(first) && !isSpecial(first))
		return (false);
	for (size_t i = 1; i < nick.size(); i++)
	{
		c = static_cast<unsigned char>(nick[i]);
		if (!isalnum(c) && !isSpecial(c) && c != '-')
			return (false);
	}
	return (true);
}

static void broadcastNickChange(Server &server, Client *client, const std::string &msg)
{
    std::set<Client *> targets;
    targets.insert(client);

    const std::map<std::string, Channel *> &channels = server.getChannels();
    std::map<std::string, Channel *>::const_iterator it;

    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (!it->second->isMember(client))
            continue;
        std::vector<Client *> members = it->second->getMembers();
        targets.insert(members.begin(), members.end());
    }

    std::set<Client *>::iterator target;
    for (target = targets.begin(); target != targets.end(); ++target)
        server.sendMessage(*target, msg);
}

void handleNick(Server &server, Client *client, const std::vector<std::string> &params)
{
    const std::string current = client->getNickname().empty() ? "*" : client->getNickname();

    if (params.empty() || params[0].empty())
        return server.sendMessage(client, ":ircserv 431 " + current + " :No nickname given");

    const std::string newNick = params[0];

    if (!isValidNickname(newNick))
        return server.sendMessage(client, ":ircserv 432 " + current + " " + newNick + " :Erroneous nickname");

    Client *owner = server.getClientByNick(newNick);
    if (owner == client)
        return;
    if (owner != NULL)
        return server.sendMessage(client, ":ircserv 433 " + current + " " + newNick + " :Nickname is already in use");

    const bool wasRegistered = client->isRegistered();
    const std::string oldPrefix = client->getNickname() + "!" + client->getUsername()
        + "@" + client->getHostname();

    server.changeNickname(client, newNick);

    if (wasRegistered)
        broadcastNickChange(server, client, ":" + oldPrefix + " NICK :" + newNick);
    else if (client->isRegistered())
        server.sendWelcome(client);
}
