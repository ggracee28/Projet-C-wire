# C-Wire Project

Ce projet analyse un réseau de distribution d'électricité en simulant environ un quart de la France. Il permet d'évaluer la production et la consommation d'énergie à différents niveaux du réseau.

## Structure du projet

```
C-Wire/
├── c-wire.sh          # Script shell principal
├── codeC/              # Code source C
│   ├── main.c        # Programme principal
│   └── avl_tree.c    # Implémentation de l'arbre AVL
├── data/             # Données d'entrée
│   └── DATA_CWIRE.csv
├── graphs/           # Graphiques générés
├── tmp/              #

./c-wire.sh data/c-wire_v00.csv lv all