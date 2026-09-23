# ft_irc — TODO bugs

Audit complet du 2026-09-17 (commit `38b0445`). Le projet **compile sans warning** avec
`-Wall -Wextra -Werror -std=c++98`. Tous les points marqués **[testé]** ont été reproduits
en lançant `./ircserv` et en s'y connectant.

Les entrées sont classées en **3 catégories** :

1. **Parsing** — lecture et découpage des messages, syntaxe RFC, validation des noms,
   format des réponses numériques.
2. **Channel** — commandes et état des channels (`JOIN`/`PART`/`MODE`/`TOPIC`/`KICK`/`INVITE`),
   diffusion aux membres.
3. **Serveur** — sockets, `poll()`, cycle de vie des clients, mémoire, signaux,
   authentification/enregistrement, dispatch, build.

Chaque entrée garde sa priorité : **[P0]** = bloquant (crash / violation du sujet / fuite),
**[P1]** = fonctionnalité manquante ou incorrecte, **[P2]** = robustesse et nettoyage.

---

# 1. 🔤 Parsing

## P0

- [x] **[P0] Une ligne `:` ou `:   ` arrête le serveur.** [testé, rc=1] **fixé** (branche `fix/parsing-bugs`)
      `Parsing::parse` lève `std::runtime_error` (`Parsing.cpp:20`, `Parsing_Utils.cpp:64,67,70`),
      `receiveClient` ne l'attrape pas, l'exception remonte jusqu'à `main` qui sort en `EXIT_FAILURE`.
      → `try/catch` autour de `_parser.parse()` dans `receiveClient`, et répondre 421 au client.

## P1 — Protocole

- [x] **[P1] Les lignes terminées par `\n` seul ne sont jamais traitées.** [testé : aucune réponse] **fixé**
      `Client::extractLine` (`src/client/Client.cpp:153`) ne cherche que `"\r\n"`.
      **C'est exactement le cas `nc`**, que le sujet demande de tester.
      → accepter `\n` et traiter le `\r` éventuel comme optionnel.

- [x] **[P1] Listes de cibles non gérées : `JOIN #a,#b`.** [testé : crée littéralement un channel
      nommé `#a,#b`] Idem pour les clés `JOIN #a,#b key1,key2`. Même problème pour `PART #a,#b`,
      `PRIVMSG a,b :msg`, `KICK #c a,b`.
      **fixé** : `JOIN` (channels + clés), `PART`, `KICK`, `PRIVMSG`, `NOTICE`, `WHOIS`, `LIST`
      gèrent maintenant les listes séparées par virgule.

