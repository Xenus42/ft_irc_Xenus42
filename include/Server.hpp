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
        Server(int _port, std::string password);
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();

        // === Life cycle ===
        void init();
        void run();
        void shutdown();

        // === Client ===
        void    acceptClient();
        // void    receiveClient();
        // void    disconnectClient();
        
        // === Channel ===
        // Channel *getChannel(std::string nick);
        // void    createChannel(std::string nick);
        // void    removeChannel(std::string nick);
    
    private:
        // === Socket ===
        void createSocket();
        void bindSocket();
        void listenSocket();

    private:
    
        int         _port;
        std::string _password;
        int         _socketFd;
        bool        _run;
        sockaddr_in _address;
        std::vector<pollfd> _pollfds;
};