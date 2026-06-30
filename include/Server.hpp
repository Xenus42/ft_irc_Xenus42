#pragma once

#include <netinet/in.h>
#include <vector>
#include <poll.h>

#include "Client.hpp"

class Server
{
    public:
        // === OCF ===
        Server();
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();

        // === Life cycle ===
        void init();
        // void run();
        // void shutdown();

        // === Client ===
        // Client* getClient(int fd);
        // Client* getClient(const std::string& nickname);
        // void removeClient(int fd);
        
        // === Channel ===
        // Channel *getChannel(std::string nick);
        // void    createChannel(std::string nick);
        // void    removeChannel(std::string nick);
    
    private:
        // Members functions
        void createSocket();
        void bindSocket();
        void listenSocket();

    private:
        int _socketFd;
        sockaddr_in _address;
        // int _port;
        // std::string _password;
        // std::vector<pollfd> pollfds;
};