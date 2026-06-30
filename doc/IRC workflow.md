# Engineering Guidelines – ft_irc

## 1. Objectif

Notre objectif est de développer vite **sans casser le serveur**, avec un cadre commun que tout le monde respecte dès le premier jour.  
Le projet impose une vraie organisation d'équipe, des commits clairs, une répartition du travail visible, un serveur IRC complet en C++98, une gestion réseau non-bloquante avec un seul `poll` (ou équivalent), une authentification correcte, une exécution via `make`, et une documentation sérieuse dans le README.

---

## 2. Règles non négociables

- Aucun push direct sur `main`. Toute modification passe par une branche puis une Pull Request.
- `main` doit toujours être **compilable et fonctionnel** avec `-Wall -Wextra -Werror -std=c++98`, et dans un état défendable en soutenance.
- Toute PR doit être relue au minimum par une autre personne.
- Aucun merge si la CI est rouge.
- Aucun code ne peut contenir de warnings du compilateur.
- Aucun secret (mot de passe, IP, token) ne doit être commité.
- Toute décision technique importante (choix de structure de données, modèle de parsing, gestion des buffers) doit être écrite quelque part — pas gardée "dans la tête" ou seulement sur Discord.
- Forking interdit. Toutes les opérations I/O doivent être non-bloquantes. Un seul `poll` (ou équivalent) pour tout.

---

## 3. Organisation Git

### 3.1 Branches

Nous utilisons une stratégie simple, proche du trunk-based development, avec de petites branches de courte durée.

- `main` : branche protégée, toujours stable et compilable.
- `feat/<issue>-<nom-court>` : nouvelle fonctionnalité.
- `fix/<issue>-<nom-court>` : correction de bug.
- `refactor/<issue>-<nom-court>` : refactor sans changement fonctionnel.
- `docs/<issue>-<nom-court>` : documentation.
- `chore/<issue>-<nom-court>` : maintenance, CI, tooling, scripts.

**Exemples**
```bash
git checkout -b feat/12-join-command
git checkout -b fix/25-part-segfault
git checkout -b refactor/31-parser-cleanup
git checkout -b docs/40-readme-setup
```

### 3.2 Durée de vie d'une branche

- Une branche doit être petite et courte (idéalement < 2 jours).
- Une PR idéale doit être lisible en moins de 30 minutes.
- Si une branche dure plus de 2 ou 3 jours sans PR, elle doit être découpée.
- On rebase ou on merge `main` souvent pour éviter les gros conflits.

**Commandes utiles**
```bash
git fetch origin
git rebase origin/main
# ou
git merge origin/main
```

### 3.3 Protection de `main`

À activer dans GitHub :
- Require a pull request before merging.
- Require approvals (au moins 1).
- Require status checks to pass before merging.
- Require branches to be up to date before merging.
- Restrict direct pushes.
- Optionnel mais recommandé : require review from CODEOWNERS.

---

## 4. Convention de commits

Nous utilisons **Conventional Commits** pour avoir un historique lisible, homogène et exploitable.

### 4.1 Format obligatoire

```text
type(scope): description courte
```

**Exemples**
```text
feat(auth): implement PASS and USER registration flow
fix(parser): handle partial message split across recv calls
refactor(channel): simplify mode flag storage
docs(readme): document compilation and usage
test(server): add nc-based fragmentation test script
chore(ci): add build check on pull_request
```

### 4.2 Types autorisés

- `feat` : nouvelle fonctionnalité (nouvelle commande IRC, nouveau mode, etc.).
- `fix` : correction de bug.
- `docs` : documentation uniquement.
- `refactor` : refactor sans ajout fonctionnel.
- `test` : tests ou scripts de test.
- `chore` : maintenance, CI, Makefile, tooling.
- `ci` : pipeline CI/CD.
- `perf` : amélioration de performance.
- `style` : formatage/style sans impact fonctionnel.
- `build` : build, dépendances, flags de compilation.

### 4.3 Règles d'écriture

- La description doit être courte, précise, et commencer par un verbe d'action.
- Pas de commit du type `fix`, `update`, `wip`, `misc` tout seul.
- Un commit = une intention claire.
- Si le changement est important, on ajoute un body après une ligne vide.
- En cas de breaking change (ex. : changement de structure interne majeur), on l'indique explicitement.

