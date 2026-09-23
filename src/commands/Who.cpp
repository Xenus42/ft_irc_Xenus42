#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	handleWho(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*member;

	const std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
	if (params.empty())
		return (reply(client, ":ircserv 461 " + nick
				+ " WHO :Not enough parameters"));
	const std::string name = params[0];
	channel = server.getChannel(name);
	if (channel == NULL)
		return (reply(client, ":ircserv 315 " + nick + " " + name
				+ " :End of WHO list"));
	std::vector<Client *> members = channel->getMembers();
	// one 352 line per channel member
	for (size_t i = 0; i < members.size(); i++)
	{
		member = members[i];
		// format 352: <chan> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
		reply(client, ":ircserv 352 " + nick + " " + name + " "
			+ member->getUsername() + " " + member->getHostname() + " ircserv "
			+ member->getNickname() + " H :0 " + member->getUsername());
	}
	// single closing 315 line, after the whole loop
	reply(client, ":ircserv 315 " + nick + " " + name + " :End of WHO list");
}
