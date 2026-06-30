#pragma once


#include <netinet/in.h>
#include <poll.h>

class Client
{
    public:
        // OCF
        Client();
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();

    private:
        int _socketFd;
        sockaddr_in _address;
};