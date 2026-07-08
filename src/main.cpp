// lib C++
#include <iostream>     // std::cout, std::cerr
#include <string>       // std::string
#include <vector>       // std::vector
#include <map>          // std::map
#include <set>          // std::set
#include <sstream>      // std::stringstream
#include <cstdlib>      // atoi

// lib POSIX / Reseau
#include <netinet/in.h>     // sockaddr_in
#include <poll.h>           // poll
#include <signal.h>         // signal
#include <errno.h>          // error

// include
#include "Server.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include "Tmp_Log.hpp"

int main(int ac, char **av)
{
    if (ac != 3 || !isValidPort(av[1]))
    {
        // TODO: specify error type
        std::cerr << "Error: bad arguments" << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        Server server(std::atoi(av[1]), av[2]);
        server.init();
        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
