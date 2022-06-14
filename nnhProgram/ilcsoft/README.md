# nnhAnalysis
Dans ce dépots, le programme permet l'analyse les cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

# La Branche `ilcsoft`
Dans cette branche, on ajoute des petites corrections aux fichiers originaux de `ggarillot` afin de s'adapter à un stockage local.

## importer 
Pour importer directement de github :
```
git clone --branch ilcsoft https://github.com/alexhxia/nnhAnalysis.git ilcsoft
```
On considère le dossier que nous venons de créer comme le `NNH_HOME` de notre projet :
```
export NNH_HOME=$PWD/nnhAnalysis/ilcsoft
```
On se place dans le dossier créer :
```
cd $NNH_HOME
```
Pour exécuter ce code, on a aussi besoin de préparer l'environnement grâce à :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```

## Données initiales
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
```
export  NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL \
        NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/RESULTS
```

## `processus`
On traite une première fois les fichiers LCIO dans la partie `processor` afin obtenir un fichier ROOT par processus (cf `processor/README`), qui sera placer dans un dossier `PROCESSOR_OUTPUT`.
```
export  NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS
```
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```

## `analysis`
```
export  NNH_ANALYSIS_INPUTFILES=$NNH_HOME/processor/RESULTS \
        NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA 
```
Attention, il ne faut pas charger ``source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh`` si on souhaite effectuer `$NNH_HOME/analysis/python/launchBDT_XX.py`.

# Rappel : le Projet initial
Ce projet est basé sur le travail de `ggarillot` accéssible directement de son github :
https://github.com/ggarillot/nnhAnalysis/tree/refactor
