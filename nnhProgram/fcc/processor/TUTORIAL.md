# Tutorial rapide ``processor``

On traite une première fois les fichiers LCIO dans la partie `processor` afin obtenir un fichier ROOT par processus (cf `processor/README`), qui sera placer dans un dossier `RESULTS`.

## Préparer l'environnement
Les commandes pour avoir un environnement opérationnel, à refaire à chaque ouverture :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
```
export  NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL \
        NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/result \
        NNH_HOME=~/nnhAnalysis/nnhProgram/fcc 
```
```
export  NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES \
        NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS
```
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

Pour exécuter tous les processus :
```
mkdir $NNH_PROCESSOR_OUTPUTFILES
```
```
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```


### Convertir plusieurs processus
Pour rappel, les fichier d'entrée LCIO sont ``/gridgroup/ilc/nnhAnalysisFiles/AHCAL/`` et seront stockés dans ``nnhAnalysis/ilcsoft/processor/RESULTS``. 

#### option `-h` `--help`
```
python3 ./launchNNHProcessor.py -h
```
```
$ python3 ./launchNNHProcessor.py -h
> usage: launchNNHProcessor.py [-h] [-n NCORES] [-p PROCESSES [PROCESSES ...]] -i INPUTDIRECTORY
> [-r] -o OUTPUTDIRECTORY
> 
> optional arguments:
>   -h, --help            show this help message and exit
>   -n NCORES, --ncores NCORES
>                         Number of threads
>   -p PROCESSES [PROCESSES ...], --processes PROCESSES [PROCESSES ...]
>                         ProcessIDs to analyse
>   -i INPUTDIRECTORY, --inputDirectory INPUTDIRECTORY
>                         Path of input files
>   -r, --remote          indicate that files need to be downloaded
>   -o OUTPUTDIRECTORY, --outputDirectory OUTPUTDIRECTORY
>                         output directory
```

#### option `-n NCORES` `--ncores NCORES`
Personalise le nombre de threads utilisé, par défaut 8 :
```
python3 launchNNHProcessor.py -n 100 -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```

#### option `-p PROCESSES [PROCESSES ...]` `--processes PROCESSES [PROCESSES ...]`
N'analysera que les numéros des processus cités, tous sinon.
```
python3 launchNNHProcessor.py -n 10 -p 402007 402008 -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```

#### Convertir tous les processus sans option facultative
 - option `-i INPUTDIRECTORY`, avec `INPUTDIRECTORY` le répertoire où sont situés les fichiers d'entrés
 - option `-o OUTPUTDIRECTORY`, avec `OUTPUTDIRECTORY` le répertoire où seront situés les fichiers de sorties
```
python3 launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
```

# Suite 
Continuer dans la partie `analysis`.

