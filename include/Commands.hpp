#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Client.hpp"
#include "Server.hpp"
#include <string>
#include <vector>

class	Server;
class	Client;

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
void	handleInvite(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleKick(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handlePrivmsg(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleNotice(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleWho(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleWhois(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handlePing(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleQuit(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleCap(Server &server, Client *client,
			const std::vector<std::string> &params);
void	handleList(Server &server, Client *client,
			const std::vector<std::string> &params);

#endif
