# Organisation des Fichiers

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

## Réseaux

`/gridgroup/ilc/nnhAnalysisFiles`

### Fichiers Initiaux

`/gridgroup/ilc/nnhAnalysisFiles/AHCAL`

```
export NNH_INPUT=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
```

### Fichiers de Sorties

`/gridgroup/ilc/nnhAnalysisFiles/result`

```
export NNH_OUTPUT=/gridgroup/ilc/nnhAnalysisFiles/result
```

## Projet `nnhAnalysis`

### Dossiers du Programme `nnhHome`
```
export NNH_HOME=~/nnhAnalysis/nnhHome
```

#### Programme `original`

```
export NNH_ORIGINAL=~/nnhAnalysis/nnhHome/original
```

#### Programme `ilcsoft`
```
export NNH_ILCSOFT=~/nnhAnalysis/nnhHome/ilcsoft
```

#### Programme `fcc`
```
export NNH_FCC=~/nnhAnalysis/nnhHome/fcc
```

#### Sous-Programme `processor`
```
export NNH_PROCESSOR_INPUT=$NNH_INPUT \
       NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/RESULTS
```

#### Sous-Programme `processor`
```
export NNH_ANALYSIS_INPUT=$NNH_HOME/processor/RESULTS \
       NNH_ANALYSIS_OUTPUT=$NNH_HOME/analysis/DATA 
```

##### Librairies générées `processor`
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```

## Projet `nnhScript`
```
export NNH_SCRIPT=~/nnhAnalysis/nnhScript
```

## Projet `nnhTest`
```
export NNH_TEST=~/nnhAnalysis/nnhTest
```
