#include "Server.hpp"

// ************************************************************************** //
//                                   Communication
// ************************************************************************** //

void Server::sendMessage(Client *client, const std::string &message)
{
	send(client->getFd(), message.c_str(), message.size(), 0);
}

void Server::sendWelcome(Client *client)
{
    const std::string nick = client->getNickname();

    sendMessage(client,
        ":ircserv 001 " + nick +
        " :Welcome to the IRC Network\r\n");

    sendMessage(client,
        ":ircserv 002 " + nick +
        " :Your host is ircserv\r\n");

    sendMessage(client,
        ":ircserv 003 " + nick +
        " :This server was created today\r\n");

	// TODO: check message excepted
    sendMessage(client,
        ":ircserv 004 " + nick +
        " ircserv 1.0 io\r\n");
}