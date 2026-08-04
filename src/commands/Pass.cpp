#include "Server.hpp"

void handlePass(Server &server, Client *client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        // Handle error: No password provided
        return;
    }

    const std::string &providedPassword = params[0];
    if (providedPassword == server.getPassword())
    {
        client->setAuthenticated(true);
        // Send success response to client
    }
    else
    {
        // Handle error: Incorrect password
        // Send error response to client
    }
}