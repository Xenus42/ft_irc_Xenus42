#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	handleWho(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	Channel	*channel;
	Client	*member;

	const std::string nick = client->getNickname();
	if (params.empty())
		return (reply(client, ":ircserv 461 " + nick
				+ " WHO :Not enough parameters"));
	const std::string name = params[0];
	channel = server.getChannel(name);
	if (channel == NULL)
		return (reply(client, ":ircserv 315 " + nick + " " + name
				+ " :End of WHO list"));
	std::vector<Client *> members = channel->getMembers();
	// une ligne 352 par membre du channel
	for (size_t i = 0; i < members.size(); i++)
	{
		member = members[i];
		// format 352 : <chan> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
		reply(client, ":ircserv 352 " + nick + " " + name + " "
			+ member->getUsername() + " " + member->getHostname() + " ircserv "
			+ member->getNickname() + " H :0 " + member->getUsername());
	}
	// une seule ligne 315 de fin, apres toute la boucle
	reply(client, ":ircserv 315 " + nick + " " + name + " :End of WHO list");
}
