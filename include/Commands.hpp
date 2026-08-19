#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include <string>
#include <vector>

// TODEL: del Class Server ??
class	Server;

void	handleJoin(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handlePart(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleTopic(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handlePass(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleNick(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleUser(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleMode(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handlePrivmsg(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleNotice(Server &server, Client *client,
			const std::vector<std::string> &params);
