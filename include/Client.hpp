#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstddef>
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
		const std::string   &getRealname() const;
        std::string         &getOutBuffer();
        bool                isAuthenticated() const;
        bool                isClosing() const;
		bool				isRegistered() const;

        // === Setters ===
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setRealname(const std::string &realname);
        void setAuthenticated(bool value);
        void markClosing();

        // === Buffer ===
        void appendInBuffer(const char *data, size_t len);
        void appendOutBuffer(const std::string &message);
        bool extractLine(std::string &line);

    private:
        int             _fd;
        std::string     _nickname;
        std::string     _username;
        std::string     _hostname;
        std::string     _realname;
        std::string     _inBuffer;
        std::string     _outBuffer;
        bool            _authenticated;
        bool            _closing;
};

#endif
