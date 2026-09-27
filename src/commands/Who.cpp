#include "Commands.hpp"


void	handleWho(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*member;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (server.sendMessage(client, ":ircserv 461 " + nick
				+ " WHO :Not enough parameters"));
	const std::string name = params[0];
	channel = server.getChannel(name);
	if (channel == NULL)
		return (server.sendMessage(client, ":ircserv 315 " + nick + " " + name
				+ " :End of WHO list"));
	std::vector<Client *> members = channel->getMembers();
	for (size_t i = 0; i < members.size(); i++)
	{
		member = members[i];
		// format 352 : <chan> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
		const std::string flags = channel->isOperator(member) ? "H@" : "H";
		server.sendMessage(client, ":ircserv 352 " + nick + " " + channel->getName() + " "
			+ member->getUsername() + " " + member->getHostname() + " ircserv "
			+ member->getNickname() + " " + flags + " :0 " + member->getRealname());
	}
	server.sendMessage(client, ":ircserv 315 " + nick + " " + channel->getName() + " :End of WHO list");
}
