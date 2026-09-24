#include "Server.hpp"

// ************************************************************************** //
//                                  Command Dispatch
// ************************************************************************** //

void Server::dispatchCommand(Client *client, const Parsing::Command &cmd)
{
	if (!client->isRegistered() && cmd.name != "PASS" && cmd.name != "NICK"
		&& cmd.name != "USER" && cmd.name != "CAP" && cmd.name != "QUIT" && cmd.name != "PING"
		&& cmd.name != "PONG")
	{
		const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
		sendMessage(client, ":ircserv 451 " + nick + " :You have not registered");
		return ;
	}
	if (cmd.name == "JOIN")
		handleJoin(*this, client, cmd.params);
	else if (cmd.name == "PART")
		handlePart(*this, client, cmd.params);
	else if (cmd.name == "TOPIC")
		handleTopic(*this, client, cmd.params);
	else if (cmd.name == "PASS")
		handlePass(*this, client, cmd.params);
	else if (cmd.name == "NICK")
		handleNick(*this, client, cmd.params);
	else if (cmd.name == "USER")
		handleUser(*this, client, cmd.params);
	else if (cmd.name == "MODE")
		handleMode(*this, client, cmd.params);
	else if (cmd.name == "INVITE")
		handleInvite(*this, client, cmd.params);
	else if (cmd.name == "KICK")
		handleKick(*this, client, cmd.params);
	else if (cmd.name == "PRIVMSG")
		handlePrivmsg(*this, client, cmd.params);
	else if (cmd.name == "NOTICE")
		handleNotice(*this, client, cmd.params);
	else if (cmd.name == "WHO")
		handleWho(*this, client, cmd.params);
	else if (cmd.name == "WHOIS")
		handleWhois(*this, client, cmd.params);
	else if (cmd.name == "LIST")
		handleList(*this, client, cmd.params);
	else if (cmd.name == "PONG")
		return ;
	else if (cmd.name == "PING")
		handlePing(*this, client, cmd.params);
	else if (cmd.name == "QUIT")
		handleQuit(*this, client, cmd.params);
	else if (cmd.name == "CAP")
		handleCap(*this, client, cmd.params);
	else
	{
		sendMessage(client, ":ircserv 421 " + client->getNickname() + " " + cmd.name + " :Unknown command");
	}
}
