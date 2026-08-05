#include "Server.hpp"
#include "Tmp_Log.hpp"

static bool isValidNickname(std::string nickName)
{
    if (nickName.length() > 30)
        return (false);
    return (true);
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

    const std::string &newNick = nick;

    client->setNickname(newNick);
    // TODO: Send success response to client
}