**Exemple complet**
```text
feat(channel): implement JOIN with error replies

Handle ERR_NOSUCHCHANNEL, ERR_CHANNELISFULL, ERR_INVITEONLYCHAN,
ERR_BADCHANNELKEY. Update client membership list on success.

Refs: #12
```

### 4.4 Contrôle automatique des commits

Si le projet est assez grand pour le justifier :
- `commitlint` pour valider le format des messages de commit.
- `husky` pour exécuter les hooks Git côté développeur.
- Hook `commit-msg` pour bloquer les commits mal nommés.

---

## 5. Norme d'écriture du code

### 5.1 Principe global

Peu importe la partie du projet, nous voulons un code **lisible, prévisible, testable et cohérent**. Le sujet demande un code explicable et des choix techniques justifiables pendant l'évaluation.

### 5.2 Règles universelles

- Une fonction fait une chose.
- Les noms doivent être explicites (`handleJoinCommand`, `ClientManager`, `Channel::setMode`).
- On évite les variables d'une lettre hors boucles triviales.
- Pas de duplication inutile.
- Pas de logique métier IRC cachée dans du code réseau bas niveau.
- Pas de "magic numbers" sans constante nommée (ex. : `MAX_CLIENTS`, `IRC_BUFFER_SIZE`, `RPL_WELCOME`).
- Toute erreur de syscall (`recv`, `send`, `accept`, `poll`…) doit être gérée proprement.
- Toute entrée utilisateur (commandes IRC) doit être validée avant traitement.
- Le projet doit compiler sans crash, même en cas de mémoire insuffisante.

### 5.3 Règles spécifiques C++98

- Compiler avec `c++ -Wall -Wextra -Werror -std=c++98` en permanence.
- Préférer les headers C++ (`<cstring>` plutôt que `<string.h>`, etc.).
- Aucune bibliothèque externe ni Boost.
- Utiliser RAII pour encapsuler les ressources (sockets, fds, buffers).
- Aucune fuite mémoire tolérée. Les destructeurs doivent fermer les sockets.
- Pas de fonction de plus de 50 lignes sans justification.

### 5.4 Formatage

- Convenir d'un style commun dès le début (indentation, accolades, nommage) et s'y tenir.
- Si `clang-format` est disponible, créer un `.clang-format` commun.
- Aucun warning compiler dans `main`.

### 5.5 Standards minimaux

- Pas de warning linter/compiler dans `main`.
- Pas de TODO non tracé en issue.
- Pas de code mort mergé.
- Les commentaires expliquent le **pourquoi**, pas le quoi.
- Toute classe ou module complexe doit avoir une responsabilité claire.

### 5.6 Architecture

Séparer clairement les responsabilités :

- `Network` : création des sockets, `bind`, `listen`, `accept`, `poll` (ou équivalent).
- `Core` / `Server` : état global, liste des clients et des channels, dispatch des commandes.
- `Parser` : lecture ligne IRC, reconstruction depuis des lectures partielles, extraction commande/paramètres.
- `Commands` : handlers pour chaque commande IRC (`PASS`, `NICK`, `USER`, `JOIN`, `PART`, `PRIVMSG`, `NOTICE`, `QUIT`, `MODE`, `KICK`, `INVITE`, `TOPIC`, `PING`/`PONG`, etc.).
- `Auth & Modes` : gestion de l'état d'enregistrement, opérateurs vs utilisateurs réguliers, modes de channel (`i`, `t`, `k`, `o`, `l`).

Interdire les imports/includes circulaires. Définir la structure de dossiers dès le début.

**Exemple**
```text
src/
  network/     → socket, poll loop, client accept
  core/        → Server, ClientManager, ChannelManager
  parser/      → IRCParser, Message
  commands/    → JoinCommand, ModeCommand, KickCommand...
  auth/        → registration state machine
include/
  ...
```

Si une décision d'architecture a un impact durable, on crée une **ADR** (Architecture Decision Record) :

```text
docs/adr/0001-single-poll-event-loop.md
docs/adr/0002-per-client-recv-buffer.md
docs/adr/0003-command-dispatch-map.md
```

