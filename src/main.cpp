// lib C++
#include <iostream>     // std::cout, std::cerr
#include <string>       // std::string
#include <vector>       // std::vector
#include <map>          // std::map
#include <set>          // std::set
#include <sstream>      // std::stringstream
#include <algorithm>    // std::find, std::remove
#include <cstdlib>      // atoi
#include <cstring>      // memset
#include <ctime>        // time

// lib POSIX / Reseau
#include <unistd.h>         // close
#include <fcntl.h>          // fcntl
#include <sys/socket.h>     // socket, bind...
#include <arpa/inet.h>      // htons, htonl, inet_ntoa
#include <netinet/in.h>     // sockaddr_in
#include <poll.h>           // poll
#include <signal.h>         // signal
#include <errno.h>          // error
#include <stdio.h>          // perror

int main(void)
{
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
        return (perror("socket"), 1);

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverFd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        return (perror("bind"), 1);

    if (listen(serverFd, 10) < 0)
        return (perror("listen"), 1);

    std::cout << "[SERVER] Waiting for client...\n";

    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    int clientFd = accept(serverFd, (sockaddr *)&clientAddr, &len);
    if (clientFd < 0)
        return (perror("accept"), 1);

    std::cout << "[SERVER] Client connected!\n";

    // ================= ECHO LOOP =================
    char buffer[1024];

    while (true)
    {
        int bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0)
        {
            std::cout << "[SERVER] Client disconnected\n";
            break;
        }

        buffer[bytes] = '\0';

        std::cout << "[RECEIVED] " << buffer;

        send(clientFd, buffer, bytes, 0);
    }

    close(clientFd);
    close(serverFd);
    return 0;
}