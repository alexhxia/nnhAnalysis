# Tutorial ``processor``

## Définition de l'environnement
À chaque redémarage de terminal, il est nécessaire d'exécuter :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
```
export NNH_HOME=~/nnhAnalysis/ilcsoft
```
Dans ce tutorial, on considère que les fichiers sont en local dans le dossier `/gridgroup/ilc/nnhAnalysisFiles/AHCAL`.
Et on placera nos fichiers de résultats dans le sous dossier `nnhAnalysis/ilcsoft/processor/RESULTS`.
```
export NNH_PROCESSOR_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL/ \
       NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS 
```
Si c'est la première exécution du code, il est nécessaire d'avoir un dossier de copilation et de résultats :
```
mkdir $NNH_HOME/processor/BUILD $NNH_HOME/processor/RESULTS
```
## Compilation
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
Exécution rapide :
```
cmake -C $ILCSOFT/ILCSoft.cmake .. & make & make install
```
La compilation génère une bibliotèque `libnnhProcessor` qu'il faut impérativement ajouter dans le `MARLIN_DLL`. 
Donc avant d'exécuter les commandes du programme, il faut donc obligatoirement, même sans recompilation :
```
export MARLIN_DLL=$MARLIN_DLL:~/nnhAnalysis/processor/lib/libnnhProcessor.so
```

## Exécution des scripts
```
cd $NNH_HOME/processor/script/
```

### Convertir un seul fichier
Dans le programme `NNH_steer.xml `, il faut adapter le fichier d'entrée `input.slcio` et  le fichier de sortie `output.root` par les chemins souhaités.

Ensuite pour exécuter le programme :
```
Marlin NNH_steer.xml 
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
python3 ./launchNNHProcessor.py -n 100 -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES
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
