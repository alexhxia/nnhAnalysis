# Analysis
This folder contains code to train a BDT to select events for the &nu;&nu;h (h &rarr; b bbar) and  &nu;&nu;h (h &rarr; WW* &rarr; qqqq) channels.

## Prérequis
Il faut un environnement au moins sous `python 3.9` et avec `root`.
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
Avant d'exécuter `analysis` il faut avoir générer les fichiers ROOTs (voir la partie `processor`).
```
export  NNH_INPUT=/gridgroup/ilc/nnhAnalysisFiles/AHCAL \
        NNH_OUTPUT=/gridgroup/ilc/nnhAnalysisFiles/result/ilcsoft \
        NNH_HOME=~/nnhAnalysis/nnhProgram/ilcsoft
```
```
export  NNH_PROCESSOR_INPUT=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/RESULTS
```
```
export  NNH_ANALYSIS_INPUT=$NNH_PROCESSOR_OUTPUTFILES \
        NNH_ANALYSIS_OUTPUT=$NNH_HOME/analysis/RESULTS 
```
```
mkdir -pv$NNH_ANALYSIS_OUTPUTFILES $NNH_HOME/analysis/BUILD
```

## Préparation de données
Dans un premier temps, on doit fusionner l'intégralité des fichiers ROOTs générer par la programme `processor` en un seul gros fichier nomé ``DATA.root`` que l'on rangera dans le dossier  ``RESULTS`` : 
```
hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root
```
where ``/path/to/single/rootfiles`` is the folder containing all the single ROOT files outputed by the Marlin processor you previously had to run.

The analysis code in this folder is maybe overcomplicated and a bit rushed so I will try to explain the global principles.

## Compilation
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
First, to use a BDT, you have to split the data set into two sets, the training and the testing set. 
This is done by the ``exec/prepareForBDT.cxx`` file :
```
cd $NNH_HOME/analysis
```
```
./bin/prepareForBDT
```
This program will output 4 files : ``split_bb_e-0.8_p+0.3.root``, ``split_bb_e+0_p+0.root``,  ``split_ww_e-0.8_p+0.3.root`` and ``split_ww_e+0_p+0.root`` in the ``DATA/`` folder you previously created. The ``e-0.8_p+0.3`` or ``e+0_p+0`` denotes the beam polarization and ``bb`` or ``ww`` denotes the (h &rarr; b bbar) or (h &rarr; WW* &rarr; qqqq) cases.

Those 4 files contains each a TTree with these variables :
- ``isSignal`` : boolean indicating if the event is a signal or a background one
- ``channelType`` : integer indicating the type of the event (signal, 2 fermions leptonic, 2 fermions hadronic...)
- ``isTrain`` : boolean indicating if the event is in the training or the testing set
- ``preSelected`` : boolean indicating if the event has passed the preselection cuts prior to the BDT
- ``weight`` : (float) weight of the event (for 1 fb<sup>-1</sup> integrated luminosity). The training and the testing sets have their own different weights. In the case of the training set, the event weight for the signal events are put arficially very high to equilibrate the number of events between signal and background to help the BDT training.

These TTree are meant to be [friend trees](https://root.cern.ch/doc/master/treefriend_8C.html) of the one in the ``DATA.root`` file

## BDT
### Pré-requis 
Les paquets `joblib`, `pandas`, `numpy`, `sklearn`, `cppyy` et `ROOT`
```
install joblib pandas numpy sklearn cppyy
```

The BDT part is done using python scripts and [scikit-learn](https://scikit-learn.org). The problem is that my python scripts are python3 and if you try to run them using the ROOT installed with ilcsoft it will not work because it is not configured to use python3 but python2. So in order to run the scripts you have to install or use a other installation of ROOT that is configured to use python3. The recent versions of ROOT are automatically configure to use both python2 and 3 (I think... in my case I used ROOT 6.24.02 and I had no problems).

### Lancement
Depuis le dossier `analysis/python` :
```
cd $NNH_HOME/analysis/python
```
Attention, il faut redémarrer ou changer de terminal, car la commande 
`source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh`
ne doit pas avoir été exécuter. Donc il faut aussi ré-export les variables d'environnement.

To run the BDT, use :
```
python3 launchBDT_bb.py
```
or : 
```
python3 launchBDT_ww.py
```
depending if you want to process the (h &rarr; b bbar) or (h &rarr; WW* &rarr; qqqq) case.

NB : si des problèmes persistes, vérifier que vous êtes en `python>=3.9` et en `ROOT>=6.24` :
```
python --version 
```
```
root --version
```

This script outputs a lot of files : 
- the ``model_....joblib`` file that contains the BDT model
- the ``scores_....root`` file that contains a TTree with this two variables :
    - ``BDTscore`` (float) BDT output
    - ``selected`` boolean that indicates is the event selected by the BDT or not
- the ``bestSelection_....root`` files that contains a TTree for only the selected events by the BDT
- the ``stats_....json`` file that contains statistics on event selection number and efficiencies

The TTree in the ``scores_....root`` file is meant to be a friend tree of the ``DATA.root`` file and ``split_.....root`` file. **WARNING : ALL the events (including the events that were not preselected before the BDT have a entry with the BDT score**, so if you want to look at the BDT scores for relevant events (the ones that passed the preselection), you have to filter them using the ``preSelected`` variable of the ``split_.....root`` file. The ``selected`` variable represents events that passed the BDT cut for the BDT cut that maximises the final significance. As using 3 TTrees at the same time and applying cuts can be quite cumbersome, you can use the ``bestSelection_....root`` that contains all the variables for the 3 TTrees for only the selected events of the testing sets. If you want to produce plots of the final result, you should use this file.

