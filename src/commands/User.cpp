#include "Server.hpp"

void handleUser(Server &server, Client *client, const std::vector<std::string> &params)
{
    if (params.size() < 4)
    {
        // Handle error: Not enough parameters
        return;
    }

    const std::string &username = params[0];
    const std::string &hostname = params[1];
    const std::string &servername = params[2];
    const std::string &realname = params[3];

    client->setUsername(username);
    client->setHostname(hostname);
    client->setServerName(servername);
    client->setRealName(realname);

    (void)server;
    // Send success response to client
}