#include "Server.hpp"

// ServerUtils.cpp
// ├── Communication
// │   ├── sendMessage()
// │   └── sendWelcome()
// │
// └── éventuellement
//     ├── broadcast()
//     └── broadcastToChannel()

// ************************************************************************** //
//                                   Print
// ************************************************************************** //

void Server::printPollfds() const
{
	printSection("Pollfds:");
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		std::cout << "[" << i << "]"
					<< "\t fd: " << _pollfds[i].fd << "\t event: ";

		if (_pollfds[i].events & POLLIN)
			std::cout << "POLLIN";

		if (_pollfds[i].revents & POLLOUT)
			std::cout << "POLLOUT";

		if (_pollfds[i].revents & POLLERR)
			std::cout << "POLLERR";

		if (_pollfds[i].revents & POLLHUP)
			std::cout << "POLLHUP";

		std::cout << std::endl;
	}
	nl(2);
}

void Server::printClientsByFd() const
{
	printSection("Clients:");

	for (std::map<int,
		Client *>::const_iterator it = _clientsByFd.begin(); it != _clientsByFd.end(); ++it)
	{
		std::cout << "fd: " << it->first;

		if (it->second)
		{
			std::cout << "\t nick: " << it->second->getNickname() << "\t user: " << it->second->getUsername();
		}
		else
		{
			std::cout << "\tClient: NULL";
		}

		std::cout << std::endl;
	}
	nl(2);
}

void Server::printChannels() const
{
	printSection("Channels:");

	for (std::map<std::string,
		Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		std::cout << "name: " << it->first;

		if (it->second)
		{
			std::cout << "\t topic: " << it->second->getTopic() << "\t members: " << it->second->getClientCount();
		}
		else
		{
			std::cout << "\tChannel: NULL";
		}

		std::cout << std::endl;
	}
	nl(2);
}

void Server::printServerStatus() const
{
	nl(1);
	printTitle("SERVER STATUS");
	printPollfds();
	printClientsByFd();
	printChannels();
}