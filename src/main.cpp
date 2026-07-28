/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebroue <lebroue@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 11:29:19 by lebroue           #+#    #+#             */
/*   Updated: 2026/07/28 16:11:04 by lebroue          ###   ########.fr       */
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
		// 9 Cas IRC avec plusieurs parametres suivis d'un texte libre
		// 10 Cas avec un texte libre vide exemple = QUIT :->> apres les ":"
		// il n'y a rien, il l'interprete comme une seule commande,
		// ce n'est ni un vrai message, ni un parametre normal,
		// et c'est un cas limite ou le texte libre est vide apres la commande.
		// 11 Deuxieme cas avec un texte libre vide et une commande differente.
		// 12 Cas avec plusieurs parametres simples sans texte libre.
		// 13 Cas IRC avec un message libre apres une cible et un parametre supplementaire.
		// 14 Cas avec une commande LIST et un parametre simple.
		// 15 Cas avec une commande NICK et un parametre unique.
		// 16 Cas avec une commande PRIVMSG et un texte libre.
		// 17 Cas avec une commande INVITE et un parametre simple.
		// 18 Cas avec une commande TOPIC et un parametre simple.
		// 19 Cas avec une commande KICK et deux parametres simples.
		// 20 Cas avec une commande USER et deux parametres simples.
		// 21 Cas avec une commande MODE et un parametre simple.
		// 22 Cas avec une commande NOTICE et un parametre simple.
		// 23 Cas avec une commande WHO et deux parametres simples.
		const std::string tests[] = {"  JOIN\t\t#general  ", "PASS\t1234\r\n",
			"JOIN #chan password", "", "josie    alice   boby",
			"PRIVMSG #chan :hello there", "NOTICE alice :bonjour les amis",
			"TOPIC #general :nouveau sujet", "MODE #chan +o alice", "QUIT :",
			"PART #chan :", "KICK #chan alice", "WHO #chan :names",
			"LIST #chan", "NICK alice", "PRIVMSG bob :hello",
			"INVITE alice #chan", "TOPIC #chan :new topic", "KICK #chan alice",
			"USER guest 0 * :Guest", "MODE #chan +i", "NOTICE alice :hello",
			"WHO #chan alice"};
		std::size_t i = 0;
		while (i < 23)
		{
			Parsing::Command cmd = parser.parse(tests[i]);
			std::cout << "input: '" << tests[i] << "'" << std::endl;
			std::cout << "command: " << cmd.name << std::endl;
			std::size_t j = 0;
			while (j < cmd.params.size())
			{
				std::cout << "param " << j << ": " << cmd.params[j] << std::endl;
				++j;
			}
			std::cout << std::endl;
			++i;
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
