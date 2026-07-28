// lib C++
#include <cstdlib>  // atoi
#include <iostream> // std::cout, std::cerr
#include <map>      // std::map
#include <set>      // std::set
#include <sstream>  // std::stringstream
#include <string>   // std::string
#include <vector>   // std::vector

// lib POSIX / Reseau
#include <errno.h>      // error
#include <netinet/in.h> // sockaddr_in
#include <poll.h>       // poll
#include <signal.h>     // signal

// include
#include "Client.hpp"
#include "Parsing.hpp"
#include "Server.hpp"
#include "Tmp_Log.hpp"
#include "Utils.hpp"

int	main(int ac, char **av)
{
	Parsing	parser;

	if (ac == 2 && std::string(av[1]) == "--parse-test") // TEST DU PARSING
	{
		Parsing::Command cmd = parser.parse("JOIN #general");
		std::cout << "command: " << cmd.name << std::endl;
		for (std::size_t i = 0; i < cmd.params.size(); ++i)
			std::cout << "param " << i << ": " << cmd.params[i] << std::endl;
		return (EXIT_SUCCESS);
	} // FIN TEST DU PARSING
	if (ac != 3 || !isValidPort(av[1]))
	{
		// TODO: specify error type
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
