#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <netinet/in.h>

class Client;
class Channel;
class CommandHandler;

class Server
{
public:
    Server(int port, const std::string& password);
    ~Server();

    // cycle de vie
    void init();
    void run();
    void shutdown();

    // accès
    int getPort() const;
    const std::string& getPassword() const;

    // clients
    Client* getClient(int fd);
    Client* getClient(const std::string& nickname);
    void removeClient(int fd);

    // channels
    Channel* getChannel(const std::string& name);
    void createChannel(const std::string& name);
    void removeChannel(const std::string& name);

    // envoi
    void sendToClient(int fd, const std::string& msg);
    void broadcast(const std::string& msg);

private:

    //-----------------------
    // Initialisation
    //-----------------------

    void createSocket();
    void bindSocket();
    void listenSocket();

    //-----------------------
    // Boucle principale
    //-----------------------

    void pollEvents();

    //-----------------------
    // Gestion réseau
    //-----------------------

    void acceptNewClient();
    void receiveData(Client& client);

    //-----------------------
    // Traitement IRC
    //-----------------------

    void processBuffer(Client& client);
    void executeCommand(Client& client, const std::string& line);

private:

    int                     _serverFd;
    int                     _port;
    std::string             _password;
    bool                    _running;

    sockaddr_in             _address;

    std::vector<pollfd>     _pollfds;

    std::map<int, Client*>              _clients;
    std::map<std::string, Channel*>     _channels;

    CommandHandler*         _commandHandler;
};

#endif


#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h>

class Client
{
public:
    Client();
    Client(int fd, const sockaddr_in& addr);
    ~Client();

    // Getters
    int getFd() const;
    const sockaddr_in& getAddress() const;

    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getHostname() const;
    const std::string& getRealname() const;

    bool isRegistered() const;
    bool isAuthenticated() const;

    // Setters
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setHostname(const std::string& hostname);
    void setRealname(const std::string& realname);

    void setAuthenticated(bool value);
    void setRegistered(bool value);

    // Buffer de réception
    void appendBuffer(const std::string& data);
    std::string& getBuffer();
    void clearBuffer();

    // Envoi
    void sendMessage(const std::string& message);

private:
    int             _fd;
    sockaddr_in     _addr;

    std::string     _nickname;
    std::string     _username;
    std::string     _hostname;
    std::string     _realname;

    bool            _authenticated;
    bool            _registered;

    std::string     _buffer;
};

#endif


#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Client;

class Channel
{
public:
    Channel();
    Channel(const std::string& name);
    ~Channel();

    // Informations
    const std::string& getName() const;
    const std::string& getTopic() const;

    bool hasClient(Client* client) const;
    bool isOperator(Client* client) const;
    bool isInviteOnly() const;
    bool hasKey() const;
    bool isUserLimitEnabled() const;
    bool isFull() const;

    // Gestion des membres
    void addClient(Client* client);
    void removeClient(Client* client);

    void addOperator(Client* client);
    void removeOperator(Client* client);

    // Configuration
    void setTopic(const std::string& topic);
    void setKey(const std::string& key);
    void setUserLimit(size_t limit);

    void setInviteOnly(bool value);
    void setTopicRestricted(bool value);

    // Communication
    void broadcast(const std::string& message);
    void broadcast(const std::string& message, Client* except);

private:
    std::string         _name;
    std::string         _topic;
    std::string         _key;

    bool                _inviteOnly;
    bool                _topicRestricted;

    size_t              _userLimit;

    std::set<Client*>   _clients;
    std::set<Client*>   _operators;
};

#endif