---

## 6. Pull Requests

### 6.1 Règle générale

Toute PR doit être **petite, claire, testable et reliée à une issue**. Les reviews servent à détecter bugs, incohérences de protocole IRC, dettes techniques et problèmes de compréhension.

### 6.2 Taille recommandée

- Idéal : 50 à 400 lignes.
- Au-delà, il faut découper si possible.
- Une PR énorme ralentit la review et augmente les erreurs.

### 6.3 Titre de PR

Le titre reprend la convention de commit :
```text
feat(parser): handle partial message reassembly
fix(mode): correct +k flag removal when no key provided
```

### 6.4 Description obligatoire de PR

```text
## Résumé
Que fait cette PR ?

## Pourquoi
Pourquoi ce changement est nécessaire ?

## Changements
- point 1
- point 2
- point 3

## Tests
- [ ] test manuel avec nc / client de référence
- [ ] test de cas limite (message fragmenté, déconnexion brutale)
- [ ] test avec mauvais paramètres (commande mal formée)

## Risques
Quels sont les impacts possibles sur le reste du serveur ?

## Issue liée
Closes #123
```

### 6.5 Checklist avant review

- Le code compile avec `-Wall -Wextra -Werror -std=c++98`.
- Aucun warning.
- Les tests manuels passent (connexion avec client de référence, scénario nc).
- La documentation a été mise à jour si nécessaire.
- La PR est reliée à une issue.

### 6.6 Merge policy

- **Squash merge** par défaut pour garder un historique propre.
- Interdiction de merger une PR avec des checks rouges.
- Interdiction d'auto-merge sans review.
- Rebase sur `main` avant merge si la branche est en retard.

---

## 7. Code Review

### 7.1 Ce qu'on vérifie en review

- Le code fait-il vraiment ce que dit l'issue ?
- Le comportement est-il conforme au sujet et au protocole IRC ?
- Les cas limites sont-ils gérés (message partiel, client qui se déconnecte en milieu de commande, commande malformée) ?
- Les erreurs système sont-elles gérées proprement ?
- Le naming est-il cohérent avec le reste du projet ?
- La sécurité de base est-elle respectée (validation des arguments, vérification opérateur, etc.) ?
- La solution est-elle trop complexe pour le besoin ?
- Les ressources (sockets, buffers) sont-elles bien libérées ?

### 7.2 Règles de review

- On critique le code, jamais la personne.
- Une review doit être précise et actionnable.
- Toute demande de changement doit expliquer **pourquoi**.
- Si le point est bloquant, on le marque clairement.
- Si c'est une suggestion, on le dit.

### 7.3 CODEOWNERS

Un fichier `CODEOWNERS` permet de définir qui doit être demandé en review sur quelles zones du repo.

**Exemple**
```text
*                   @team/core
/src/network/*      @team/network
/src/commands/*     @team/commands
/src/parser/*       @team/parser
/docs/*             @team/docs
```

---

## 8. Issues et gestion du travail

### 8.1 Une issue = un objectif clair

Chaque issue doit contenir :
- Contexte (quelle partie du protocole / du sujet).
- Objectif.
- Critères d'acceptation.
- Dépendances (autres commandes, structures nécessaires).
- Risques éventuels.
- Estimation.
- Responsable.

**Template**
```text
## Contexte
Pourquoi cette tâche existe ?

## Objectif
Quel résultat attendu ?

## Acceptation
- [ ] condition 1
- [ ] condition 2
- [ ] condition 3

## Dépendances
- issue liée
- structure nécessaire

## Notes techniques
éléments utiles (numéros de reply IRC, cas limites, etc.)
```

### 8.2 Labels minimum

- `feature`
- `bug`
- `tech-debt`
- `infra`
- `docs`
- `security`
- `blocked`
- `priority:high`
- `priority:medium`
- `priority:low`
- `good first issue`

### 8.3 Workflow d'une issue

- `Backlog` → `Ready` → `In progress` → `In review` → `Done`

### 8.4 Règles

- Pas de branche sans issue.
- Pas de PR sans issue liée.
- Pas d'issue vague du type "faire le parsing" ou "gérer les modes".
- Une issue trop grosse doit être découpée (ex. : une issue par commande IRC).

