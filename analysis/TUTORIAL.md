# Analysis

## Préparation de l'environnement
Il faut un environnement au moins sous `python 3.9` et avec `root`.
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
Avant d'exécuter `analysis` il faut avoir générer les fichiers ROOTs (voir la partie `processor`).
```
export  NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL \
        NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/result \
        NNH_HOME=~/nnhAnalysis/original
```
```
export  NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS
```
```
export  NNH_ANALYSIS_INPUTFILES=$NNH_PROCESSOR_OUTPUTFILES \
        NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA 
```

## Préparation de données
```
mkdir $NNH_ANALYSIS_OUTPUTFILES
```
```
hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root
```

## Compilation
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

## Préparer la BDT 
```
cd $NNH_HOME/analysis
```
```
./bin/prepareForBDT
```

## BDT
```
cd $NNH_HOME/analysis/python
```
Attention, il ne faut pas que la commande 
`source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh`
est étée exécuter. Donc il faut aussi ré-export les variables d'environnement.

```
python3 launchBDT_bb.py
```
or : 
```
python3 launchBDT_ww.py
```
