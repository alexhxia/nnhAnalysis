# `nnhTest` directory

Ce dossier regroupe toutes les fonctions qui vont permettent de tester les résultats de programme de [`nnhHome`](../nnhHome) 
(cad les progremmes `processor` et `analysis`).

## `testXX_isCompleted.py` programs
Vérifie que le programme à générer tous les fichiers qu'il devrait.

### For `processor`

```
testProcessor_isCompleted.py -p path/to/processor_directory_root_files
```
Vérifie que le programme `processor` à bien un fichier root par numéro de processus :
```
# processus number list
402173, 402182, 402007, 402008, 402176, 402185, 402009, 402010, 402011, 
402012, 402001, 402002, 402013, 402014, 402003, 402004, 402005, 402006, 
500006, 500008, 500010, 500012, 500062, 500064, 500066, 500068, 500070, 
500072, 500074, 500076, 500078, 500080, 500082, 500084, 500101, 500102, 
500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112, 500086, 
500088, 500090, 500092, 500094, 500096, 500098, 500100, 500113, 500114, 
500115, 500116, 500117, 500118, 500119, 500120, 500122, 500124, 500125, 
500126, 500127, 500128
```

### For `analysis`

Le programme `testAnalysis_isCompleted.py` prend le chemin d'un dossier et test si ce dossier contient tous les fichiers générés par le programme `analysis`.

```
python testAnalysis_isCompleted.py -a path/to/directory
```
Vérifie que le dossier `directory` a bien tous les fichiers suivants :
```
# files created by XX (here for ww or bb)
"bestSelection_XX_e-0.8_p+0.3.root", 
"split_XX_e+0_p+0.root",
"split_XX_e-0.8_p+0.3.root",
"scores_XX_e-0.8_p+0.3.root",
"stats_XX_e-0.8_p+0.3.json",
"model_XX_e-0.8_p+0.3.joblib"
"DATA.root"
```

## `testXX_isSame2.py` programs
Regarde, suivant la définition de Kolmogorov, si 2 dossiers de résultats sont identiques.

### For `processor`
Tous les processor doivent être identiques.
```
testProcessor_isSame2.py -p1 path/to/processor_directory_root_files_1 -p2 path/to/processor_directory_root_files_2
```

### For `analysis`
Les résultats peuvent être légèrement différents mais doivent rester équivalent.
```
testProcessor_isSame2.py -a1 path/to/analysis_directory_data_files_1 -a2 path/to/analysis_directory_data_files_2
```
