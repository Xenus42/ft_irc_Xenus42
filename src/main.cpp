/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 11:29:19 by lebroue           #+#    #+#             */
/*   Updated: 2026/07/28 15:32:08 by lebroue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		// TESTS DE BASE POUR LE PARSING
		// 1 Commande JOIN avec espaces et tabulations autour du parametre
		// 2 Commande PASS avec un parametre suivi d'un retour a la ligne
		// 3 Commande JOIN avec plusieurs parametres
		// 4 Ligne vide
		// 5 Commande avec plusieurs espaces entre mots
		// 6 Parametre IRC avec deux points et espaces
		// 7 Cas IRC avec un message libre apres des parametres
		// 8 Autre cas IRC avec cible et message libre
		const std::string tests[] = {"  JOIN\t\t#general  ", "PASS\t1234\r\n",
			"JOIN #chan password", "", "josie    alice   boby",
			"PRIVMSG #chan :hello there", "NOTICE alice :bonjour les amis",
			"TOPIC #general :nouveau sujet"};
		for (std::size_t i = 0; i < 8; ++i)
		{
			Parsing::Command cmd = parser.parse(tests[i]);
			std::cout << "input: '" << tests[i] << "'" << std::endl;
			std::cout << "command: " << cmd.name << std::endl;
			for (std::size_t j = 0; j < cmd.params.size(); ++j)
				std::cout << "param " << j << ": " << cmd.params[j] << std::endl;
			std::cout << std::endl;
		}
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
