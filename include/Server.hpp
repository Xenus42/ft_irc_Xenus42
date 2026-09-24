#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Parsing.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "Channel.hpp"
#include "Commands.hpp"

class Server
{
	// **************************************************************************
		//
	//                                  Server Main
	// **************************************************************************
		//

	// === Constructors ===
  private:
	Server(const Server &other);
	Server &operator=(const Server &other);

  public:
	Server();
	Server(int _port, std::string password);
	~Server();

	// === Life cycle ===
	void init();
	void run();
	void shutdown();
	void processPollEvents();
	static void signalHandler(int sig);

	// === Getters ===
	std::string getPassword() const;

	// **************************************************************************
		//
	//                                  Server Channels
	// **************************************************************************
		//

	Channel *getChannel(const std::string &name);
	Channel *createChannel(const std::string &name);
	void removeChannel(const std::string &name);
	const std::map<std::string, Channel *> &getChannels() const;

	// **************************************************************************
		//
	//                                  Server Clients
	// **************************************************************************
		//

	// === Client lifecycle ===
	void addClient(int fd, const sockaddr_in &address);
	void disconnectClient(Client *client, const std::string &reason);
	void disconnectAllClients();

	// === Client Identity ===
	void changeNickname(Client *client, const std::string &newNick);
	Client *getClientByNick(const std::string &nick);
	void removeClientFromChannels(Client *client);
	void broadcastQuit(Client *client, const std::string &reason);

	// === Client Indexes ===
	void addClientByFd(Client *client);
	Client *getClientByFd(int fd);
	void removeClientByFd(Client *client);

	void addClientByNick(Client *client);
	void removeClientByNick(Client *client);

	// **************************************************************************
		//
	//                                  Server Commands
	// **************************************************************************
		//

	void dispatchCommand(Client *client, const Parsing::Command &cmd);

	// **************************************************************************
		//
	//                                  Server Network
	// **************************************************************************
		//

	// === Socket ===
  private:
	void createSocket();
	void bindSocket();
	void listenSocket();

	// === Poll ===
  public:
	void addPollfd(int fd);
	void removePollfd(int fd);

	// === Client I/O ===
	void acceptClient();
	void receiveClient(int fd);

	// **************************************************************************
		//
	//                                  Server Communication
	// **************************************************************************
		//

	void sendMessage(Client *client, const std::string &message);
	void sendWelcome(Client *client);
	void sendClient(int fd);

  private:
	int _port;
	std::string _password;
	int _listenFd;
	bool _running;
	Parsing _parser;
	static volatile sig_atomic_t _signal;

	sockaddr_in _address;

	// === Collections ===
	std::vector<pollfd> _pollfds;
	std::map<int, Client *> _clientsByFd;
	std::map<std::string, Client *> _clientsByNick;
	std::map<std::string, Channel *> _channels;
};

#endif
