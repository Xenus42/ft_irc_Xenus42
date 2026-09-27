#include "Server.hpp"
#include <Utils.hpp>
#include <cstdlib>
#include <iostream>

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
