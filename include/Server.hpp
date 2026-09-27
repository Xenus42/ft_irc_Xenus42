#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Parsing.hpp"
#include <csignal>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <vector>

class Server
{
	// ---------------- Server.cpp ----------------

	// === Orthodox Canonical Form ===
  private:
	Server(const Server &other);
	Server &operator=(const Server &other);

  public:
	Server();
	Server(int _port, const std::string &password);
	~Server();

	// === Life Cycle ===
	void init();
	static void signalHandler(int sig);
	void run();
	void shutdown();

	// === Getters ===
	std::string getPassword() const;

	// ---------------- ServerChannels.cpp ----------------

	// === Channel lifecycle ===
	Channel *createChannel(const std::string &name);
	Channel *getChannel(const std::string &name);
	void removeChannel(const std::string &name);
	const std::map<std::string, Channel *> &getChannels() const;

	// ---------------- ServerClients.cpp ----------------

	// === Client lifecycle ===
	void addClient(int fd, const sockaddr_in &address);
	void disconnectClient(Client *client, const std::string &reason);
	void disconnectAllClients();

	// === Client Channels ===
	void removeClientFromChannels(Client *client);
	void broadcastQuit(Client *client, const std::string &reason);

	// === Client Identity ===
	void changeNickname(Client *client, const std::string &newNick);

	// === Client Indexes ===
	void addClientByFd(Client *client);
	Client *getClientByFd(int fd);
	void removeClientByFd(Client *client);
	void addClientByNick(Client *client);
	Client *getClientByNick(const std::string &nick);
	void removeClientByNick(Client *client);

	// ---------------- ServerCommands.cpp ----------------

	// === Command Dispatch ===
	void dispatchCommand(Client *client, const Parsing::Command &cmd);

	// ---------------- ServerNetwork.cpp ----------------

	// === Socket ===
  private:
	void createSocket();
	void bindSocket();
	void listenSocket();

  public:
	// === Poll ===
	void addPollfd(int fd);
	void processPollEvents();
	void removePollfd(int fd);

	// === Client I/O ===
	void acceptClient();
	void receiveClient(int fd);

	// ---------------- ServerCommunication.cpp ----------------

	// === Communication ===
	void sendMessage(Client *client, const std::string &message);
	void sendClient(int fd);
	void sendWelcome(Client *client);

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
