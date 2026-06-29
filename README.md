# ft_irc

# Fiche récapitulative – Projet ft_irc (42)

# 1. Bibliothèques utilisées

## Bibliothèques C++

```cpp
#include <iostream>     // Affichage console (cout, cerr)
#include <string>       // Manipulation des chaînes de caractères
#include <vector>       // Tableau dynamique
#include <map>          // Association clé/valeur
#include <set>          // Ensemble de valeurs uniques
#include <sstream>      // Découpage des commandes IRC
#include <algorithm>    // find(), remove(), etc.
#include <cstdlib>      // atoi(), EXIT_SUCCESS...
#include <cstring>      // memset(), strerror()
#include <ctime>        // time()
```

### À quoi servent-elles ?

* **iostream** : afficher des messages de debug ou des erreurs.
* **string** : toutes les commandes IRC sont des chaînes de caractères.
* **vector** : stocker les clients, les channels, les `pollfd`.
* **map** : associer une clé (par exemple le socket d'un client) à un objet Client.
* **set** : éviter les doublons (opérateurs, membres...).
* **sstream** : parser les commandes reçues (`JOIN`, `NICK`, `PRIVMSG`...).
* **algorithm** : rechercher ou supprimer facilement dans les conteneurs.

---

# Bibliothèques réseau / système

```cpp
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
```

### Elles permettent de :

* créer des sockets
* écouter un port
* accepter des connexions
* envoyer/recevoir des données
* gérer plusieurs clients simultanément
* mettre les sockets en non bloquant
* gérer Ctrl+C proprement

---

# 2. Les structures importantes

## sockaddr_in

Structure représentant une adresse IPv4.

```cpp
sockaddr_in serverAddr;
```

Elle contient :

```cpp
serverAddr.sin_family;
serverAddr.sin_port;
serverAddr.sin_addr.s_addr;
```

Exemple :

```cpp
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(6667);
serverAddr.sin_addr.s_addr = INADDR_ANY;
```

---

## pollfd

Structure utilisée par `poll()`.

```cpp
pollfd pfd;
```

Elle contient :

```cpp
pfd.fd
```

Le socket.

```cpp
pfd.events
```

Les événements à surveiller.

```cpp
pfd.revents
```

Les événements réellement détectés.

---

# 3. Les fonctions réseau

## socket()

### Rôle

Créer un socket.

Syntaxe :

```cpp
int socket(
    int domain,
    int type,
    int protocol
);
```

Exemple :

```cpp
int serverFd = socket(AF_INET, SOCK_STREAM, 0);
```

Paramètres :

* AF_INET → IPv4
* SOCK_STREAM → TCP
* 0 → protocole automatique

Retour :

* > = 0 : succès
* -1 : erreur

---

## setsockopt()

### Rôle

Configurer un socket.

Très utilisé pour réutiliser un port.

```cpp
int opt = 1;

setsockopt(serverFd,
           SOL_SOCKET,
           SO_REUSEADDR,
           &opt,
           sizeof(opt));
```

Sans cette fonction :

```
Address already in use
```

après avoir relancé rapidement le serveur.

---

## bind()

### Rôle

Associer le socket au port.

```cpp
bind(serverFd,
     (sockaddr *)&serverAddr,
     sizeof(serverAddr));
```

Après le bind, le serveur écoute réellement sur le port choisi.

---

## listen()

### Rôle

Mettre le socket en attente de connexions.

```cpp
listen(serverFd, SOMAXCONN);
```

Le serveur est maintenant prêt à accepter des clients.

---

## accept()

### Rôle

Accepter une connexion.

```cpp
int clientFd = accept(
    serverFd,
    (sockaddr *)&clientAddr,
    &len
);
```

Important :

Le socket serveur reste ouvert.

`accept()` crée un **nouveau socket** uniquement pour le client.

Chaque client possède donc son propre file descriptor.

---

## recv()

### Rôle

Recevoir des données.

```cpp
char buffer[512];

recv(clientFd,
     buffer,
     sizeof(buffer),
     0);
```

Retour :

* > 0 → données reçues
* 0 → client déconnecté
* -1 → erreur

Exemple :

Le client envoie :

```
JOIN #42
```

Le serveur récupère cette chaîne avec `recv()`.

---

## send()

### Rôle

Envoyer des données.

```cpp
send(clientFd,
     message.c_str(),
     message.size(),
     0);
```

Exemple :

```
:server 001 user :Welcome
```

est envoyé au client.

---

## close()

### Rôle

Fermer un socket.

```cpp
close(clientFd);
```

À utiliser lorsqu'un client quitte le serveur.

---

# 4. poll()

## La fonction la plus importante du projet

Permet de surveiller plusieurs sockets simultanément.

Syntaxe :

```cpp
poll(pollfds.data(),
     pollfds.size(),
     timeout);
```

Exemple :

```cpp
poll(pollfds.data(),
     pollfds.size(),
     -1);
```

Le `-1` signifie :

Attendre indéfiniment.

---

## Pourquoi utiliser poll ?

Sans `poll()` :

Le serveur ne pourrait gérer qu'un seul client à la fois.

Avec `poll()` :

Le serveur surveille :

* le socket serveur
* tous les sockets clients

Dès qu'un événement arrive :

```
nouvelle connexion

ou

message reçu
```

`poll()` le signale immédiatement.

---

## Les événements

### POLLIN

Des données sont disponibles.

Très utilisé.

---

### POLLOUT

Le socket peut envoyer.

---

### POLLERR

Erreur sur le socket.

---

### POLLHUP

Client déconnecté.

---

# 5. fcntl()

### Rôle

Mettre un socket en mode non bloquant.

```cpp
fcntl(serverFd,
       F_SETFL,
       O_NONBLOCK);
```

Pourquoi ?

Sinon :

```
accept()

recv()

send()
```

peuvent bloquer complètement le serveur.

---

# 6. htons()

Host To Network Short.

Convertit un entier 16 bits (port).

```cpp
serverAddr.sin_port = htons(6667);
```

Le réseau utilise le format Big Endian.

---

# 7. htonl()

Host To Network Long.

Convertit un entier 32 bits.

```cpp
serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
```

---

# 8. memset()

Initialiser une structure.

```cpp
memset(&serverAddr,
       0,
       sizeof(serverAddr));
```

Très utilisé avant de remplir un `sockaddr_in`.

---

# 9. signal()

Intercepter Ctrl+C.

```cpp
signal(SIGINT, handler);
```

Exemple :

```cpp
void handler(int)
{
    running = false;
}
```

Permet de fermer proprement le serveur.

---

# 10. atoi()

Convertir une chaîne en entier.

```cpp
int port = atoi(argv[1]);
```

Si :

```
./ircserv 6667 pass
```

Alors :

```
argv[1]

↓

6667

↓

int
```

---

# 11. Fonctions de std::string

## size()

Retourne la longueur.

```cpp
msg.size();
```

---

## substr()

Extraire une partie.

```cpp
msg.substr(5);
```

---

## find()

Chercher une sous-chaîne.

```cpp
msg.find("JOIN");
```

---

## erase()

Supprimer une partie.

```cpp
msg.erase(pos);
```

---

## append()

Ajouter du texte.

```cpp
msg.append("\r\n");
```

---

## c_str()

Transformer une string en `const char *`.

Obligatoire avec `send()`.

```cpp
send(fd,
     msg.c_str(),
     msg.size(),
     0);
```

---

# 12. std::stringstream

Très utilisé pour parser les commandes IRC.

Exemple :

```
PRIVMSG #42 :Bonjour
```

Code :

```cpp
std::stringstream ss(line);

std::string cmd;

ss >> cmd;
```

Résultat :

```
cmd = "PRIVMSG"
```

Puis :

```
target

↓

message
```

sont récupérés progressivement.

---

# 13. std::vector

Tableau dynamique.

Exemple :

```cpp
std::vector<pollfd> pollfds;
```

Fonctions utiles :

```cpp
push_back()

erase()

size()

clear()

begin()

end()
```

---

# 14. std::map

Association clé → valeur.

Exemple :

```cpp
std::map<int, Client> clients;
```

Clé :

```
socket
```

Valeur :

```
Client
```

Permet de retrouver rapidement un client.

---

# 15. std::set

Collection sans doublons.

Exemple :

```cpp
std::set<std::string> operators;
```

Très utile pour :

* opérateurs
* membres
* invitations

---

# 16. std::algorithm

## find()

Chercher un élément.

```cpp
std::find(vec.begin(),
          vec.end(),
          value);
```

---

## remove()

Préparer la suppression.

Souvent combiné avec :

```cpp
erase()
```

---

# 17. Déroulement complet du serveur

```
main()

↓

socket()

↓

setsockopt()

↓

fcntl()

↓

bind()

↓

listen()

↓

poll()

↓

Nouvelle connexion ?

↓

accept()

↓

Ajout dans le vector de pollfd

↓

poll()

↓

recv()

↓

Parser la commande

↓

PASS

↓

NICK

↓

USER

↓

JOIN

↓

PART

↓

PRIVMSG

↓

MODE

↓

KICK

↓

TOPIC

↓

INVITE

↓

send()

↓

close()
```

---

# 18. Structures principales du projet

## Server

Responsabilités :

* créer le socket
* lancer le serveur
* gérer poll()
* accepter les connexions
* supprimer les clients
* envoyer les réponses

---

## Client

Contient généralement :

```cpp
fd

nickname

username

hostname

buffer

authenticated

registered
```

---

## Channel

Contient :

```cpp
nom

topic

liste des membres

liste des opérateurs

liste des invités

clé

limite d'utilisateurs
```

---

# 19. Commandes IRC à implémenter

```
PASS
```

Authentification.

```
NICK
```

Changer de pseudo.

```
USER
```

Déclarer le nom d'utilisateur.

```
JOIN
```

Rejoindre un salon.

```
PART
```

Quitter un salon.

```
PRIVMSG
```

Envoyer un message.

```
KICK
```

Expulser un utilisateur.

```
INVITE
```

Inviter un utilisateur.

```
TOPIC
```

Modifier le sujet d'un salon.

```
MODE
```

Modifier les modes du salon.

---

# 20. Les fonctions indispensables à connaître

| Fonction            | Utilité                                                                  |
| ------------------- | ------------------------------------------------------------------------ |
| `socket()`          | Créer un socket TCP.                                                     |
| `setsockopt()`      | Configurer le socket (`SO_REUSEADDR`).                                   |
| `bind()`            | Associer le socket à une adresse IP et un port.                          |
| `listen()`          | Mettre le serveur en attente de connexions.                              |
| `accept()`          | Créer un socket dédié à un nouveau client.                               |
| `poll()`            | Gérer plusieurs connexions simultanément sans bloquer.                   |
| `recv()`            | Recevoir les données envoyées par un client.                             |
| `send()`            | Envoyer des données à un client.                                         |
| `close()`           | Fermer un socket proprement.                                             |
| `fcntl()`           | Activer le mode non bloquant sur les sockets.                            |
| `htons()`           | Convertir un port au format réseau (16 bits).                            |
| `htonl()`           | Convertir une adresse au format réseau (32 bits).                        |
| `memset()`          | Initialiser une structure à zéro.                                        |
| `signal()`          | Intercepter un signal (ex. `SIGINT`) pour arrêter le serveur proprement. |
| `atoi()`            | Convertir une chaîne de caractères en entier.                            |
| `std::stringstream` | Découper et analyser les commandes IRC.                                  |
| `std::vector`       | Stocker des listes dynamiques (`pollfd`, clients, channels...).          |
| `std::map`          | Associer rapidement une clé à une valeur (ex. `fd → Client`).            |
| `std::set`          | Stocker des éléments uniques (opérateurs, invités...).                   |
| `std::string`       | Manipuler les commandes et les messages IRC.                             |
