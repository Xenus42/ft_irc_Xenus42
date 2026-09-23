#include "Server.hpp"
#include "Tmp_Log.hpp"

// "Special" characters allowed in a nick (RFC 2812), at any position.
static bool	isSpecial(unsigned char c)
{
	return (c == '[' || c == ']' || c == '\\' || c == '^' || c == '_'
		|| c == '`' || c == '{' || c == '|' || c == '}');
}

static bool	isValidNickname(const std::string &nick)
{
	unsigned char	first;
	unsigned char	c;

	if (nick.empty() || nick.size() > 9)
		return (false);
	first = static_cast<unsigned char>(nick[0]);
	if (!isalpha(first) && !isSpecial(first))
		return (false);
	for (size_t i = 1; i < nick.size(); i++)
	{
		c = static_cast<unsigned char>(nick[i]);
		if (!isalnum(c) && !isSpecial(c) && c != '-')
			return (false);
	}
	return (true);
}

void	handleNick(Server &server, Client *client,
		const std::vector<std::string> &params)
{
	const std::string target = client->getNickname().empty() ? "*" : client->getNickname();

	if (params.empty())
	{
		reply(client, ":ircserv 431 " + target + " :No nickname given");
		return ;
	}

	const std::string nick = params[0];

	if (!isValidNickname(nick))
	{
		reply(client, ":ircserv 432 " + target + " " + nick
			+ " :Erroneous nickname");
		return ;
	}

	Client *existing = server.getClientByNick(nick);
	if (existing == client)
		return ;
			// same nick (possibly different case): silent no-op

	if (existing)
	{
		reply(client, ":ircserv 433 " + target + " " + nick
			+ " :Nickname is already in use");
		return ;
	}

	server.changeNickname(client, nick);

	if (client->isRegistered())
		server.sendWelcome(client);
}