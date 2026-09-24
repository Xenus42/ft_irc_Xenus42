// lib C++
#include <cstdlib>  
#include <iostream>
#include <map>      
#include <set>      
#include <sstream>  
#include <string>   
#include <vector>

// lib POSIX / Reseau
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>

// include
#include "Client.hpp"
#include "Parsing.hpp"
#include "Server.hpp"
#include "Utils.hpp"

int	main(int ac, char **av)
{
	if (ac != 3 || !isValidPort(av[1]) || !isValidPassword(av[2]))
	{
		std::cerr << "Error: bad arguments" << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		Server server(std::atoi(av[1]), av[2]);
		server.init();
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
