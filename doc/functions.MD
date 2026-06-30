socket()	Crée un descripteur de socket.	domain : famille d'adresses (AF_INET, AF_INET6, ...)
type : type de socket (SOCK_STREAM, SOCK_DGRAM)
protocol : généralement 0	≥ 0 : descripteur de socket
-1 : erreur


bind()	Associe une adresse IP et un port à une socket (principalement côté serveur).	sockfd : socket
addr : adresse (struct sockaddr *)
addrlen : taille de la structure	0 : succès
-1 : erreur


listen()	Place une socket TCP en mode écoute pour accepter des connexions.	sockfd : socket liée avec bind()
backlog : nombre maximal de connexions en attente	0 : succès
-1 : erreur


connect()	Établit une connexion TCP vers un serveur (côté client).	sockfd : socket
addr : adresse du serveur
addrlen : taille de l'adresse	0 : succès
-1 : erreur


poll()	Attend qu'un ou plusieurs descripteurs deviennent prêts (lecture, écriture, etc.).	fds : tableau de struct pollfd
nfds : nombre d'éléments du tableau
timeout : temps d'attente (ms), -1 = infini	> 0 : nombre de descripteurs prêts
0 : timeout
-1 : erreur


send()	Envoie des données sur une socket connectée.	sockfd : socket
buf : données à envoyer
len : nombre d'octets
flags : généralement 0	Nombre d'octets envoyés
-1 : erreur


recv()	Reçoit des données depuis une socket connectée.	sockfd : socket
buf : tampon de réception
len : taille du tampon
flags : généralement 0