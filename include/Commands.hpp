#pragma once

#include "Server.hpp"

// TODEL: del Class Server ??
class	Server;

void handleJoin(Server &server, Client *client, const std::vector<std::string> &params);
void handleTopic(Server &server, Client *client, const std::vector<std::string> &params);
void handlePass(Server &server, Client *client, const std::vector<std::string> &params);
void handleNick(Server &server, Client *client, const std::vector<std::string> &params);
void handleUser(Server &server, Client *client, const std::vector<std::string> &params);
