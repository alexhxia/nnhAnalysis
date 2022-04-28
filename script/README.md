# nnhAnalysis
Dans ce dépots, le programme permet l'analyse les cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

# La Branche `script`

## importer 
Pour importer directement de github :
```
git clone --branch script https://github.com/alexhxia/nnhAnalysis.git script
```

## Données initiales
Les données LCIO sont stockées localement dans le dossier :
```
/gridgroup/ilc/nnhAnalysisFiles/AHCAL
```
Puis chaque fichier est trié dans des sous-dossiers en fonction de leur numéro de processus.

Un exemple de nom d'un fichier avec son chemin :
``` 
/gridgroup/ilc/nnhAnalysisFiles/402001/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I402001.Pe1e1h.eL.pR.n000.d_dstm_15089_0_MINI.slcio 
```
```
inputFiles
└───402001
|   | 402001_file_0_mini-DST.slcio
|   | 402001_file_1_mini-DST.slcio
|   | ...
└───402002
|   | 402002_file_0_mini-DST.slcio
|   | 402002_file_1_mini-DST.slcio
|   | ...
└───402003
|   | 402003_file_0_mini-DST.slcio
|   | 402003_file_1_mini-DST.slcio
|   | ...
...
```

## `processus`
On traite une première fois les fichiers LCIO dans la partie `processor` afin obtenir un fichier ROOT par processus (cf `processor/README`), qui sera placer dans un dossier `PROCESSOR_OUTPUT`.
```
./processor.sh
```

## `analysis`
```
./analysis_prepareBDT.sh
```
```
./analysis_launchBDT.sh
```

# Rappel : le Projet initial
Ce projet est basé sur le travail de `ggarillot` accéssible directement de son github :
https://github.com/ggarillot/nnhAnalysis/tree/refactor