---

## 9. CI avec GitHub Actions

### 9.1 Checks obligatoires sur chaque PR

- Compilation avec `-Wall -Wextra -Werror -std=c++98` (échec si la moindre erreur ou warning).
- Vérification que le binaire `ircserv` est produit par `make`.
- Vérification que `make clean` puis `make` fonctionne (pas de relinking inutile).
- Optionnel : script de test automatisé minimal (connexion `nc`, scénario de base).

### 9.2 Exemple de workflow minimal

```yaml
name: ci

on:
  pull_request:
  push:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build
        run: make
      - name: Check no relinking
        run: make
      - name: Clean build
        run: make fclean && make
```

### 9.3 Jobs recommandés

- `build` : compilation complète avec les bons flags.
- `clean-build` : vérification `make fclean && make`.
- `lint` : si `clang-format` est configuré, vérifier le formatage.
- `nc-test` : script de test basique avec `nc -C` si disponible en CI.

### 9.4 Politique CI

- Une PR rouge ne se merge pas.
- Une PR verte n'est pas automatiquement bonne, elle doit encore être relue.
- La CI doit rester rapide ; si elle devient lente, on la découpe.

---

## 10. Build et environnement

Le sujet exige un `Makefile` respectant les règles 42 et une compilation C++98.

### 10.1 Règles Makefile

- Le `Makefile` doit contenir les règles : `$(NAME)`, `all`, `clean`, `fclean`, `re`.
- Pas de relinking inutile.
- Compilation avec `c++` et les flags `-Wall -Wextra -Werror -std=c++98`.
- Aucune bibliothèque externe ni Boost.

**Commandes standard**
```bash
make           # compile le projet
make clean     # supprime les fichiers objets
make fclean    # supprime les objets + le binaire
make re        # fclean + all
./ircserv <port> <password>
```

### 10.2 Environnement local

- Convenir d'un environnement commun (Linux, version de `g++` ou `clang++`).
- Documenter dans le README comment installer les éventuels outils de dev (valgrind, nc, client IRC de référence).
- Les variables sensibles (si un `.env` est utilisé pour des scripts de test) ne vont jamais dans Git.

### 10.3 Client de référence

- Choisir **un** client IRC de référence dès le début (ex. : HexChat, irssi, WeeChat) et le documenter.
- Tout le comportement serveur est validé contre ce client.
- Le client de référence sera utilisé lors de l'évaluation.

---

## 11. Comportement IRC et fonctionnalités

Pour chaque commande implémentée, documenter :
- Préconditions (client enregistré ? opérateur ? channel existant ?).
- Replies de succès attendus (numéros RFC).
- Replies d'erreur attendus.
- Changements d'état (membership, topic, modes, etc.).

### 11.1 Commandes obligatoires

| Commande | Scope | Notes |
|---|---|---|
| `PASS` | Auth | Doit précéder NICK/USER |
| `NICK` | Auth | Vérifier unicité et caractères valides |
| `USER` | Auth | Finalise l'enregistrement |
| `JOIN` | Channel | Créer si inexistant, gérer modes |
| `PART` | Channel | Broadcast + retirer du channel |
| `PRIVMSG` | Messaging | User-to-user et user-to-channel |
| `NOTICE` | Messaging | Comme PRIVMSG sans auto-reply |
| `QUIT` | Lifecycle | Broadcast + nettoyage |
| `PING`/`PONG` | Keepalive | Indispensable pour éviter timeout client |
| `KICK` | Opérateur | Avec vérification droits |
| `INVITE` | Opérateur | Avec mode `i` |
| `TOPIC` | Opérateur | Avec mode `t` |
| `MODE` | Opérateur | Flags `i`, `t`, `k`, `o`, `l` |

### 11.2 Modes de channel

| Mode | Description |
|---|---|
| `i` | Invite-only |
| `t` | TOPIC réservé aux opérateurs |
| `k` | Mot de passe de channel |
| `o` | Donner/retirer le statut d'opérateur |
| `l` | Limite d'utilisateurs |

---

## 12. Gestion du réseau et I/O

### 12.1 Event loop (poll ou équivalent)