- [x] **[P1] Pas de limite de 512 octets par ligne.** [testé : un PRIVMSG de 2000 octets est relayé
      intégralement, 2033 octets reçus] (voir aussi le P0 « buffer d'entrée sans limite » côté serveur)
      **fixé** : buffer client borné à 512 octets (`Client::appendBuffer`) et rejet d'une ligne
      déjà complète mais trop longue (`Parsing::parse`, `MAX_LINE_LENGTH`).

- [x] **[P1] Pas de limite à 15 paramètres** (RFC 1459). **fixé** (`Parsing::parse`, `MAX_PARAMS`).

- [ ] **[P1] Nicks et channels sensibles à la casse.** [testé]
      `alice` et `BOB`/`bob` coexistent ; `#Test` et `#test` sont deux channels distincts et
      un message dans l'un n'atteint pas l'autre. L'IRC impose une comparaison insensible à la
      casse, avec le *scrambling* `[]\` ↔ `{}|`. → clé de map normalisée dans `_clientsByNick`
      et `_channels`, en gardant le nom d'affichage à part.

- [ ] **[P1] Validation de nick trop stricte.** [testé : `a[b]c` refusé]
      `src/commands/Nick.cpp:20-26` n'autorise que `alnum`, `-` et `_` après le 1er caractère,
      alors que `[ ] \ ` ^ { } |` sont légaux — et sont d'ailleurs acceptés en position 0 par
      le même code. Incohérent.

## P1 — Format des codes numériques

- [ ] **[P1] `432` au lieu de `433` pour un nick déjà pris** (`Nick.cpp:50`).

- [ ] **[P1] Format des numériques cassé dans NICK et PASS.** [testé]
      Reçu : `:ircserv 432 :alice:Nickname is already in use`
      Attendu : `:ircserv 433 * alice :Nickname is already in use`
      Il manque le paramètre cible et le `:` est mal placé. Idem `464` dans `Pass.cpp` :
      `:ircserv 464 :Password incorrect` → `:ircserv 464 * :Password incorrect`.

- [ ] **[P1] Numériques avec nick vide.** [testé] Un client non enregistré qui fait `MODE #x` reçoit
      `:ircserv 403  #x :No such channel` — double espace, champ cible vide. → utiliser `*`
      tant que le nick n'est pas défini (comme le fait déjà `Cap.cpp:9`).
      **partiellement fixé** : `WHO`, `WHOIS`, `LIST`, `PRIVMSG` utilisent maintenant `*` en
      fallback. `MODE` (et les autres commandes citées) restent à faire.

## P2 — Nettoyage

- [ ] **[P2] `isalpha`/`isalnum`/`isdigit` avec un `char` signé** — `Nick.cpp:9,22` et `Utils.cpp:13` :
      UB pour les octets > 127. Il faut caster en `unsigned char`, comme c'est correctement fait
      dans `Mode.cpp:121` et `Parsing_Utils.cpp:32`.

- [x] **[P2] Mode debug `--parse-test` dans `main.cpp:27-140`** — ~115 lignes de tests en dur à supprimer.
      **fixé** : bloc supprimé.

---

# 2. 📺 Channel

## P1 — Commandes manquantes ou non branchées

- [ ] **[P1] `PART` n'est pas branché.** [testé : aucune réponse]
      `handlePart` existe (`src/commands/Part.cpp`), est compilé, mais n'est **ni déclaré dans
      `include/Commands.hpp` ni appelé dans `Server::dispatchCommand`**
      (`src/server/ServerCommands.cpp:9-44`). C'est du code mort.

- [ ] **[P1] `JOIN 0`** (quitter tous les channels) non géré.

- [ ] **[P1] `NAMES` absent** — optionnel, mais irssi/HexChat l'envoie au login.

- [ ] **[P1] Pas de validation des noms de channel.** `JOIN` n'exige qu'un `#` initial et laisse
      passer espaces, virgules, `:` et caractères de contrôle. `MODE` accepte `#` **et** `&`,
      `JOIN` non → incohérent entre les deux.

## P1 — Codes numériques

- [ ] **[P1] `461` utilisé comme fourre-tout dans MODE.** [testé]
      `+l abc`, `+l 0`, `+l 99999999999` renvoient tous `461 Not enough parameters` alors que
      des paramètres sont bien présents. `+k` sur un channel déjà `+k` devrait renvoyer `467`.

## P2 — Robustesse et nettoyage

- [ ] **[P2] `Channel::broadcast` appelle `::send()` en dur** (`Channel.cpp:200`) au lieu de passer
      par le serveur — le commentaire ligne 190 le note déjà. Voir aussi le P0 serveur sur la
      valeur de retour de `send()`.

- [ ] **[P2] `reply()` déclaré dans `Channel.hpp:90` mais défini dans `Join.cpp:10`** — placement
      incohérent (voir aussi le P0 serveur sur `Server serv;`).

- [ ] **[P2] `ApplyMode` n'est pas `static`** (`Mode.cpp:97`) — pollue l'espace de noms global.

- [ ] **[P2] Indentation cassée dans `Topic.cpp:19-26`** (le `else` n'est pas aligné avec son `if`).

---

# 3. 🖥️ Serveur

## P0 — Le serveur meurt (à corriger en premier)

- [ ] **[P0] `PASS` sans paramètre → SIGABRT.** [testé, rc=-6]
      `src/commands/Pass.cpp:25` lit `params[0]` sans jamais tester `params.empty()`.
      Un client qui envoie `PASS\r\n` tue le serveur. → ajouter le garde `461 ERR_NEEDMOREPARAMS`.

- [ ] **[P0] Déconnexion d'un client seul dans ≥ 2 channels → SIGSEGV.** [testé, rc=-11]
      `src/server/ServerClients.cpp:50-61` : `removeClientFromChannels()` itère sur `_channels`
      et appelle `removeChannel()` qui fait `_channels.erase(it)` → itérateur invalidé, puis `++it`.
      Se déclenche aussi bien sur `QUIT` que sur une fermeture brutale de socket.
      → collecter les noms à supprimer dans un `std::vector<std::string>`, supprimer après la boucle
      (ou `_channels.erase(it++)`).

- [ ] **[P0] `QUIT` suivi d'une autre commande dans le même paquet TCP → use-after-free.** [testé, rc=-6]
      `handleQuit` (`src/commands/Quit.cpp:22`) appelle `disconnectClient()` qui fait `delete client`,
      puis `src/server/ServerNetwork.cpp:213` reboucle sur `client->extractLine(line)`.
      → faire remonter un booléen « client détruit » depuis `dispatchCommand` et sortir de la boucle,
      ou différer la destruction en fin de tour de `processPollEvents`.

- [ ] **[P0] `disconnectAllClients()` invalide son propre itérateur.**
      `src/server/ServerClients.cpp:37-42` : `disconnectClient()` fait `_clientsByFd.erase()`.
      Même bug que le précédent, sur le chemin `shutdown()`.
      → `while (!_clientsByFd.empty()) disconnectClient(_clientsByFd.begin()->second);`

## P0 — Violations du sujet

- [ ] **[P0] Les sockets sont bloquantes.** [testé : `/proc/<pid>/fdinfo` → `O_NONBLOCK=False` sur le fd
      d'écoute **et** sur les fds clients]
      Aucun `fcntl(fd, F_SETFL, O_NONBLOCK)` nulle part. Le sujet l'exige explicitement.
      → à poser dans `createSocket()` et dans `acceptClient()` juste après `accept()`.

- [ ] **[P0] Un seul client qui ne lit pas fige le serveur entier.** [testé : après ~9 400 messages
      diffusés vers un client passif, plus aucune nouvelle connexion n'est servie]
      Conséquence directe du point précédent + `send()` appelé en direct.
      « Your server must never block » — c'est le point le plus sanctionné en soutenance.
      → buffer de sortie par client + `POLLOUT` dans `_pollfds` quand ce buffer est non vide.

- [ ] **[P0] Valeur de retour de `send()` jamais vérifiée → envois partiels perdus.**
      `src/server/ServerCommunication.cpp:9` et `src/channel/Channel.cpp:200`.
      → même correctif que ci-dessus (file d'attente d'écriture).

- [ ] **[P0] Le mot de passe est contournable.** [testé] Sans aucun `PASS`, ou avec un `PASS` **faux**,
      on peut faire `JOIN`, `MODE`, `TOPIC`, `KICK`, `INVITE` :
      ```
      NICK alice / USER a a a :A / JOIN #secret   →  :alice!a@a JOIN #secret
      MODE #secret +i                             →  :alice!a@a MODE #secret +i
      ```
      Ces handlers testent `isRegistered()` (nick+user+host non vides) mais jamais `isAuthenticated()`.

- [ ] **[P0] `SIGPIPE` non ignoré.** Pas de `signal(SIGPIPE, SIG_IGN)` ni de `MSG_NOSIGNAL` dans les
      `send()`. Écrire vers un socket que le pair vient de fermer tue le process.

- [ ] **[P0] Pas de handler `SIGINT`.** [testé, rc=-2] Ctrl+C tue le serveur sans fermer les fds
      ni libérer quoi que ce soit. → handler qui met `_running = false`, `poll()` sort sur `EINTR`.

## P0 — Fuites mémoire / ressources

- [ ] **[P0] `~Server()` est vide** (`src/server/Server.cpp:44`). Aucun `delete` sur les `Client *` de
      `_clientsByFd` ni sur les `Channel *` de `_channels`. Tout fuit à l'arrêt.

- [ ] **[P0] `_listenFd` n'est jamais fermé.** `Server::shutdown()` (`Server.cpp:87-91`) met
      `_listenFd = -1` sans `close()`.

- [ ] **[P0] Buffer d'entrée sans limite.** [testé : 5 Mo envoyés sans un seul `\r\n`, tout est accumulé]
      `Client::appendBuffer` grossit indéfiniment → un client peut faire exploser la RAM.
      → couper à 512 octets (RFC 1459) et déconnecter au-delà.

- [ ] **[P0] `reply()` construit un `Server` complet à chaque ligne envoyée.**
      `src/commands/Join.cpp:10-15` :
      ```cpp
      void reply(Client *client, const std::string &line) {
          Server serv;                      // ← objet Server jeté à chaque appel
          serv.sendMessage(client, line + "\r\n");
      }
      ```
      Ça ne « marche » que parce que `sendMessage` n'utilise que `client->getFd()` et que
      `~Server()` est vide. **Dès que le destructeur libérera quelque chose (point ci-dessus),
      ça devient un double-free généralisé.** → passer par `Server&` (déjà disponible dans tous
      les `handleXxx`) ou en faire une méthode de `Client`.

## P1 — Dispatch et commandes manquantes

- [ ] **[P1] `PONG` non géré.** `Parsing::isKnownCommand` le reconnaît mais le dispatch tombe dans
      « Unknown command ». Les clients qui répondent à un PING serveur seront ignorés.

- [ ] **[P1] Pas de `421 ERR_UNKNOWNCOMMAND`.** [testé : aucune réponse]
      `ServerCommands.cpp:43` fait juste un `std::cout` côté serveur.

- [ ] **[P1] Pas de PING keepalive serveur → client** (TODO déjà noté dans `Ping.cpp:14`).
      Les connexions mortes restent indéfiniment dans `_pollfds`.

- [ ] **[P1] `MOTD` absent** — optionnel, mais irssi/HexChat l'envoie au login.

## P1 — Diffusion des changements d'état

- [ ] **[P1] `NICK` ne diffuse pas le changement.** [testé : alice n'est pas notifiée quand bob
      devient bobby] Il manque le `:oldnick!user@host NICK :newnick` envoyé à tous les channels
      communs (+ au client lui-même). Les clients graphiques affichent un nick périmé.

- [ ] **[P1] Aucune notification `QUIT` sur déconnexion brutale.**
      `disconnectClient()` → `removeClientFromChannels()` retire silencieusement. Seul `handleQuit`
      diffuse. Si le socket tombe, les autres membres ne voient jamais partir le client.

## P1 — Enregistrement et authentification

- [ ] **[P1] `PASS` après enregistrement → `464`, devrait être `462 ERR_ALREADYREGISTRED`.** [testé]

- [ ] **[P1] `USER` envoyé une 2ᵉ fois est accepté et renvoie un second welcome.** [testé]
      Devrait être `462`. Permet d'écraser username/hostname après coup.

- [ ] **[P1] `NICK` vers son propre nick actuel → `432 already in use`.** [testé]
      Devrait être un no-op silencieux.

- [ ] **[P1] `handlePass` valide le mot de passe *du serveur*, pas celui du client.**
      `src/commands/Pass.cpp:19` : `isValidPassword(server.getPassword())`. Si le mot de passe
      du serveur contient un espace ou dépasse 255 caractères, **tous** les clients sont
      rejetés en 464. La validation devrait porter sur `params[0]`.

- [ ] **[P1] `004` incomplet.** `:ircserv 004 nick ircserv 1.0 io` n'annonce pas les modes de channel
      supportés (`itkol`). Pas de `005 ISUPPORT` non plus (`ServerCommunication.cpp:28` le note déjà).

## P1 — Contrôle d'accès incohérent

- [ ] **[P1] Trois politiques différentes coexistent selon la commande :**

  | Vérification | Commandes |
  |---|---|
  | `isRegistered()` seul | `JOIN`, `PART` |
  | `isAuthenticated()` seul | `PRIVMSG`, `NOTICE`, `WHO`, `WHOIS`, `LIST` |
  | **aucune** | `MODE`, `TOPIC`, `KICK`, `INVITE` |

  → une seule règle : tout sauf `PASS`/`NICK`/`USER`/`CAP`/`QUIT` doit exiger
  `isAuthenticated() && isRegistered()`, idéalement via un garde unique dans `dispatchCommand`.

- [ ] **[P1] `isRegistered()` dépend de `_hostname`** (`src/client/Client.cpp:95`), qui est rempli
      avec le 2ᵉ paramètre de `USER` — donc une valeur fournie par le client, jamais vérifiée.
      Le hostname devrait venir de `_address` (`inet_ntoa`), qui n'est **jamais exploité**.

- [ ] **[P1] `Client::_password` / `setPassword()` ne sont jamais appelés.** Champ mort.

## P2 — Robustesse

- [ ] **[P2] `processPollEvents` ignore `POLLERR`, `POLLHUP` et `POLLNVAL`.**
      `src/server/ServerNetwork.cpp:126-150` — la version qui les gérait est commentée juste
      au-dessus (lignes 64-124). Un fd en erreur garde son `revents` positionné, `poll()` rend
      la main immédiatement, la boucle ne fait rien → **100 % CPU indéfiniment**.

- [ ] **[P2] `recv() < 0` ne déconnecte pas le client.** `ServerNetwork.cpp:192-196` se contente
      d'un `return`. Combiné au point précédent → boucle infinie sur un fd cassé.

- [ ] **[P2] `Server::run()` appelle `processPollEvents()` même après une erreur de `poll()`**
      (`Server.cpp:74-81`), donc sur des `revents` périmés. Il manque un `continue`/`break`.

- [ ] **[P2] `Server::sendMessage` ne teste pas `client != NULL`** (`ServerCommunication.cpp:9`).

- [ ] **[P2] Constructeur de copie et `operator=` de `Server`** copient des pointeurs bruts
      (`_clientsByFd`) → double-free potentiel — **et oublient `_channels`** (`Server.cpp:20-42`).
      → les rendre privés et non définis.

## P2 — Nettoyage avant soutenance

- [ ] **[P2] `Tmp_Log.hpp` / `Tmp_Log.cpp`** sont marqués temporaires et encore utilisés
      (`ServerClients.cpp:32`). À intégrer proprement ou retirer.

- [ ] **[P2] `//TODEL:` dans `include/Server.hpp:17-19`** (includes `Commands.hpp` / `Channel.hpp`)
      et `include/Commands.hpp:8`.

- [ ] **[P2] Includes circulaires `Server.hpp` ↔ `Client.hpp`** avec `#pragma once` : ça ne compile
      aujourd'hui que grâce à l'ordre d'inclusion. → forward-declare `class Client;` dans
      `Server.hpp` et retirer `#include "Server.hpp"` de `Client.hpp`.

- [ ] **[P2] `isValidPort` refuse les ports < 1024** (`Utils.cpp:28`) — restriction non demandée par
      le sujet.

- [ ] **[P2] Makefile : les couleurs ne s'affichent pas.** `make` utilise `/bin/sh`, dont le `echo`
      n'interprète pas `\033` ni `\n` → la sortie de compilation affiche littéralement
      `\033[0;35m...`. → `printf` au lieu de `echo`, ou `SHELL := /bin/bash`.

- [ ] **[P2] Makefile : `INCLUDES` / `INC` sont calculés mais jamais utilisés** comme prérequis
      (les `.d` de `-MMD` font déjà le travail). `Parsing.hpp` n'y figure d'ailleurs pas.
      `.PHONY` oublie `clean-all` et `fclean-all`.

- [ ] **[P2] Headers 42 absents** sur tous les fichiers sauf `Channel.hpp` et `Channel.cpp`.

---

## ✅ Ce qui fonctionne (vérifié)

Pour mémoire, ces parties se comportent correctement en test :

- Séquence de connexion complète type irssi (`CAP LS` → `PASS` → `NICK` → `USER` → `CAP END`)
  avec un seul welcome `001`-`004`.
- Modes `+i`, `+t`, `+k`, `+l`, `+o` : application, refus (`471`/`473`/`475`/`482`), affichage
  `324`, retrait, et rejet des modes inconnus (`472`).
- `INVITE` qui débloque bien un `+i`.
- `KICK` avec raison contenant des espaces, diffusé à tout le channel, et le kické est bien
  retiré (`404` ensuite).
- `TOPIC` en lecture (`331`/`332`), en écriture, effacement, et protection `+t`.
- Parsing du *trailing* `:` avec espaces, du préfixe, de la casse des commandes.
- Suppression d'un channel quand il devient vide.
