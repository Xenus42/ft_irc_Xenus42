# Notes sur le parsing

## Objectif actuel

Creer un petit parseur IRC simple et progressif pour decouper un message en commande et en parametres.

## Comportement actuel

Le parseur fait pour l'instant :
- enlever les espaces et les tabulations au debut et a la fin
- separer l'entree en mots
- garder le premier mot comme nom de commande
- garder les autres mots comme parametres
- traiter les parametres precedes de `:` comme un seul parametre de texte libre
- ignorer un prefixe IRC optionnel au debut d'une ligne, comme `:nick!user@host COMMAND`
- conserver ce prefixe dans la structure de commande sous la forme `cmd.prefix`
- mettre le nom de commande en majuscules, car les commandes IRC sont insensibles a la casse
- gerer les lignes vides et les cas limites comme `QUIT :` ou `PART #chan :`
- conserver un parametre de texte libre vide lorsqu'il est present apres `:`
- conserver le texte libre tel quel apres le premier `:`, sans retirer les espaces
  ou les `:` supplementaires qu'il contient
- classifier les commandes IRC reconnues via une fonction simple de dispatch interne
- rejeter les prefixes invalides, comme une ligne reduite a `:` seul ou a des espaces apres `:`,
  en levant une erreur explicite plutot que de produire une commande vide
- fonctionner dans le mode de test `--parse-test` du programme principal

## Organisation interne

Le fichier [src/parsing/Parsing.cpp](../src/parsing/Parsing.cpp) est decoupe en petites fonctions statiques,
chacune avec un role precis, pour rester lisible :
- `isSeparator` : indique si un caractere separe des mots (espace, tabulation, `\r`, `\n`)
- `removeSpaceBeginningAndEndString` : enleve les separateurs en debut et en fin de chaine
- `toUpperCase` : met une chaine en majuscules
- `findFirstSeparator` : cherche la position du premier separateur a partir d'une position donnee
- `extractPrefix` : extrait le prefixe optionnel (`:nick!user@host `) et renvoie le reste de la ligne
- `findTextParamsStart` : cherche la position ou commence le texte libre marque par `:`
- `splitAndPutSimpleWordsIntoArray` : decoupe une chaine en mots simples separes par des espaces
- `splitCommandAndArgsToArray` : decoupe la commande et ses parametres, en gerant le texte libre final

`Parsing::parse` orchestre ces fonctions dans l'ordre : nettoyage des espaces, extraction du prefixe,
decoupage en mots, puis mise en majuscules du nom de commande.

## Cas de test de base

