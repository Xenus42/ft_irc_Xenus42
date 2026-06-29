#pragma once

class Server
{
    public:
        Server();
        Server(const Server &other);
        Server&operator=(const Server &other) const;
        ~Server();

    private:
        int _socketFd;

}