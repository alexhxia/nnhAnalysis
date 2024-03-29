# Tutorial rapide ``processor``

On traite une première fois les fichiers LCIO dans la partie `processor` afin obtenir un fichier ROOT par processus (cf `processor/README`), qui sera placer dans un dossier de sortie.

## Préparer l'environnement
Pour avoir un environnement opérationnel, il faut exécuter les commandes suivantes à chaque ouverture :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
- Dossier qui contient les fichiers initiaux : `NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL`
- Dossier qui contient les codes des programmes à exécuter : `NNH_HOME=~/nnhAnalysis/original`
```
export  NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL 
        NNH_HOME=~/nnhAnalysis/original 
```
- Dossier qui contient les fichiers d'entré : `NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES`
- Dossier qui contiendra les fichiers de sorti : `NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS`
```
export  NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS
```
- Fichiers générés à la compilation du programme `processor` :
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```
## Compilation 
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
## Exécution
Pour un seul fichier (modifier avant le nom des fichiers `input.lcio` et `output.root` dans `Marlin NNH_steer.xml`) :
```
Marlin $NNH_HOME/processor/NNH_steer.xml 
```
Pour exécuter tous les processus :
```
mkdir $NNH_PROCESSOR_OUTPUTFILES
```
```
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```
