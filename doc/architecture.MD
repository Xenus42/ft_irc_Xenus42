# Architecture IRC Server – Principes de conception

## Principe n°1 : le Server ne doit pas tout faire

Beaucoup de projets étudiants finissent avec un Server.cpp énorme contenant :

- réseau
- parsing
- gestion des channels
- gestion des messages
- gestion des modes
- réponses IRC

👉 Résultat : code difficile à maintenir, tester et faire évoluer.

Le rôle du Server doit être minimal :
gérer le réseau et déléguer toute la logique métier.

---

## Architecture générale

                main
                  │
                  ▼
             +-----------+
             |  Server   |
             +-----------+
                  │
        ┌─────────┴─────────┐
        │                   │
        ▼                   ▼
   ClientManager      ChannelManager
        │                   │
        ▼                   ▼
      Client            Channel
        │
        ▼
 CommandDispatcher
        │
        ▼
 IRCCommand (interface)
        │
 ┌──────┼─────────────┐
 ▼      ▼             ▼
Join   Nick       Privmsg

---

## Classe Server

class Server
{
private:
    int socketFd;
    std::vector<pollfd> pollfds;
    ClientManager clients;
    ChannelManager channels;

public:
    void run();

private:
    void acceptClient();
    void receiveData();
    void disconnectClient();
};

---

## Classe Client

class Client
{
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string buffer;
    bool authenticated;

public:
    void appendBuffer();
    std::string getNickname();
    void sendMessage();
};

---

## Classe Channel

class Channel
{
private:
    std::string name;
    std::string topic;
    std::set<Client*> members;
    std::set<Client*> operators;
    std::string password;
    bool inviteOnly;
    int limit;

public:
    void addClient();
    void removeClient();
    void broadcast();
};

---

## ClientManager

class ClientManager
{
private:
    std::map<int, Client*> clients;

public:
    Client* getClient(int fd);
    Client* getByNickname();
    void addClient();
    void removeClient();
};

---

## ChannelManager

class ChannelManager
{
private:
    std::map<std::string, Channel*> channels;

public:
    Channel* getChannel();
    Channel* createChannel();
    void deleteChannel();
};

---

## Parser

JOIN #42 -> ParsedCommand

class Parser
{
public:
    ParsedCommand parse(std::string line);
};

class ParsedCommand
{
public:
    std::string name;
    std::vector<std::string> params;
};

---

## Pattern Command

class ICommand
{
public:
    virtual void execute(Client&, ParsedCommand&) = 0;
};

class JoinCommand : public ICommand {};
class NickCommand : public ICommand {};
class QuitCommand : public ICommand {};
class InviteCommand : public ICommand {};
class ModeCommand : public ICommand {};

---

## Dispatcher

std::map<std::string, ICommand*> commands;

commands["JOIN"]->execute();

---

## Replies

class Replies
{
public:
    static std::string welcome();
    static std::string needMoreParams();
    static std::string nicknameInUse();
    static std::string unknownCommand();
};

---

## Utils

- split
- trim
- upper
- lower
- tokenize
- isValidNickname
- isChannelName

---

## Dépendances

Server → Managers → Client/Channel → Dispatcher → Commands

---

## Conclusion

- Server = réseau uniquement
- Client = état utilisateur
- Channel = état salon
- Parser = texte → commande
- Dispatcher = routing
- ICommand = logique IRC
- Replies = réponses centralisées