- Un seul `poll` (ou `select`, `epoll`, `kqueue`) pour gérer tous les fds (socket d'écoute + clients).
- Tous les sockets en mode non-bloquant dès la création.
- Ne jamais appeler `recv`/`send` sur un fd non prêt selon `poll`.
- Gérer proprement les déconnexions (retour 0 de `recv`, erreur `ECONNRESET`, etc.) pour éviter les fds zombies.

### 12.2 Buffers par client

- Chaque client a son propre buffer d'entrée.
- Les messages IRC sont délimités par `\r\n` ; un `recv` peut apporter un fragment.
- On accumule dans le buffer et on ne traite que les lignes complètes.
- Ne jamais supposer qu'un `recv` retourne un message entier.

**Test de fragmentation (sujet)**
```bash
nc -C 127.0.0.1 6667
com^Dman^Dd
```

---

## 13. Tests

### 13.1 Ce qu'il faut couvrir en priorité

- Flux d'authentification complet (`PASS`/`NICK`/`USER` dans le bon ordre, puis dans le mauvais).
- Connexion avec mot de passe incorrect.
- Rejoindre un channel, envoyer un message, vérifier broadcast à tous les membres.
- Messages privés entre deux clients.
- Déconnexion brutale d'un client (kill du process nc) : le serveur ne doit pas crasher.
- Commandes d'opérateur (succès et refus si non-opérateur).
- Modes de channel (activer, désactiver, interaction entre modes).
- Messages fragmentés (multi-`recv` pour une seule commande).
- Charge : plusieurs clients simultanés, messages croisés.
- Commandes malformées ou inconnues : le serveur doit répondre avec le bon code d'erreur, pas crasher.

### 13.2 Politique

- Tout bug important corrigé doit avoir un test de non-régression (script `nc` ou client).
- Pas de merge d'une commande critique sans test de validation avec le client de référence.
- Les scripts de test doivent être reproductibles et documentés dans `docs/runbook.md`.
- Écrire des programmes de test même s'ils ne sont pas soumis ; ils servent en soutenance.

### 13.3 Outils

```bash
# Test basique
nc -C 127.0.0.1 6667

# Test avec valgrind (fuites mémoire)
valgrind --leak-check=full ./ircserv 6667 password

# Test avec client de référence
hexchat  # ou irssi, weechat
```

---

## 14. Sécurité minimale

### 14.1 Checklist minimale

- Validation stricte de toutes les entrées IRC (commandes, paramètres, noms de channels, nicks).
- Vérification systématique des permissions (opérateur requis ? client enregistré ?).
- Contrôle des limites (taille de message, nombre de clients, longueur de nick).
- Pas de secret commité (mots de passe de test, IPs, etc.).
- Pas de plantage en conditions anormales (mémoire pleine, connexion brutalement fermée).
- Logs sans fuite de mots de passe ou de données sensibles.

### 14.2 Ce qu'on interdit

- Confiance aveugle dans les données reçues du client.
- Actions opérateur sans vérifier que le client est bien opérateur.
- Accès à des ressources (channels, clients) sans vérifier leur existence préalable.
- Code qui crashe sur entrée malformée.

---

## 15. Documentation obligatoire

Le sujet demande un README complet avec description, instructions de compilation/exécution, ressources, et explication de l'usage de l'IA.

### 15.1 Documents à maintenir

- `README.md`
- `docs/engineering-guidelines.md`
- `docs/architecture.md`
- `docs/runbook.md`
- `docs/adr/*.md`
- `CHANGELOG.md`

### 15.2 Ce que le README doit contenir

- Première ligne italique :  
  *This project has been created as part of the 42 curriculum by \<login1>[, \<login2>[, ...]]*
- Section **Description** : objectif et aperçu du projet.
- Section **Instructions** : compilation, installation, exécution (`make`, `./ircserv <port> <password>`).
- Section **Resources** : références IRC (RFC 1459, RFC 2812, documentation des clients, tutoriels réseau), et description de l'usage de l'IA (pour quelles tâches, quelles parties du projet).

### 15.3 Ce qu'on documente dans les autres fichiers

- Comment compiler et lancer le serveur.
- Comment lancer les tests manuels.
- Comment connecter le client de référence.
- Les décisions d'architecture importantes (ADR).
- Procédure si le serveur crashe en test.
- Qui est responsable de quelle partie du code.

### 15.4 Runbook minimal

Le `runbook.md` doit contenir :
- Démarrage local.
- Commandes `make` utiles.
- Procédure de connexion avec nc et avec le client de référence.
- Procédure si un bug réseau apparaît en soutenance.
- Procédure pour recréer un environnement propre.
- Scripts de test reproductibles.

---

## 16. Réunions et pilotage

### 16.1 Rituels (adaptés à la taille du projet)

- **Daily async** : chacun poste ce qu'il a fait, ce qu'il fait, ce qui bloque.
- **Planning hebdo** : choix des priorités (quelle(s) commande(s) cette semaine ?).
- **Review technique** : sujets d'architecture et blocages réseau/protocole.
- **Demo interne** : montrer quelque chose qui marche avec le client de référence.

### 16.2 Format de daily

```text
Hier :
Aujourd'hui :
Blocages :
```

### 16.3 Règles

- Un blocage de plus de 24h doit être remonté immédiatement.
- Une grosse décision (ex. : comment gérer les modes combinés, structure de `ClientManager`) doit être écrite.
- Une fonctionnalité commencée doit avoir un responsable clair.

---

## 17. Definition of Done

Une tâche n'est **pas** "finie" quand le code existe. Elle est finie quand elle est intégrée proprement.

Une issue est **Done** seulement si :

- Le code compile sans warning avec `-Wall -Wextra -Werror -std=c++98`.
- Le comportement est conforme au sujet et testé avec le client de référence.
- Les cas limites sont couverts (entrée malformée, déconnexion, mauvais droits).
- La PR est reviewée et mergée.
- La CI est verte.
- La doc utile est à jour.
- La fonctionnalité est démontrable en soutenance et **explicable** par celui qui l'a écrite.

---

## 18. Commandes standard du projet

```bash
# Build
make

# Relancer proprement
make re

# Lancer le serveur
./ircserv <port> <password>

# Lancer le serveur en debug (exemple avec valgrind)
valgrind --leak-check=full --track-origins=yes ./ircserv 6667 password

# Test basique
nc -C 127.0.0.1 6667

# Nettoyage
make clean    # objets seulement
make fclean   # objets + binaire
```

---

## 19. Fichiers à mettre en place dès le début

```text
.github/
  workflows/
    ci.yml
  ISSUE_TEMPLATE/
    bug_report.md
    feature_request.md
  pull_request_template.md
CODEOWNERS
.clang-format          (si clang-format utilisé)
.editorconfig
.gitignore
README.md
Makefile
src/
include/
docs/
  engineering-guidelines.md
  architecture.md
  runbook.md
  adr/
    0001-single-poll-event-loop.md
    0002-per-client-recv-buffer.md
CHANGELOG.md
```

---

## 20. Résumé exécutable

Dès la première session, on met en place :
- Protection de `main` (PR obligatoire, review obligatoire, CI verte).
- Conventional Commits (+ commitlint/husky si jugé utile).
- Templates de PR et d'issues.
- CI GitHub Actions : build + check flags + éventuellement nc-test.
- `Makefile` conforme au sujet (`NAME`, `all`, `clean`, `fclean`, `re`, flags corrects).
- Structure de dossiers et découpage modulaire convenus.
- CODEOWNERS + review obligatoire.
- README + docs d'architecture + runbook.
- Definition of Done commune.
- Choix du client IRC de référence documenté.

---

## 21. Politique d'équipe

Nous préférons :
- la clarté à la "technique impressionnante".
- une PR petite à une grosse branche bloquée.
- un code simple à une architecture brillante mais fragile.
- une décision documentée à une convention implicite.
- une commande IRC terminée et démontrable à trois commandes à moitié finies.

Seules les fonctionnalités **pleinement fonctionnelles** comptent à l'évaluation. La partie bonus n'est évaluée que si la partie obligatoire est parfaite et sans dysfonctionnement.

Si vous appliquez vraiment tout ce document, vous aurez un niveau d'organisation et de stabilité **très au-dessus** de la moyenne pour un projet étudiant, tout en restant alignés avec les attentes du sujet.