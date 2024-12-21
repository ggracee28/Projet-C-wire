# C-Wire Project

Ce projet analyse un réseau de distribution d'électricité en simulant environ un quart de la France. 
Il permet d'évaluer la production et la consommation d'énergie à différents niveaux du réseau en
- Filtrant les données en fonction de la station (HVB, HVA, LV) ou du consommateur
- Calculant la somme des consommations et production totale avec un AVL
- Afficher ces résultats dans des fichiers CSV

## Structure du projet

```
c-wire/
├── c-wire.sh          # Script shell principal
├── codeC/             # Code source C
│   ├── Makefile       # Fichier Makefile pour la compilation
│   ├── avl_tree.c     # Implémentation de l'arbre AVL
│   ├── avl_tree.h     # Déclaration des fonctions et structures AVL
│   └── main.c         # Programme principal
├── data/              # Données d'entrée
│   ├── c-wire_v00.csv # Fichier de données initial
│   └── input/         # Fichiers d'entrée supplémentaires
├── graphs/            # Graphiques générés
│   └── lv_consumption 
├── output/            # Résultats de sortie générés à l'exécution
│   └── lv_all_minmax.csv # Exemple de fichier de sortie
├── test/              # Résultats finaux
├── tmp/               # Fichiers temporaires

On a également en plus de ça le Rapport de projet C-Wire.pdf qui contient le compte rendu sur l'organisation de notre équipe au cours du projet et les difficulté rencontrées.


### Installation

- Ouverture d'un terminal en C
- Executer les commandes suivantes pour installer gnuplot:
    sudo apt-get update
    sudo apt-get install gnuplot

#### Utilisation

1. Télécharger le dossier en cliquant sur "code" et "Download.zip"
2. Extraire le dossier en le dé-zippant
3. Placer votre fichier au format .csv à traiter dans le fichier input de notre projet c-wire
4. Dans le terminal, lancer le script Shell c-wire.sh en utilisant: 
./c-wire.sh data/<fichier_csv><type de station voulu><type de consommateur voulu><identifiant de la centrale (facultatif)>
5. En cas d'erreur lors de l'éxecution de script Shell, pour rendre le fichier éxécutable, écrire: chmod777 c-wire.sh
6. Si vous avez besoin d'aide vous pouvez écrire : ./c-wire.sh -h. Une page d'aide s'affichera alors.

Différentes stations:
- HVB
- HVA
- LV

Consommateurs disponibles:
- comp
- indiv
- all (indiv + comp)

exemples de sasie:
- ./c-wire.sh data/c-wire_v00.csv hvb comp
- ./c-wire.sh data/c-wire_v00.csv lv all


##### Auteurs
Alexandre KLOBB
Grace BIDI SINDA
Maëlle SMILIANITCH

