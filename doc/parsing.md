# Notes sur le parsing

## Objectif actuel

Creer un petit parseur IRC simple et progressif pour decouper un message en commande et en parametres.

## Comportement actuel

Le parseur fait pour l'instant :
- enlever les espaces et les tabulations au debut et a la fin
- separer l'entree en mots
- garder le premier mot comme nom de commande
- garder les autres mots comme parametres

## Cas de test de base

Le mode `--parse-test` dans `src/main.cpp` teste pour l'instant :
- `JOIN` avec un parametre (`#general`)
- `PASS` avec un parametre (`1234`)
- des espaces et des tabulations autour des mots
- une ligne qui finit par `\r\n`

## Prochaines etapes

Les prochaines etapes peuvent etre :
- gerer plusieurs parametres sur une meme ligne
- gerer proprement les lignes vides
- preparer une structure pour la logique propre aux commandes IRC
