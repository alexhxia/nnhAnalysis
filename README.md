# nnhAnalysis

Ce programme permet l'analyse de fichiers LCIO dans le cadre des futures projets de collisionneur leptonique.

L'objectif est de développer des programmes d'analyse des cannaux :

- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

> Ce projet est basé sur le travail de `ggarillot` pour ILC (Japon), disponible directement depuis son github :
> ```
> git clone --branch refactor https://github.com/ggarillot/nnhAnalysis.git
> ```

Ma version se place dans le cadre de mon stage de stage M2 effectuer à l'IP2I, l'Institut de Physique des 2 Infinies (CNRS), dans groupe FCC (sous-groupe de CMS). Ici je cherche à rendre le travail de `ggarillot` compatible avec le projet FCC avec ses nouvelles suites logicielles.

Pour l'importer :
```
git clone https://github.com/alexhxia/nnhAnalysis.git
```

# Organisation du Projet

## [`LaTeX`](LaTeX)

Mon rapport et ma présentaion de stage.

## [`nnhProgram`](nnhProgram)

Dans ce dossiers se trouve les codes des différents projets.

## [`nnhScript`](nnhScript)

Ici vous trouverez des scripts permettant d'automatiser la compilation, l'exécution et la gestion des données générées par les programmes.

## [`nnhTest`](nnhTest)

Dans ce dossier vous trouverez des programmes python qui permettent de vérifier les données générées par les programmes.

## [`nnhResult`](nnhResult)

Dans ce dossier vous trouverez résultats en JSON qui vérifie les données générées par les programmes.