Le mode `--parse-test` dans [src/main.cpp](../src/main.cpp) est utilise pour valider a chaque fois les cas suivants, avec leurs exemples de chaine de test en argument :
- 1. `JOIN` avec un parametre et des espaces/tabulations autour : `"  JOIN\t\t#general  "`
- 2. `PASS` avec un parametre suivi d'un retour a la ligne : `"PASS\t1234\r\n"`
- 3. `JOIN` avec plusieurs parametres simples : `"JOIN #chan password"`
- 4. une ligne vide : `""`
- 5. plusieurs espaces entre mots : `"josie    alice   boby"`
- 6. un parametre IRC avec texte libre apres les deux points : `"PRIVMSG #chan :hello there"`
- 7. un autre cas avec cible et texte libre : `"NOTICE alice :bonjour les amis"`
- 8. plusieurs parametres suivis d'un texte libre : `"TOPIC #general :nouveau sujet"`
- 9. un cas avec plusieurs parametres simples et un mode : `"MODE #chan +o alice"`
- 10. un cas limite avec texte libre vide : `"QUIT :"`
- 11. un autre cas limite avec texte libre vide et une autre commande : `"PART #chan :"`
- 12. un cas avec plusieurs parametres simples sans texte libre : `"KICK #chan alice"`
- 13. un cas avec cible et texte libre : `"WHO #chan :names"`
- 14. un cas avec la commande LIST et un parametre simple : `"LIST #chan"`
- 15. un cas avec la commande NICK et un parametre unique : `"NICK alice"`
- 16. un cas avec la commande PRIVMSG et un texte libre : `"PRIVMSG bob :hello"`
- 17. un cas avec la commande INVITE et deux parametres : `"INVITE alice #chan"`
- 18. un cas avec la commande TOPIC et un texte libre : `"TOPIC #chan :new topic"`
- 19. un cas avec la commande KICK et deux parametres : `"KICK #chan alice"`
- 20. un cas avec la commande USER et plusieurs parametres : `"USER guest 0 * :Guest"`
- 21. un cas avec la commande MODE et un parametre simple : `"MODE #chan +i"`
- 22. un cas avec la commande NOTICE et un parametre simple : `"NOTICE alice :hello"`
- 23. un cas avec la commande WHO et deux parametres simples : `"WHO #chan alice"`
- 24. un cas avec la commande PRIVMSG et un texte libre vide : `"PRIVMSG #chan :"`
- 25. un cas avec un prefixe IRC avant la commande : `":nick!user@host PRIVMSG #chan :hello"`
- 26. un cas avec une commande en minuscules : `"join #chan"`
- 27. un cas avec un prefixe sur une commande de changement de surnom : `":nick!user@host NICK alice"`
- 28. un cas avec une commande PING et un texte libre : `"PING :server"`
- 29. un cas avec un parametre libre pour PASS : `"PASS :secret"`
- 30. un cas avec un parametre libre pour JOIN : `"JOIN #chan :topic"`
- 31. un cas avec un parametre libre pour QUIT : `"QUIT :leaving"`
- 32. un cas avec un parametre libre pour PART : `"PART #chan :bye"`
- 33. un cas avec plusieurs parametres pour MODE : `"MODE #chan +k secret"`
- 34. un cas avec une seconde commande de reponse IRC : `"PONG :server"`
- 35. un cas avec une commande d'extension CAP : `"CAP LS 302"`
- 36. un cas avec une commande d'authentification : `"AUTHENTICATE PLAIN"`
- 37. un cas avec une commande WHOIS : `"WHOIS alice"`
- 38. un cas avec un prefixe et plusieurs parametres libres : `":server PRIVMSG #chan alice :hello there friend"`
- 39. un cas invalide avec seulement `:` comme entree : `":"` (prefixe sans commande, leve `std::runtime_error("Invalid IRC prefix: missing command")`)
- 40. un cas invalide avec `:` suivi uniquement d'espaces : `":   "` (idem, leve la meme exception)
- 41. un cas invalide avec un prefixe valide mais sans commande : `":nick!user@host"` (idem, leve la meme exception)
- 42. un cas invalide avec un prefixe valide suivi uniquement d'espaces : `":nick!user@host   "` (idem, leve la meme exception)
- 43. un texte libre qui commence par un espace juste apres `:` : `"PRIVMSG #chan : hello there"` (le parametre garde l'espace de tete : `" hello there"`)
- 44. un texte libre qui contient un second `:` : `"NOTICE #chan ::double colon"` (le parametre garde le `:` supplementaire : `":double colon"`)

Ces cas sont actuellement consideres comme valides tant que la commande `./ircserv --parse-test` affiche le comportement attendu.

## Prochaines etapes

- gerer plusieurs parametres sur une meme ligne [DONE]
- gerer proprement les lignes vides [DONE]
- preparer une structure pour la logique propre aux commandes IRC [DONE]
- conserver le prefixe dans une structure de commande exploitable [DONE]
- rejeter les prefixes invalides (`:` seul ou suivi uniquement d'espaces) [DONE]
- rejeter un prefixe valide sans commande derriere [DONE]
- renforcer la logique de parsing sur des cas IRC plus complexes
- ajouter des tests supplementaires pour les commandes avec plusieurs parametres libres
- brancher ce parseur au flux de reception du serveur une fois la logique de parsing stabilisee
