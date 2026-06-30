# Engineering Guidelines

## 1. Objectif

Notre objectif est de développer vite **sans casser l’application**, avec un cadre commun que tout le monde respecte dès le premier jour.  
Le projet impose déjà une vraie organisation d’équipe, des commits clairs, une répartition du travail visible, une application web complète, une base de données, une authentification correcte, une exécution via conteneurisation en une commande, et une documentation sérieuse dans le README.

## 2. Règles non négociables
- Aucun push direct sur `main`. Toute modification passe par une branche puis par une Pull Request.docs.github+1
- `main` doit toujours être **déployable** et dans un état propre.[docs.github](https://docs.github.com/en/actions/tutorials/create-an-example-workflow)
- Toute PR doit être relue au minimum par une autre personne. Le sujet recommande explicitement les code reviews pour les changements importants.
- Aucun merge si la CI est rouge.[docs.github](https://docs.github.com/en/actions/tutorials/create-an-example-workflow)
- Aucun code non formaté, non linté ou non testé ne doit être mergé.[docs.github](https://docs.github.com/en/actions/tutorials/create-an-example-workflow)
- Aucun secret ne doit être commité. Les variables sensibles doivent rester dans `.env`, et un `.env.example` doit être maintenu.
- Toute décision importante doit être écrite quelque part, pas gardée “dans la tête” ou seulement sur Discord. Le sujet insiste aussi sur la documentation des décisions et du fonctionnement.


---

## 3. Organisation Git

## 3.1 Branches

Nous utilisons une stratégie simple, proche de trunk-based development, avec petites branches de courte durée.

## 3.2 Durée de vie d’une branche
- Une branche doit être petite et courte.
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

## 3.3 Protection de `main`

À activer dans GitHub :
- Require a pull request before merging.
- Require approvals.
- Require status checks to pass before merging.
- Require branches to be up to date before merging.
- Restrict direct pushes.
- Optionnel mais recommandé : require review from code owners.github+1

---

## 4. Convention de commits

Nous utilisons **Conventional Commits** pour avoir un historique lisible, homogène et exploitable par des outils. La spécification définit un format léger et standardisé, avec un type, un scope optionnel, une description courte, puis éventuellement un body et des footers.
## 4.1 Format obligatoire

```text
type(scope): description courte
```

## 4.3 Règles d’écriture
- La description doit être courte, précise, et commencer par un verbe d’action.
- Pas de commit du type `fix`, `update`, `wip`, `misc`, `test` tout seul.
- Un commit = une intention claire.
- Si le changement est important, on ajoute un body après une ligne vide, comme le prévoit la spécification.
- En cas de breaking change, on l’indique explicitement.

## 4.4 Contrôle automatique des commits

À mettre en place :
- `commitlint` pour valider le format des messages de commit.dev+1
- `husky` pour exécuter les hooks Git côté développeur.[dev](https://dev.to/tene/mastering-conventional-commits-structure-benefits-and-tools-3cpo)
- Hook `commit-msg` pour bloquer les commits mal nommés.[dev](https://dev.to/tene/mastering-conventional-commits-structure-benefits-and-tools-3cpo)


**Exemple**

```bash
pnpm add -D @commitlint/cli @commitlint/config-conventional husky
npx husky init
```

**`commitlint.config.cjs`**
```js
module.exports = {
   extends: ['@commitlint/config-conventional'],
};
```

---

## 5. Norme d’écriture du code

## 5.1 Principe global

Peu importe le langage, nous voulons un code **lisible, prévisible, testable, et cohérent**. Le sujet demande un code explicable, des choix techniques justifiés, et une vraie capacité à défendre l’implémentation pendant l’évaluation.

## 5.2 Règles universelles
- Une fonction fait une chose.
- Les noms doivent être explicites.
- On évite les variables d’une lettre hors boucles triviales.
- Pas de duplication inutile.
- Pas de logique métier cachée dans des composants UI.
- Pas de “magic numbers” sans constante nommée.
- Toute erreur attendue doit être gérée proprement.
- Toute entrée utilisateur doit être validée côté front **et** côté back, ce que le sujet impose explicitement.

## 5.3 Formatage et linting
Chaque langage doit avoir **un formateur automatique** et **un linter**.

**TypeScript / JavaScript**
- Formatage : Prettier.
- Linting : ESLint.
- Typage : TypeScript strict.
- Option recommandée : `noImplicitAny`, `strict`, `noUncheckedIndexedAccess`.

**Rust**
- Formatage : `rustfmt`.
- Linting : `clippy`.
- Vérification : `cargo check`.

**Shell**
- Formatage : `shfmt`.
- Linting : `shellcheck`.

**Docker**
- Validation : `hadolint`.

**Commandes utiles**

bash
```bash
pnpm lint
pnpm format
cargo fmt --all cargo
clippy --all-targets --all-features -- -D warnings
cargo check
shellcheck scripts/*.sh
hadolint Dockerfile`
```

## 5.4 Standards minimaux
- Pas de warning linter dans `main`.
- Pas de TODO non tracé en issue.
- Pas de code mort mergé.
- Pas de fonction énorme sans justification.
- Pas de commentaire qui répète le code ; les commentaires servent à expliquer le **pourquoi**, pas le **quoi**.
- Toute API, service ou module complexe doit avoir une responsabilité claire.

## 5.5 Architecture
- Séparer clairement UI, logique métier, accès données, intégrations externes.
- Interdire les imports circulaires.
- Définir une structure de dossier commune dès le début.
- Introduire un vocabulaire de domaine partagé pour éviter 4 noms différents pour le même concept.
- Si une décision d’architecture a un impact durable, on crée une **ADR** (Architecture Decision Record).

**Exemple d’ADR**

```text
docs/adr/0001-use-sveltekit-and-prisma.md
docs/adr/0002-server-authoritative-game-state.md
```

---

## 6. Pull Requests

## 6.1 Règle générale

Toute PR doit être **petite, claire, testable, et reliée à une issue**. Les reviews servent à détecter bugs, incohérences, dettes techniques et problèmes de compréhension, ce qui va dans le sens des code reviews recommandées par le sujet.

## 6.2 Taille recommandée
- Idéal : 50 à 400 lignes.
- Au-delà, il faut découper si possible.
- Une PR énorme ralentit la review et augmente les erreurs.

## 6.5 Checklist obligatoire avant review
- Le code compile.
- Les tests passent.
- Le lint passe.
- Le formatage est appliqué.
- Les migrations DB sont incluses si besoin.
- La documentation a été mise à jour si nécessaire.
- La PR est reliée à une issue.
- Les captures / logs utiles sont joints pour les changements UI ou infra.

## 6.6 Merge policy
Je recommande :
- **Squash merge** par défaut pour garder un historique propre.
- Interdiction de merge d’une PR qui a des checks rouges.
- Interdiction d’auto-merge sans review.
- Rebase sur `main` avant merge si la branche est en retard.

---

## 7. Code Review
## 7.1 Ce qu’on vérifie en review
- Le code fait-il vraiment ce que dit l’issue ?
- Le code est-il lisible ?
- Les cas limites sont-ils gérés ?
- Les erreurs sont-elles gérées proprement ?
- Le naming est-il cohérent ?
- La sécurité de base est-elle respectée ?
- La solution est-elle trop complexe pour le besoin ?

## 7.2 Règles de review
- On critique le code, jamais la personne.
- Une review doit être précise et actionnable.
- Toute demande de changement doit expliquer **pourquoi**.
- Si le point est bloquant, on le marque clairement.
- Si c’est une suggestion, on le dit.

## 7.3 CODEOWNERS

Un fichier `CODEOWNERS` permet de définir qui doit être demandé en review sur quelles zones du repo, et GitHub l’utilise sur la branche de base de la PR

**Exemple**
```text
*                          @team/core
/apps/frontend/*           @team/frontend 
/apps/backend/*            @team/backend 
/infrastructure/*          @team/devops 
/docs/*                    @team/docs
```

---

## 8. Issues et gestion du travail

Le sujet recommande explicitement une organisation des tâches, des réunions régulières, une division du travail, et un suivi clair des responsabilités.

## 8.1 Une issue = un objectif clair

Chaque issue doit contenir :
- Contexte.
- Objectif.
- Critères d’acceptation.
- Dépendances.
- Risques éventuels.
- Estimation.
- Responsable.

## 8.4 Règles
- Pas de branche sans issue.
- Pas de PR sans issue liée.
- Pas d’issue vague du type “faire le chat”.
- Une issue trop grosse doit être découpée.

---

## 9. CI avec GitHub Actions
GitHub Actions permet d’exécuter automatiquement des workflows déclenchés sur `push` ou `pull_request`, et GitHub recommande d’utiliser `actions/checkout` dès qu’un job a besoin du code du dépôt.[docs.github](https://docs.github.com/en/actions/tutorials/create-an-example-workflow)

## 9.1 Checks obligatoires sur chaque PR
- Installation des dépendances.
- Lint.
- Format check.
- Tests unitaires.
- Build.
- Vérification Docker.
- Vérification migrations si besoin.

## 9.3 Jobs recommandés
- `lint`
- `test`
- `build`
- `docker-build`
- `e2e`
- `docs-check`

## 9.4 Politique CI
- Une PR rouge ne se merge pas.
- Une PR verte n’est pas automatiquement bonne, elle doit encore être relue.
- La CI doit rester rapide ; si elle devient lente, on la découpe.
- Les tests flakys doivent être traités immédiatement.

---

## 10. Docker et environnements

Le sujet exige une solution de conteneurisation exécutable en une commande.

## 10.1 Règles Docker
- Un `Dockerfile` propre par service.
- Un `docker-compose.yml` ou équivalent pour l’environnement local.
- Un `docker compose up --build` doit suffire pour démarrer le projet.
- Images minimales, étapes multi-stage si utile.
- Variables d’environnement documentées dans `.env.example`.

**Commandes**

```bash
docker compose up --build
docker compose down -v
docker compose logs -f
docker compose exec app sh
```

## 10.2 Environnements
- `local`
- `test`
- `staging`
- `production` si utile

## 10.3 Règles d’environnement
- Les valeurs sensibles ne vont jamais dans Git.
- Les configurations par environnement doivent être explicites.
- Les migrations doivent être versionnées.
- Les seeds de dev doivent être reproductibles.

---

## 11. Base de données
Le sujet demande une base avec schéma clair et relations bien définies.

## 11.1 Bonnes pratiques
- Migration obligatoire pour chaque changement de schéma.
- Pas de modification manuelle “hors migration” sur une DB partagée.
- Noms de tables, colonnes et index cohérents.
- Contraintes en base quand c’est pertinent.
- Données de test et seed documentés.

## 11.2 Politique de migration
- Une PR qui change le schéma doit inclure la migration.
- La migration doit être testée localement.
- Toute migration risquée doit avoir une note de rollback.

---

## 12. Tests

## 12.1 Pyramide minimale
- Tests unitaires sur la logique métier.
- Tests d’intégration sur API / DB / WebSocket.
- Quelques tests end-to-end sur les parcours critiques.

## 12.2 Ce qu’il faut couvrir en priorité
- Authentification.
- Permissions / rôles.
- Flux critiques métier.
- Validation des entrées.
- États d’erreur.
- Reconnexion / concurrence si temps réel, car le sujet exige une gestion correcte des utilisateurs simultanés et des actions concurrentes.

## 12.3 Politique
- Tout bug important corrigé doit avoir un test de non-régression.
- Pas de merge d’une feature critique sans test.
- Les tests doivent être lisibles et déterministes.

---

## 13. Sécurité minimale
Le sujet impose déjà auth sécurisée, validation côté front et back, usage correct des secrets, HTTPS côté backend et multi-utilisateur propre.

## 13.1 Checklist minimale
- Hash de mot de passe robuste.
- Validation stricte des entrées.
- Contrôle d’accès sur chaque route sensible.
- Vérification systématique des permissions.
- Protection des uploads.
- Logs sans fuite de secrets.
- Limitation des permissions des services.
- Rotation simple des secrets si possible.

## 13.2 Ce qu’on interdit
- Secrets en dur.
- Logs contenant tokens, mots de passe ou cookies.
- Route admin cachée sans vrai contrôle d’accès.
- Confiance dans le front pour les droits utilisateur.

---

## 14. Documentation obligatoire

Le sujet demande un README complet avec description, instructions, ressources, organisation d’équipe, stack technique, schéma de base, modules, contributions individuelles et gestion du projet.

## 14.1 Documents à maintenir
- `README.md`
- `docs/engineering-guidelines.md`
- `docs/architecture.md`
- `docs/runbook.md`
- `docs/adr/*.md`
- `CHANGELOG.md`

## 14.2 Ce qu’on documente
- Comment lancer le projet.
- Comment lancer les tests.
- Comment remplir `.env`.
- Comment faire une migration.
- Comment ouvrir une PR.
- Comment rollback un service localement.
- Qui possède quoi dans le projet.

## 14.3 Runbook minimal
Le `runbook` doit contenir :
- Démarrage local.
- Commandes utiles.
- Procédure si la DB casse.
- Procédure si la CI échoue.
- Procédure si une migration passe mal.
- Procédure pour recréer un environnement propre.

---

## 15. Observabilité et stabilité

Le sujet propose même des modules avancés de logs, monitoring, healthchecks, backups et disaster recovery.

## 15.1 Minimum recommandé
- Logs structurés.
- Healthcheck applicatif.
- Endpoint de statut.
- Logs d’erreurs exploitables.
- Monitoring minimal des services.
- Sauvegarde DB au moins en environnement partagé.

## 15.2 Ce qu’on suit
- Démarrage/arrêt service.
- Erreurs applicatives.
- Erreurs DB.
- Latence API.
- Taux d’échec des jobs.
- Reconnexions WebSocket si temps réel.

---

## 16. Réunions et pilotage

Le sujet recommande des points réguliers, un découpage du travail, des outils de suivi et une communication claire.

## 16.1 Rituels
- **Daily async** : chacun poste ce qu’il a fait, ce qu’il fait, ce qui bloque.
- **Planning hebdo** : choix des priorités.
- **Review technique** : sujets d’architecture et blocages.
- **Demo interne** : montrer quelque chose qui marche.

## 16.3 Règles
- Un blocage de plus de 24h doit être remonté.
- Une grosse décision doit être écrite.
- Une feature commencée doit avoir un responsable clair.

---

## 17. Définition of Done
Une tâche n’est **pas** “finie” quand le code existe ; elle est finie quand elle est intégrée proprement.

Une issue est **Done** seulement si :
- le code est terminé
- le lint passe
- les tests passent
- la PR est reviewée
- la CI est verte
- la doc utile est à jour
- la feature est démontrable
- le comportement est explicable en soutenance, ce qui est essentiel pour l’évaluation 42.

---

## 18. Commandes standard du projet
Ces commandes doivent exister dans tous les projets, peu importe la stack exacte.

```bash
# installation
pnpm install

# développement
pnpm dev

# qualité
pnpm lint
pnpm format
pnpm test
pnpm build

# docker
docker compose up --build
docker compose down -v
docker compose logs -f`
```

Si Rust est présent :

```bash
cargo fmt --all
cargo clippy --all-targets --all-features -- -D warnings
cargo test
cargo check
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
.editorconfig
.gitignore
.env.example
README.md
docs/
	engineering-guidelines.md
	architecture.md
	runbook.md
	adr/
CHANGELOG.md
```

---

## 20. Résumé exécutable

Dès la première semaine, on met en place :
- protection de `main`docs.github+1
- Conventional Commits + commitlint + huskydev+1
- templates de PR et d’issues
- CI GitHub Actions sur PR et push[docs.github](https://docs.github.com/en/actions/tutorials/create-an-example-workflow)
- lint + format + tests automatiques[docs.github](https://docs.github.com/en/actions/tutorials/create-an-example-workflow)
- Docker local reproductible en une commande
- README + docs d’architecture + runbook
- CODEOWNERS + review obligatoiregithub+1
- Definition of Done commune

---

## 21. Politique d’équipe

Nous préférons :
- la clarté à la “technique impressionnante”
- une PR petite à une grosse branche bloquée
- un code simple à une architecture brillante mais fragile
- une décision documentée à une convention implicite
- une feature terminée et démontrable à trois features à moitié finies, ce qui colle bien avec le fait que seuls les modules pleinement fonctionnels comptent à l’évaluation.

Si vous appliquez vraiment tout ce document, vous aurez déjà un niveau d’organisation et de stabilité **très au-dessus** de la moyenne pour un projet étudiant, tout en restant alignés avec les attentes du sujet.