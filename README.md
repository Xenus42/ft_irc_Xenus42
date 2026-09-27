# ft_irc

Serveur IRC en C++98 compatible avec le cœur du sujet 42 : sockets TCP, clients multiples, canaux, parsing des commandes IRC et gestion des états utilisateurs.

Ce projet est développé selon les principes de base du protocole IRC et s’appuie notamment sur les références suivantes :
- RFC 1459 : Internet Relay Chat Protocol
- RFC 2812 : Internet Relay Chat: Client Protocol
- Sujet 42 `ft_irc`

Sources du projet consultées dans le code :
- `src/server/ServerNetwork.cpp` — sockets, `poll()`, accept/recv/send
- `src/server/ServerCommands.cpp` — dispatch des commandes IRC
- `src/parsing/Parsing.cpp` — parsing des lignes de commande
- `src/commands/Join.cpp` — gestion des canaux
- `src/channel/Channel.cpp` — logique des canaux et modes
- `src/client/Client.cpp` — état du client et buffers

## 1. Objectif du projet

Implémenter un serveur IRC minimal, multi-clients, non bloquant, capable de :
- accepter plusieurs connexions TCP en même temps,
- gérer l’enregistrement d’un client (`PASS`, `NICK`, `USER`),
- créer et rejoindre des canaux,
- envoyer des messages privés et publics,
- gérer les modes de canaux et la logique d’accès,
- répondre avec des codes IRC standard (ex. `401`, `403`, `431`, `433`, `461`, `462`, `464`, `475`, etc.).

## 2. Technologies et contraintes

- Langage : C++98
- Système : Linux
- Sockets : BSD sockets
- I/O multiplexing : `poll()`
- Non bloquant : `fcntl(..., O_NONBLOCK)`
- Sécurité / robustesse : gestion des erreurs sockets, fermeture propre, flux IRC limité

## 3. Compilation

Depuis la racine du projet :

```bash
make
```

Pour nettoyer le build :

```bash
make clean
make fclean
```

## 4. Lancement

```bash
./ircserv <port> <password>
```

Exemple :

```bash
./ircserv 6667 mysecret
```

## 5. Utilisation

Un client IRC peut se connecter au serveur puis envoyer les commandes suivantes :

```text
PASS mysecret
NICK alice
USER alice 0 * :Alice
JOIN #general
PRIVMSG #general :Bonjour !
PRIVMSG bob :Salut
WHOIS alice
LIST
MODE #general
TOPIC #general :Nouveau sujet
PART #general
QUIT :bye
```

Il est possible de tester avec un client IRC standard ou via un simple client TCP. La logique serveur est organisée autour des fichiers suivants :
- `include/Server.hpp`
- `include/Channel.hpp`
- `include/Client.hpp`
- `include/Parsing.hpp`
- `include/Commands.hpp`

## 6. Commandes IRC gérées

Le projet implémente les commandes suivantes dans le dispatch principal (`src/server/ServerCommands.cpp`) :

- `PASS`
- `NICK`
- `USER`
- `JOIN`
- `PART`
- `TOPIC`
- `MODE`
- `INVITE`
- `KICK`
- `PRIVMSG`
- `NOTICE`
- `WHO`
- `WHOIS`
- `LIST`
- `PING`
- `QUIT`
- `CAP`

Les handlers associés sont dans `src/commands/`.

## 7. Architecture du projet

```text
include/
├── Channel.hpp
├── Client.hpp
├── Commands.hpp
├── Parsing.hpp
├── Server.hpp
├── Utils.hpp

src/
├── main.cpp
├── channel/Channel.cpp
├── client/Client.cpp
├── commands/
│   ├── Invite.cpp
│   ├── Join.cpp
│   ├── Kick.cpp
│   ├── List.cpp
│   ├── Mode.cpp
│   ├── Nick.cpp
│   ├── Notice.cpp
│   ├── Part.cpp
│   ├── Pass.cpp
│   ├── Ping.cpp
│   ├── Privmsg.cpp
│   ├── Quit.cpp
│   ├── Topic.cpp
│   ├── User.cpp
│   ├── Who.cpp
│   └── Whois.cpp
├── parsing/Parsing.cpp
├── parsing/ParsingUtils.cpp
├── server/
│   ├── Server.cpp
│   ├── ServerChannels.cpp
│   ├── ServerClients.cpp
│   ├── ServerCommands.cpp
│   ├── ServerCommunication.cpp
│   ├── ServerNetwork.cpp
│   └── ...
├── utils/Utils.cpp
```

### Rôle des composants

- `Server` : boucle principale, sockets, gestion des clients, dispatch des commandes
- `Client` : état du client, buffers d’entrée/sortie, authentification, registration
- `Channel` : gestion des membres, opérateurs, modes, topic, invitations
- `Parsing` : extraction du préfixe, découpage de la commande et des paramètres
- `Utils` : validation de port, normalisation de chaînes, découpage CSV

## 8. Protocole et standards suivis

Le projet suit les bases de l’IRC tel que défini dans les RFC de référence :
- parsing des commandes IRC : préfixe, commande, paramètres,
- codes d’erreur IRC standard,
- canaux avec nom commençant par `#`,
- logique de registration `PASS` → `NICK` → `USER`,
- diffusion de messages au sein d’un canal ou vers un client.

Les règles de parsing et de validation sont implémentées dans :
- `src/parsing/Parsing.cpp`
- `src/parsing/ParsingUtils.cpp`
- `include/Parsing.hpp`

## 9. Exemples de flux serveur

### Enregistrement client

```text
CLIENT -> PASS mysecret
CLIENT -> NICK alice
CLIENT -> USER alice 0 * :Alice
SERVER -> :ircserv 001 alice :Welcome...
```

### Création / join d’un canal

```text
CLIENT -> JOIN #general
SERVER -> :alice!user@host JOIN #general
SERVER -> :ircserv 353 alice = #general :alice
SERVER -> :ircserv 366 alice #general :End of /NAMES list
```

### Message privé

```text
CLIENT -> PRIVMSG bob :bonjour
SERVER -> :alice!user@host PRIVMSG bob :bonjour
```

## 10. Sources et références

- RFC 1459 — Internet Relay Chat Protocol
- RFC 2812 — Internet Relay Chat: Client Protocol
- Sujet 42 `ft_irc`
- Code du projet dans `src/` et `include/`

## 11. Critères de validation du sujet

Le projet couvre le cœur attendu du sujet :
- serveur TCP multi-clients,
- sockets non bloquants,
- système de parsing IRC,
- commandes de base,
- canaux et messages,
- gestion d’état du client.

## 12. Notes de build

Le projet a été conçu pour fonctionner en environnement Linux standard avec `g++` / `c++` en mode C++98.

Commande de test rapide :

```bash
make
./ircserv 6667 password
```

Puis connexion depuis un client IRC ou un test TCP simple.

## 13. Licence / usage

Projet pédagogique développé dans le cadre du cursus 42. Le code est destiné à l’apprentissage et à la validation du sujet `ft_irc`.
