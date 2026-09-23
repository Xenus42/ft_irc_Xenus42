#include "Server.hpp"

static bool isValidUsername(const std::string &userName)
{
    if (userName.empty() || userName.size() > 255)
        return (false);
    if (userName.find(' ') != std::string::npos)
        return (false);
    if (userName.find('@') != std::string::npos)
        return (false);
    if (userName.find(':') != std::string::npos)
        return (false);
    if (userName.find('\0') != std::string::npos)
        return (false);
    if (userName.find('\r') != std::string::npos)
        return (false);
    if (userName.find('\n') != std::string::npos)
        return (false);
    return (true);   
}

static bool isValidRealname(const std::string &realname)
{
    if (realname.empty() || realname.size() > 255)
        return (false);
    if (realname.find('\0') != std::string::npos)
        return (false);
    if (realname.find('\r') != std::string::npos)
        return (false);
    if (realname.find('\n') != std::string::npos)
        return (false);    
    return (true);
}

static bool isValidHostname(const std::string &hostName)
{
    if (hostName.empty() || hostName.size() > 255)
        return (false);
    return (true);
}

static bool isValidServername(const std::string &serverName)
{
    if (serverName.empty() || serverName.size() > 255)
        return (false);
    return (true);
}

void handleUser(Server &server, Client *client, const std::vector<std::string> &params)
{
    const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();

    if (client->isRegistered())
        return reply(client, ":ircserv 462 " + nick + " :You may not reregister");

    if (params.size() < 4)
    {
        reply(client, ":ircserv 461 " + nick + " USER :Not enough parameters");
        return;
    }


    const std::string &username = params[0];
    const std::string &hostname = params[1];
    const std::string &servername = params[2];
    const std::string &realname = params[3];


    if (!isValidUsername(username) ||
        !isValidRealname(realname) ||
        !isValidHostname(hostname) ||
        !isValidServername(servername))
    {
        reply(client, ":ircserv 461 " + nick + " USER :Invalid parameters");
        return;
    }
    client->setUsername(username);
    client->setHostname(hostname);
    client->setServerName(servername);
    client->setRealName(realname);

    if (client->isRegistered())
        server.sendWelcome(client);
}