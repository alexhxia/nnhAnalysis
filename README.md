# nnhAnalysis
Stage M2 IP2I (CNRS), groupe FCC (CMS)

Dans ce dépots, le programme permet l'analyse les cannaux :

- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

# La Branche `fcc`
Dans cette branche, on convertie les fichiers de la branche (`ilcsoft`) `original`au projet du FCC du CERN.

## importer 
Pour importer directement de github :
```
git clone --branch fcc https://github.com/alexhxia/nnhAnalysis.git fcc
```
On considère le dossier que nous venons de créer comme le `NNH_HOME` de notre projet :
```
export NNH_HOME=nnhAnalysis
```
On se place dans le dossier créer :
```
cd nnhAnalysis
```
Ou 
```
git clone --branch fcc https://github.com/alexhxia/nnhAnalysis.git fcc
```
```
export NNH_HOME=fcc
```
```
cd fcc
```
Pour exécuter ce code, on a aussi besoin de préparer l'environnement grâce à :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```

## Données initiales
Les données LCIO sont stockées localement dans le dossier :
```
/gridgroup/ilc/nnhAnalysisFiles/AHCAL/
```
Puis chaque fichier est trié dans des sous-dossiers en fonction de leur numéro de processus.

Un exemple de nom d'un fichier avec son chemin :
``` 
/gridgroup/ilc/nnhAnalysisFiles/402001/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402001.Pe1e1h.eL.pR.n000.d_dstm_15089_0_MINI.slcio 
```

## `processus`
On traite une première fois les fichiers LCIO dans la partie `processor` afin obtenir un fichier ROOT par processus (cf `processor/README`), qui sera placer dans un dossier `PROCESSOR_OUTPUT`.

NB : les commandes pour avoir un environnement opérationnel, à refaire à chaque ouverture :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
```
export  NNH_HOME=~/nnhAnalysis \
        NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL/
```
```
export  NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/OUTPUT
```
```
export MARLIN_DLL=$MARLIN_DLL:~/nnhAnalysis/processor/lib/libnnhProcessor.so
```
## `analysis`
NB : les commandes pour avoir un environnement opérationnel, à refaire à chaque ouverture :
```
export  NNH_HOME=~/nnhAnalysis \
        NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL/
```
```
export  NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/OUTPUT
```
```
export  NNH_ANALYSIS_INPUTFILES=$NNH_PROCESSOR_OUTPUTFILES \
        NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA \
        NNH_DATA=$NNH_ANALYSIS_OUTPUTFILES
```
Attention, il ne faut pas le charger si on souhaite effectuer `$NNH_HOME/analysis/python/launchBDT_XX.py`.
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
# Rappel : le Projet initial
Ce projet est basé sur le travaille de `ggarillot` accéssible directement de son github :
https://github.com/ggarillot/nnhAnalysis/tree/refactor
