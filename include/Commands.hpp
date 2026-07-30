#pragma once

#include "Server.hpp"

void handleJoin(Server &server, Client *client, const std::vector<std::string> &params);