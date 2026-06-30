🧠 Méthodes std::map utiles pour ft_irc

Je te les classe par usage réel dans ton serveur IRC.

1. 🔍 Recherche (TRÈS important)
find()
it = map.find(key);

✔️ Sert à vérifier si un client / channel existe
✔️ Évite les effets de bord de operator[]

Exemple ft_irc :
std::map<int, Client>::iterator it = _clients.find(fd);

if (it != _clients.end())
{
    Client &client = it->second;
}

👉 C’est LA méthode la plus importante du projet.

count()
map.count(key);

✔️ Retourne 0 ou 1 (map = clés uniques)

Exemple :
if (_channels.count("#general"))
{
    // channel exists
}

👉 Pratique mais moins flexible que find().

2. ➕ Ajout d’éléments
insert()
map.insert(std::make_pair(key, value));

✔️ Ajoute seulement si la clé n’existe pas déjà

Exemple :
_clients.insert(std::make_pair(fd, Client(fd, addr)));
emplace() (si autorisé en C++98 → parfois non)
map.emplace(key, value);

✔️ Construit directement dans la map
✔️ Plus moderne mais pas toujours accepté en 42 selon sujet

operator[] ⚠️ (à utiliser avec précaution)
map[key] = value;

⚠️ Si la clé n’existe pas → elle est créée automatiquement

Danger ft_irc :
_clients["Bob"]; // crée un client vide !!!

👉 À éviter pour les checks d’existence

3. ❌ Suppression
erase()
map.erase(key);

ou :

map.erase(iterator);
Exemple ft_irc :
_clients.erase(fd);

ou :

if (it != _clients.end())
    _clients.erase(it);
4. 📏 Taille
size()
map.size();

✔️ Nombre de clients / channels

Exemple :
if (_clients.size() > MAX_CLIENTS)
empty()
map.empty();

✔️ Vérifie si vide

5. 🔁 Parcours
itérateurs (IMPORTANT)
for (std::map<int, Client>::iterator it = _clients.begin();
     it != _clients.end();
     ++it)
{
    it->first;   // key (fd)
    it->second;  // Client
}
Très utilisé pour :
broadcast messages
cleanup clients
listing channels
ping checks
6. 🧹 Nettoyage global
clear()
map.clear();

✔️ Supprime tout

Exemple :
_clients.clear();
_channels.clear();
7. 📌 Les membres que tu utilises vraiment dans ft_irc
🔥 Top 6 indispensables

Tu peux faire 95% du projet avec :

find()
insert()
erase()
begin() / end()
size()
clear()
8. 💡 Résumé mental simple

Dans ft_irc :

Action	map method
vérifier existence	find()
ajouter client	insert()
supprimer client	erase()
parcourir tous	begin()/end()
compter clients	size()
reset serveur	clear()
9. ⚠️ Erreur classique à éviter (très important)
_clients["Bob"]

❌ peut créer un client vide involontairement
❌ bug silencieux très fréquent en ft_irc

👉 Toujours préférer :

if (_clients.find("Bob") != _clients.end())