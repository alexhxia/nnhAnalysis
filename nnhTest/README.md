# `nnhTest` directory

Ce dossier regroupe toutes les fonctions qui vont permettent de tester les résultats de programme de [`nnhHome`](../nnhHome) 
(cad les programmes `processor` et `analysis`).

## `testXXCompleted.py` programs, `XX` (soit `Processus`, soit `Analysis`)
Permet de vérifier qu'un dossier possède tous les fichiers qu'il devrait, càd que le programme à bien généner tous les fichiers qu'il aurait du.

### For `Processor`

```
python testProcessorCompleted.py -d path/to/directory
```
Prend en entré, un dossier (de résultat du programme `Processor`) est vérifie qu'il contient un fichier ROOT par numéro de processus (liste récupérer à partir du dossier `/gridgroup/ilc/nnhAnalysisFiles/AHCAL`).

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
Il va retourner les processus manquant sur le terminal et dans un fichier `testProcessorCompleted.txt`.

### For `Analysis`

De manière similiare,le programme `testAnalysisCompleted.py` prend le chemin d'un dossier et 
test si ce dossier contient tous les fichiers générés par le programme `analysis`.
```
python testAnalysisCompleted.py -d path/to/directory
```
Vérifie donc que le dossier `directory` a bien les fichiers suivants :
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
Là aussi, il retourne les noms des dossiers manquants sur le terminal et dans un fichier `testAnalysisCompleted.txt`.

## `testXXSame2.py` programs
Regarde, suivant la définition de Kolmogorov, si tous les fichiers de résultats sont identiques (néglige s'il manque des fichiers).

### For `Processor`
Tous les processors doivent être identiques.
```
python testProcessorSame.py -d1 path/to/directory1 -d2 path/to/directory2
```

### For `Analysis`
Les résultats peuvent être légèrement différents mais doivent rester équivalent.
```
python testAnalysisSame.py -d1 path/to/directory1 -d2 path/to/directory2
```

| Programme | Temps d'exécution sur server | 
| --- | --- |
| `testProcessorCompleted.py` | quelques seconds |
| `testAnalysisCompleted.py` | quelques seconds |
| `testProcessorSame.py` | 1h30 |
| `testAnalysisSame.py` | 3h |
