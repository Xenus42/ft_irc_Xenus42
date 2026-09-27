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
- rejeter les prefixes invalides, comme une ligne reduite a `:` seul ou a des espaces apres `:`,
  en levant une erreur explicite plutot que de produire une commande vide

## Organisation interne

Le code est reparti en deux fichiers, tous deux membres de la classe `Parsing` (declares dans
[include/Parsing.hpp](../include/Parsing.hpp)) :

[src/parsing/Parsing.cpp](../src/parsing/Parsing.cpp) contient la logique principale :
- `parse` : orchestre les fonctions outils dans l'ordre (nettoyage des espaces, extraction du
  prefixe, decoupage en mots, mise en majuscules du nom de commande)

[src/parsing/Parsing_Utils.cpp](../src/parsing/Parsing_Utils.cpp) contient les petites fonctions
outils, chacune avec un role precis :
- `isSeparator` : indique si un caractere separe des mots (espace, tabulation, `\r`, `\n`)
- `removeSpaceBeginningAndEndString` : enleve les separateurs en debut et en fin de chaine
- `toUpperCase` : met une chaine en majuscules
- `findFirstSeparator` : cherche la position du premier separateur a partir d'une position donnee
- `extractPrefix` : extrait le prefixe optionnel (`:nick!user@host `) et renvoie le reste de la ligne
- `findTextParamsStart` : cherche la position ou commence le texte libre marque par `:`
- `splitAndPutSimpleWordsIntoArray` : decoupe une chaine en mots simples separes par des espaces
- `splitCommandAndArgsToArray` : decoupe la commande et ses parametres, en gerant le texte libre final

Toutes ces methodes sont `public` (aucune n'est `static` ni `private`) : `Parsing` doit etre
instanciee pour les utiliser (`Parsing parser; parser.parse(line);`).
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
