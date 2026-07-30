#pragma once

#include <netinet/in.h>
#include <vector>
#include <map>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"

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
        void    init();
        void    run();
        void    shutdown();

        // === Client ===
        void    acceptClient();
        void    addClient(int fd, const sockaddr_in& address);
        void    receiveClient(int fd);
        // void    removeClient();

        // === Channel ===
        Channel *getChannel(const std::string &name);
        Channel *createChannel(const std::string &name);
        void     removeChannel(const std::string &name);

        // === Poll ===
        void    addPollfd(int fd);
        void    removePollfd(int fd);

    private:
        // === Socket ===
        void    createSocket();
        void    bindSocket();
        void    listenSocket();

        // === Print ===
        void    printPollfds() const;
        void    printClientsByFd() const;
        void    printServerStatus() const;

    private:
    
        int         _port;
        std::string _password;
        int         _listenFd;
        bool        _running;
        
        sockaddr_in _address;

        // === Collections === 
        std::vector<pollfd>             _pollfds;
        std::map<int, Client*>          _clientsByFd;
        std::map<std::string, Client*>  _clientsByNick;
        std::map<std::string, Channel*> _channels;
    };