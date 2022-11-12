# Organisation des Fichiers

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

## Réseaux

`/gridgroup/ilc/nnhAnalysisFiles`

```
/gridgroup/ilc/nnhAnalysisFiles
└───AHCAL
|   └─── 402001
|   |   └─── rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402001.Pe1e1h.eL.pR.n000.d_dstm_15089_0_MINI.slcio
|   |   └─── rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402001.Pe1e1h.eL.pR.n000.d_dstm_15089_1_MINI.slcio
|       ...
|   └───402002
|   |   └─── rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402002.Pe1e1h.eR.pL.n000.d_dstm_15089_0_MINI.slcio
|   |   └─── rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402002.Pe1e1h.eR.pL.n000.d_dstm_15089_1_MINI.slcio
|       ...
|   └─── ...
└───result
|   └─── original
|   |   └─── run_1
|   |   |   └─── processor
|   |   |   |   └─── 402001.root 
|   |   |   |   └─── 402002.root
|   |   |   |   └─── ...
|   |   |   └─── analysis
|   |   |   |   └─── run_1.1 
|   |   |   |   └─── run_1.2
|   |   |   |   └─── ...
|   |   └─── run_2
|   |   └─── ...
|   └─── ilcsoft
|   |   └─── run_1
|   |   |   └─── processor
|   |   |   |   └─── 402001.root 
|   |   |   |   └─── 402002.root
|   |   |   |   └─── ...
|   |   |   └─── analysis
|   |   |   |   └─── run_1.1 
|   |   |   |   └─── run_1.2
|   |   |   |   └─── ...
|   |   └─── run_2
|   |   └─── ...
|   └─── fcc
```

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

```
/path/to/nnhAnalysis
└─── program
|   └─── original
|   |   └─── miniDSTMaker
|   |   └─── processor
|   |   └─── analysis
|   └─── ilcsoft
|   |   └─── processor
|   |   └─── analysis
|   └─── fcc
└─── script
|   └─── nnh.sh
|   └─── nnhProcessor.sh
|   └─── nnhAnalysis.sh
|   └─── prepareBDT.sh
|   └─── launchBDT.sh
|   └─── tools
|       └─── help.sh
|       └─── export.sh
|       └─── functions.sh
└─── test
|   └─── testProcessor_isCompleted.py
|   └─── testProcessor_isSame2.py
|   └─── testAnalysis_isCompleted.py
|   └─── testAnalysis_isSame2.py
```
```
export NNH=/path/to/nnhAnalysis
```

### Dossiers du Programme `nnhHome`

```
/path/to/nnhAnalysis
└─── program
|   └─── original
|   └─── ilcsoft
|   └─── fcc
└─── script
└─── test
```

```
export NNH_PROGRAM=$NNH/program
```

#### Programme `original`
```
/path/to/nnhAnalysis/program/original
└─── miniDSTMaker
|   └─── miniDST.py
|   └─── Observer.py
|   └─── Transfer.py
|   └─── downloadAndMiniDst.py
|   └─── mini-DST-maker.xml
|   └─── lfns
|   |   └─── 402001.lfns
|   |   └─── 402002.lfns
|   |   └─── ...
|   └─── 4q250_ZZ_v4_p00_ildl5
|   |   └─── 4q250_v04_p00_ildl5_c0_bdt.class.C
|   |   └─── 4q250_v04_p00_ildl5_c1_bdt.class.C
|   |   └─── 4q250_v04_p00_ildl5_c2_bdt.class.C
|   |   └─── 4q250_v04_p00_ildl5_c3_bdt.class.C
|   |   └─── 4q250_v04_p00_ildl5_c0_bdt.weights.xml
|   |   └─── 4q250_v04_p00_ildl5_c1_bdt.weights.xml
|   |   └─── 4q250_v04_p00_ildl5_c2_bdt.weights.xml
|   |   └─── 4q250_v04_p00_ildl5_c3_bdt.weights.xml
└─── processor
|   └─── script
|   |   └─── Merge.py
|   |   └─── launchNNHProcessor.py
|   |   └─── NNHProcessor.py
|   |   └─── Observer.py
|   |   └─── Transfert.py
|   |   └─── NNH_steer.xml
|   |   └─── setEnv.sh
|   └─── include
|   |   └─── EventSharpe.hh
|   |   └─── NNHProcessor.hh
|   |   └─── ParticleInfo.hh
|   └─── src
|   |   └─── EventSharpe.cc
|   |   └─── NNHProcessor.cc
└─── analysis
|   └─── CMakeLists.txt
|   └─── channels.json
|   └─── exec
|   |   └─── prepareForBDT.cxx
|   └─── include
|   |   └─── Channels.hh
|   └─── python
|   |   └─── launchBDT_WW.py
|   |   └─── launchBDT_bb.py
```
```
export NNH_ORIGINAL=$NNH_PROGRAM/original
```

#### Programme `ilcsoft`
```
/path/to/nnhAnalysis/program/ilcsoft
└─── processor
|   └─── script
|   |   └─── Merge.py
|   |   └─── launchNNHProcessor.py
|   |   └─── NNHProcessor.py
|   |   └─── Observer.py
|   |   └─── Transfert.py
|   |   └─── NNH_steer.xml
|   |   └─── setEnv.sh
|   └─── include
|   |   └─── EventSharpe.hh
|   |   └─── NNHProcessor.hh
|   |   └─── ParticleInfo.hh
|   |   └─── PDGInfo.hh
|   └─── src
|   |   └─── EventSharpe.cc
|   |   └─── NNHProcessor.cc
|   |   └─── ParticleInfo.cc
|   |   └─── PDGInfo.cc
└─── analysis
|   └─── CMakeLists.txt
|   └─── channels.json
|   └─── exec
|   |   └─── prepareForBDT.cxx
|   └─── include
|   |   └─── Channels.hh
|   └─── python
|   |   └─── launchBDT_WW.py
|   |   └─── launchBDT_bb.py
```
```
export NNH_ILCSOFT=$NNH_PROGRAM/ilcsoft
```

#### Programme `fcc` 
```
/path/to/nnhAnalysis/program/fcc
```
```
export NNH_FCC=$NNH_PROGRAM/fcc
```

#### Sous-Programme `processor`
Les dossiers d'entrés et de sorties du programme :
```
export NNH_PROCESSOR_INPUT=$NNH_INPUT \
       NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/RESULTS
```

#### Sous-Programme `analysis`
Les dossiers d'entrés et de sorties du programme :
```
export NNH_ANALYSIS_INPUT=$NNH_HOME/processor/RESULTS \
       NNH_ANALYSIS_OUTPUT=$NNH_HOME/analysis/DATA 
```

##### Librairies générées `processor`
```
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so
```

## Dossier `script`
```
/path/to/nnhAnalysis
└─── program
└─── script
|   └─── nnh.sh
|   └─── nnhProcessor.sh
|   └─── nnhAnalysis.sh
|   └─── prepareBDT.sh
|   └─── launchBDT.sh
|   └─── tools
|       └─── help.sh
|       └─── export.sh
|       └─── functions.sh
└─── test
```
```
export NNH_SCRIPT=$NNH/script
```

## Dossier `test`
```
/path/to/nnhAnalysis
└─── program
└─── script
└─── test
|   └─── testProcessor_isCompleted.py
|   └─── testProcessor_isSame2.py
|   └─── testAnalysis_isCompleted.py
|   └─── testAnalysis_isSame2.py
```
```
export NNH_TEST=$NNH/test
```
