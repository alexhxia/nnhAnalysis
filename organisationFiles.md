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
└─── nnhHome
|   └─── original
|   |   └─── miniDSTMaker
|   |   └─── processor
|   |   └─── analysis
|   └─── ilcsoft
|   |   └─── processor
|   |   └─── analysis
|   └─── fcc
└─── nnhScript
|   └─── nnh.sh
|   └─── nnhProcessor.sh
|   └─── nnhAnalysis.sh
|   └─── prepareBDT.sh
|   └─── launchBDT.sh
|   └─── tools
|       └─── help.sh
|       └─── export.sh
|       └─── functions.sh
└─── nnhTest
|   └─── testProcessor_isCompleted.py
|   └─── testProcessor_isSame2.py
|   └─── testAnalysis_isCompleted.py
|   └─── testAnalysis_isSame2.py
```

### Dossiers du Programme `nnhHome`

```
/path/to/nnhAnalysis
└─── nnhHome
|   └─── original
|   └─── ilcsoft
|   └─── fcc
└─── nnhScript
└─── nnhTest
```

```
export NNH_HOME=~/nnhAnalysis/nnhHome
```

#### Programme `original`
```
/path/to/nnhAnalysis/nnhHome/original
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
export NNH_ORIGINAL=~/nnhAnalysis/nnhHome/original
```

#### Programme `ilcsoft`
```
/path/to/nnhAnalysis/nnhHome/ilcsoft
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
export NNH_ILCSOFT=~/nnhAnalysis/nnhHome/ilcsoft
```

#### Programme `fcc` 
```
/path/to/nnhAnalysis/nnhHome/fcc
```
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
/path/to/nnhAnalysis
└─── nnhHome
└─── nnhScript
|   └─── nnh.sh
|   └─── nnhProcessor.sh
|   └─── nnhAnalysis.sh
|   └─── prepareBDT.sh
|   └─── launchBDT.sh
|   └─── tools
|       └─── help.sh
|       └─── export.sh
|       └─── functions.sh
└─── nnhTest
```
```
export NNH_SCRIPT=~/nnhAnalysis/nnhScript
```

## Projet `nnhTest`
```
/path/to/nnhAnalysis
└─── nnhHome
└─── nnhScript
└─── nnhTest
|   └─── testProcessor_isCompleted.py
|   └─── testProcessor_isSame2.py
|   └─── testAnalysis_isCompleted.py
|   └─── testAnalysis_isSame2.py
```
```
export NNH_TEST=~/nnhAnalysis/nnhTest
```
