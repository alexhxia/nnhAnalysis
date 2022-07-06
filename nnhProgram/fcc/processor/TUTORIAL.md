# Tutorial rapide ``processor``

On traite une première fois les fichiers LCIO dans la partie `processor` afin obtenir un fichier ROOT par processus (cf `processor/README`), qui sera placer dans un dossier `RESULTS`.

## Préparer l'environnement
Les commandes pour avoir un environnement opérationnel, à refaire à chaque ouverture :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
```
export  NNH_INPUT=/gridgroup/ilc/nnhAnalysisFiles/AHCAL \
        NNH_OUTPUT=/gridgroup/ilc/nnhAnalysisFiles/result \
        NNH_HOME=~/nnhAnalysis/nnhHome/fcc 
```
```
export  NNH_PROCESSOR_INPUT=$NNH_INPUT/inputFiles \
        NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/outputFiles
```
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```
## Compilation 
```
mkdir $NNH_HOME/processor/build 
```
```
cd $NNH_HOME/processor/build
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
Marlin $NNH_HOME/processor/NNH_steer.py 
```
Pour exécuter tous les processus :
```
mkdir $NNH_PROCESSOR_OUTPUTFILES
```
```
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```
