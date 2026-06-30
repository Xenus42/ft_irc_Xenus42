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

// include
#include "Server.hpp"
#include "Client.hpp"

void    printInfo(std::string message)
{
    std::cout << "[INFO] " << message << std::endl;
}

void    nl(void)
{
    std::cout << std::endl;
}

int main(void)
{
    std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Testing Server orthodox canonical form" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
	{
        Server defaultServ;
        Server copyServ(defaultServ);
        Server assignateServ;

        assignateServ = defaultServ;
        nl();
    }

    std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Testing Client orthodox canonical form" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
    {
        Client defaultClient;
        Client copyClient(defaultClient);
        Client assignateClient;

        assignateClient = defaultClient;
        nl();
    }

    std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Testing minimal Server" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
    {
        Server server;

        server.init();
        
    }
    
    return (0);
}