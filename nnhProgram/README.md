# `nnhHome` directory

Dans ce dossier se trouve les 3 projets d'analyse des cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

## Fonctionnement  

Ce dossier contient les différentes projets que j'appelle "branche", elle correspond à un nom de projet : `ìlcsoft`, `fcc`... Sachant que ces projets numériques sont idépendants les uns, les autres. 

Chacune doit au moins contenir un dossier `processus` et `analysis`, qui contient les codes des programmes.

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
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUT -o $NNH_PROCESSOR_OUTPUT
```

#### Output data
```
$NNH_HOME/processor/RESULTS 
└───402001.root
└───402002.root
└───402003.root
...
```
`launchNNHProcessor.py ` va générer un fichier root par dossier de processus. Pour confirmer que tous ces fichiers sont bien créés, on peut utiliser le programme [`testProcessorCompleted.py`](../nnhTest/testProcessorCompleted.py).

De plus chacun de ces fichiers de sortie doivent donner des fichiers roots équivalents (voir identiques si vous êtes dans la même branche).
Que l'on peut tester avec le programme [`testProcessorSame.py`](../nnhTest/testProcessorSame.py).


### `analysis`

Ici il y a 2 programmes à exécuter.

#### `prepareForBDT` program

##### Input data

Prend en entrée le dossier des résultats du programme précédent :
```
inputFiles
└───402001.root
└───402002.root
└───402003.root
...
```

##### Run
Une fois encore il faut importer :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
```
export NNH_HOME=/path/to/nnhHome/branch
```
```
export  NNH_ANALYSIS_INPUT=/path/to/processorOutputDirectory \
        NNH_ANALYSIS_OUTPUT=$NNH_HOME/analysis/DATA 
```
Ensuite pour préparer les données
```
mkdir $NNH_ANALYSIS_OUTPUT
```
```
hadd $NNH_ANALYSIS_OUTPUT/DATA.root $NNH_ANALYSIS_INPUT/*.root
```
Puis, comme pour `processor`, il faut compiler le programme :
```
mkdir $NNH_HOME/analysis/BUILD
```
```
cd $NNH_HOME/analysis/BUILD
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
Et enfin, pour préparer la BDT :
```
cd $NNH_HOME/analysis
```
```
./bin/prepareForBDT
```

##### Output data

D'abord la commande `hadd` va construire un fichier `DATA.root`.

Puis le programme, `prepareForBDT` va construire plusieurs fichiers :
- split_XX_eXX_pXX.root, pour 2 type de particules "bb" ou "WW", 2 energies niveau d'énergies et 2 niveaux de polarisation "0" et "0", ou "-0.8" et "+0.3" :
        - split_bb_e+0_p+0.root
        - split_bb_e-0.8_p+0.3.root
        - split_ww_e+0_p+0.root
        - split_ww_e-0.8_p+0.3.root

#### `launchBDT` program

##### Input data

Il s'agit de la suite de `prepareForBDT`. Il utilise les fichiers de `$NNH_ANALYSIS_OUTPUT`.

##### Run

Attention, il ne faut pas charger ``source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh`` si on souhaite effectuer `$NNH_HOME/analysis/python/launchBDT_XX.py`. Donc il faut aussi ré-export les variables d'environnement.
```
export NNH_HOME=/path/to/nnhHome/branch
```
```
cd $NNH_HOME/analysis/python
```
```
python3 launchBDT_bb.py
```
et/ou : 
```
python3 launchBDT_ww.py
```

##### Output data

- model_XX_eXX_pXX.root, pour 2 type de particules "bb" ou "WW", au niveau énergie et de polarisation "-0.8" et "+0.3" :
        - model_bb_e-0.8_p+0.3.joblib
        - model_ww_e-0.8_p+0.3.joblib

- scores_XX_eXX_pXX.root, pour 2 type de particules "bb" ou "WW", au niveau énergie et de polarisation "-0.8" et "+0.3" :
        - scores_bb_e-0.8_p+0.3.root
        - scores_ww_e-0.8_p+0.3.root

- bestSelection_XX_eXX_pXX.root, pour 2 type de particules "bb" ou "WW", au niveau énergie et de polarisation "-0.8" et "+0.3" :
        - bestSelection_bb_e-0.8_p+0.3.root
        - bestSelection_ww_e-0.8_p+0.3.root
- stats_XX_eXX_pXX.root, pour 2 type de particules "bb" ou "WW", au niveau énergie et de polarisation "-0.8" et "+0.3" :
        - stats_bb_e-0.8_p+0.3.json
        - stats_ww_e-0.8_p+0.3.json


## Branchs
### ``original`` branch : the init projet
Ce projet est basé sur le travail de `ggarillot` accessible directement de son github :
https://github.com/ggarillot/nnhAnalysis/tree/refactor

Donc dans cette branche, vous trouverez les codes originaux, avec des corrections mineures (pour qu'ils puissent s'exécuter sur le server local et avec les données en local).

Ce programme est développé avec la suite logiciel `iLCSoft`, adapté pour le projet ILC.

### ``ilcsoft`` branch

Ce projet correspond à ma version du code original de `ggarillot`. Mais n'apporte aucune modification majeure et donne les mêmes résultats.

### ``fcc`` branch

blabla à rajouter
