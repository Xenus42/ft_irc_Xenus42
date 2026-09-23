#pragma once

#include "Server.hpp"
#include <string>
#include <netinet/in.h>

class Client
{
    public:
        // === OCF ===
        Client();
        Client(int fd, const sockaddr_in& address);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();

        // === Getters ===
        int                 getFd() const;
        const std::string   &getNickname() const;
        const std::string   &getUsername() const;
        const std::string   &getHostname() const;
        const std::string   &getInBuffer() const;
        std::string         &getOutBuffer();
        bool                isAuthenticated() const;

        // === Setters ===
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setHostname(const std::string &hostname);
        void setAuthenticated(bool value);
        void setServerName(const std::string &servername);
        void setRealName(const std::string &realname);
        void setPassword(const std::string &password);

        // === Buffer ===
        void appendInBuffer(const char *data, size_t len);
        void appendOutBuffer(const std::string &message);
        void clearInBuffer();
        bool extractLine(std::string &line);

        // === Registration ===
        bool isRegistered() const;
        bool hasPassword() const;

		friend class Server;

    private:
        int             _fd;
        sockaddr_in     _address;

        std::string     _nickname;
        std::string     _username;
        std::string     _hostname;
        std::string     _servername;
        std::string     _realname;
        std::string     _password;
        std::string     _inBuffer;
        std::string     _outBuffer;
        bool            _authenticated;
};