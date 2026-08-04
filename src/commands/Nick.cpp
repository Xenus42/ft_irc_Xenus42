#include "Server.hpp"

void handleNick(Server &server, Client *client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        // Handle error: No nickname provided
        return;
    }
    
    const std::string &newNick = params[0];
    client->setNickname(newNick);
    (void)server;

    // Send success response to client
}