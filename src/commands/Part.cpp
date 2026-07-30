#include <sys/socket.h>
#include <string>

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

// handlePart  —  PART <#channel> [:reason]
// (params deja decoupes par le parser : params[0] = nom, params[1] = raison optionnelle)
void handlePart(Server &server, Client *client,
                const std::vector<std::string> &params)
{
    const std::string nick = client->getNickname();

    // 1. Le client doit etre enregistre
    if (!client->isAuthenticated())
        return reply(client, ":ircserv 451 " + nick + " :You have not registered");

    // 2. Il faut au moins un nom de channel
    if (params.empty())
        return reply(client, ":ircserv 461 " + nick + " PART :Not enough parameters");

    const std::string name   = params[0];
    const std::string reason = (params.size() > 1) ? params[1] : "";

    // 3. Le channel doit exister
    Channel *channel = server.getChannel(name);
    if (channel == NULL)
        return reply(client, ":ircserv 403 " + nick + " " + name + " :No such channel");

    // 4. Le client doit etre membre du channel
    if (!channel->isMember(client))
        return reply(client, ":ircserv 442 " + nick + " " + name
                     + " :You're not on that channel");

    // 5. Prevenir TOUT LE MONDE (le partant inclus) AVANT de le retirer,
    //    sinon le partant ne recevrait pas sa propre confirmation.
    const std::string prefix = nick + "!" + client->getUsername() + "@" + client->getHostname();
    std::string       msg    = ":" + prefix + " PART " + name;
    if (!reason.empty())
        msg += " :" + reason;
    channel->broadcast(msg);

    // 6. Retirer le client du channel
    channel->removeClient(client);

    // 7. Si le channel est devenu vide, le detruire
    if (channel->isEmpty())
        server.removeChannel(name);
}
