# `nnhHome` directory

Dans ce dossier se trouve les 3 projets d'analyse des cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

## Fonctionnement  

Ce dossier contient les différents branches, que j'appelle "branche" qui correspond à un nom de projet : `ìlcsoft`, `fcc`... Sachant que ces projets numériques sont idépendants les uns, les autres. 

Chacune doit au moins contenir un dossier `processus` et `analysis`, qui contient les codes du programmes.

## Programs

### `processor`

#### Input data
Les données LCIO sont stockées localement dans le dossier :
```
/gridgroup/ilc/nnhAnalysisFiles/AHCAL
```
Puis chaque fichier est trié dans des sous-dossiers en fonction de leur numéro de processus.

Un exemple de nom d'un fichier avec son chemin :
``` 
/gridgroup/ilc/nnhAnalysisFiles/402001/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402001.Pe1e1h.eL.pR.n000.d_dstm_15089_0_MINI.slcio 
```
```
inputFiles
└───402001
|   | 402001_file_0_mini-DST.slcio
|   | 402001_file_1_mini-DST.slcio
|   | ...
└───402002
|   | 402002_file_0_mini-DST.slcio
|   | 402002_file_1_mini-DST.slcio
|   | ...
└───402003
|   | 402003_file_0_mini-DST.slcio
|   | 402003_file_1_mini-DST.slcio
|   | ...
...
```
#### Run

Pour exécuter ce code, on a aussi besoin de préparer l'environnement grâce à :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
Pour construire le projet :
```
```
Cela va générer une bibliotèque qu'il faut déclarer :
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```

#### Output data

doivent donner en sortie des fichiers root équivalents (voir identiques)

### `analysis`

#### Input data

#### Run

#### Output data


## Branchs
### ``original`` branch : the init projet
Ce projet est basé sur le travail de `ggarillot` accéssible directement de son github :
https://github.com/ggarillot/nnhAnalysis/tree/refactor

blabla à rajouter

### ``ilcsoft`` branch

blabla à rajouter

### ``fcc`` branch

blabla à rajouter
