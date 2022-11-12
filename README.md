# nnhAnalysis

Ce programme permet l'analyse de fichiers LCIO dans le cadre des futurs projets de collisionneur leptonique.

L'objectif est de développer des programmes d'analyse pour les cannaux :

- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

> Ce projet est basé sur le travail de `ggarillot` pour ILC (Japon), disponible directement depuis son github :
> ```
> git clone --branch refactor https://github.com/ggarillot/nnhAnalysis.git
> ```

Ma version se place dans le cadre de mon stage de stage M2 effectuer à l'IP2I, l'Institut de Physique des 2 Infinies (CNRS) dans le groupe FCC (sous-groupe de CMS). Mon objectif est d'amélioré le travail de `ggarillot` et de le rendre compatible avec le projet FCC et ses nouvelles suites logicielles.

> Pour l'importer :
> ```
> git clone https://github.com/alexhxia/nnhAnalysis.git
> ```

# Organisation du Projet

## [`LaTeX`](LaTeX)

Ce dossier contient tous les documents que j'ai produit au cours de mon stage :
- Ma présentation pour FCC France 2022
- Mon rapport de Stage
- Ma présentaion de Stage

## [`program`](program)

Dans ce dossiers se trouve les codes des différents projets :
- [`original`](program/original) le programme de `ggarillot` avec de très légères modifications
- [`ilcsoft`](program/ilcsoft) mon adaptation du programme de `ggarillot` pour ILCSoft
- [`fcc`](program/fcc) une adaptation du programme de `ilcsoft` pour FCC

## [`script`](script)

Ici vous trouverez des scripts permettant d'automatiser la compilation, l'exécution et la gestion des données générées par les programmes.

## [`test`](test)

Dans ce dossier vous trouverez des programmes python qui permettent de vérifier les données générées par les programmes.

## [`result`](result)

Dans ce dossier vous trouverez résultats en JSON qui valide les données générées par les programmes.
