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


Placer le dossier c-wire dans votre dossier personnel et placer votre fichier au format .csv à traiter, dans le fichier input de c-wire.

Ouvrir le terminal, lancer le script c-wire.sh en utilisant: 
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

##c-wire.sh 
Le script shell principal permet:
- D'afficher de l'aide en cas de besoin
- Valider les paramètres initiales (nombre d'arguments, type de consommateur etc.)
- Vérification de l'existence du fichier csv
- Création des répertoires temp, graphs et output s'ils n'existent pas
- Compile le code en C
- Filtrage des données

##codeC
- Définition de la strucutre de base dans avl_tree.h
- Codage de l'arbre dans avl_tree.c
- Programme en c dans le main.c
- Makefile

##data
- emplacement du fichier d'entrée

##graphs
-  génération des graphs pour le bonus

##output
- les différents fichiers de résultat

##tmp
- fichiers temporaires


./c-wire.sh data/c-wire_v00.csv lv all 