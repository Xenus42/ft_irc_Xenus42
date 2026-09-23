#include "Server.hpp"
#include "Tmp_Log.hpp"

static bool isValidNickname(const std::string &nick)
{
    if (nick.empty() || nick.size() > 9)
        return false;

    if (!isalpha(nick[0]) &&
        nick[0] != '[' &&
        nick[0] != ']' &&
        nick[0] != '\\' &&
        nick[0] != '`' &&
        nick[0] != '_' &&
        nick[0] != '^' &&
        nick[0] != '{' &&
        nick[0] != '|')
        return false;

    for (size_t i = 1; i < nick.size(); i++)
    {
        if (!isalnum(nick[i]) &&
            nick[i] != '-' &&
            nick[i] != '_')
            return false;
    }

    return true;
}

void handleNick(Server &server, Client *client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        reply(client, ":ircserv 431 :No nickname given");        
        return;
    }

    std::string nick = params[0];

    if (!isValidNickname(nick))
    {
        // TODO: check if reply message is ok
        reply(client, ":ircserv 432 :" + nick + ":Erroneous nickname");
        return;
    }

    if (server.getClientByNick(nick))
    {
        reply(client, ":ircserv 432 :" + nick + ":Nickname is already in use");
        return;
    }

    bool wasRegistered = client->isRegistered();
    server.changeNickname(client, nick);
    if (!wasRegistered && client->isRegistered())
	    server.sendWelcome(client);
}