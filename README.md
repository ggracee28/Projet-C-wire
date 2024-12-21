# C-Wire Project

Ce projet analyse un réseau de distribution d'électricité en simulant environ un quart de la France. 
Il permet d'évaluer la production et la consommation d'énergie à différents niveaux du réseau en
- Filtrant les données en fonction de la station (HVB, HVA, LV) ou du consommateur
- Calculant la somme des consommations et production totale avec un AVL
- Afficher ces résultats dans des fichiers CSV

## Structure du projet

```
C-Wire/
├── c-wire.sh          # Script shell principal
├── codeC/             # Code source C
│   ├── main.c         # Programme principal
│   ├── avl_tree.c     # Implémentation de l'arbre AVL
│   ├── avl_tree.h     # Déclaration des fonctions et structures AVL
│   └── Makefile       # Fichier Makefile pour la compilation
├── data/              # Données d'entrée
│   └── DATA_CWIRE.csv
├── graphs/            # Graphiques générés
├── tmp/               # Fichiers temporaires
├── output/            # Résultats de sortie et test
│   └── lv_all_minmax.csv # exemple de fichier de sortie

On a également en plus de ça le Rapport de projet C-Wire.pdf qui contient le compte rendu sur l'organisation de notre équipe au cours du projet et les difficulté rencontrées.


## Installation

- Ouverture d'un terminal en C
- Executer les commandes suivantes pour installer gnuplot:
    sudo apt-get update
    sudo apt-get install gnuplot

## Utilisation

1. Télécharger le dossier en cliquant sur "code" et "Download.zip"
2. Extraire le dossier en le dé-zippant
3. Placer votre fichier au format .csv à traiter dans le fichier input de notre projet c-wire
4. Dans le terminal, lancer le script c-wire.sh en utilisant: 
./c-wire/c-wire.sh <type de station voulu><type de consommateur voulu><identifiant de la centrale (facultatif)>

Différentes stations:
- HVB
- HVA
- LV

Consommateurs disponibles:
- comp
- HVA
- indiv
- all (indiv + comp + HVA)

exemples de sasie:
- ./c-wire/c-wire.sh data/c-wire_v00.csv hvb comp
- ./c-wire/c-wire.sh data/c-wire_v00.csv lv all


## Auteurs
Alexandre KLOBB
Grace BIDI SINDA
Maëlle SMILIANITCH
