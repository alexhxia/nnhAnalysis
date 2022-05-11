# `nnhHome` directory

Dans ce dossier se trouve les 3 projets d'analyse des cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

## Fonctionnement  

Ce dossier contient les différentes projets que j'appelle "branche", elle correspond à un nom de projet : `ìlcsoft`, `fcc`... Sachant que ces projets numériques sont idépendants les uns, les autres. 

Chacune doit au moins contenir un dossier `processus` et `analysis`, qui contient les codes du programmes.

### `processor`

#### Input data
Les données LCIO sont stockées localement dans le dossier :
```
/gridgroup/ilc/nnhAnalysisFiles/AHCAL
```
Un exemple de nom d'un fichier avec son chemin :
``` 
/gridgroup/ilc/nnhAnalysisFiles/402001/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402001.Pe1e1h.eL.pR.n000.d_dstm_15089_0_MINI.slcio 
```
Puis chaque fichier est trié dans des sous-dossiers en fonction de leur numéro de processus.
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
```
export NNH_HOME=/path/to/nnhHome/branch
```
Pour construire le projet :
```
mkdir $NNH_HOME/processor/BUILD 
```
```
cd $NNH_HOME/processor/BUILD
```
```
cmake -C $ILCSOFT/ILCSoft.cmake .. 
```
```
make
```
```
make install
```
Ce qui va générer une bibliotèque qu'il faut déclarer :
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```
Et enfin, pour exécuter tous les processus, on doit utiliser le programme `script/launchNNHProcessor.py` :
```
export  NNH_PROCESSOR_INPUT=/gridgroup/ilc/nnhAnalysisFiles/AHCAL \ # input directory
        NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/RESULTS            # output directory
```
```
mkdir $NNH_PROCESSOR_OUTPUT
```
```
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```

#### Output data

`launchNNHProcessor.py ` va générer un fichier root par dossier de processus. Pour confirmer que tous ces fichiers sont bien créés, on peut utiliser le programme [`testProcessor_isCompleted.py`](../nnhTest/testProcessor_isCompleted.py).

De plus chacun de ces fichiers de sortie doivent donner des fichiers roots équivalents (voir identiques si vous êtes dans la même branche).
Que l'on peut tester avec le programme [`testProcessor_isSame2.py`](../nnhTest/testProcessor_isSame2.py).

### `analysis`

Ici il y a 2 programmes à exécuter.

#### `prepareBDT` program

##### Input data

##### Run
```
export NNH_HOME=/path/to/nnhHome/branch
```

##### Output data

#### `launchBDT` program

Attention, il ne faut pas charger ``source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh`` si on souhaite effectuer `$NNH_HOME/analysis/python/launchBDT_XX.py`.
##### Input data

##### Run
```
export NNH_HOME=/path/to/nnhHome/branch
```


##### Output data


## Branchs
### ``original`` branch : the init projet
Ce projet est basé sur le travail de `ggarillot` accéssible directement de son github :
https://github.com/ggarillot/nnhAnalysis/tree/refactor

blabla à rajouter

### ``ilcsoft`` branch

blabla à rajouter

### ``fcc`` branch

blabla à rajouter
