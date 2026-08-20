#include "Server.hpp"

// ************************************************************************** //
//                                  Command Dispatch
// ************************************************************************** //

void Server::dispatchCommand(Client *client, const Parsing::Command &cmd)
{
	if (cmd.name == "JOIN")
		handleJoin(*this, client, cmd.params);
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
	else
	{
		std::cout << "[SERVER] Unknown command: " << cmd.name << std::endl;
	}
}