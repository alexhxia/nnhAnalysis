# `nnhTest` directory

Ce dossier regroupe toutes les fonctions qui vont permettent de tester les résultats d'un programme de [`nnhHome`](../nnhHome).

## `testXXCompleted.py` programs, `XX` (soit `Processus`, soit `Analysis`)
Permet de vérifier qu'un dossier possède tous les fichiers qu'il devrait, càd que le programme à bien généner tous les fichiers qu'il aurait du.

### For `Processor`

```
python testProcessorCompleted.py -p /path/to/directory -s /path/to/directory -o /path/to/outpoutFile.json
```
Prend en entré, un dossier (de résultat d'un programme `processor`) est vérifie qu'il contient un fichier ROOT par numéro de processus.

Ce programme peut prendre 3 paramètres :
- `-p` ou `--processor`  suivi du chemin qui mène au dossier que l'on souhaite tester, le paramètre est obligatoire.
- `-o` ou `--output` suivie d'un chemin avec le nom d'un fichier JSON qui contiendra le résultat du test. Le paramètre étant facultatif, si l'option n'est pas utilisée alors il n'y aura pas de stockage fichier.
- `-s` ou `--server` suivi du chemin vers le dossier qui contient les dossiers initaux du processus. Le paramètre est aussi facultatif, sinon il récupére la liste à partir du dossier `/gridgroup/ilc/nnhAnalysisFiles/AHCAL`. Mais si le dossier n'est pas trouvé alors il utlise la liste des processus suivants (qui peut ne pas être adapter à votre processus) :
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
Dans tous les cas, il retournera le résultat sur la sortie standard.

### For `Analysis`

```
python testAnalysisCompleted.py -a /path/to/directory -o /path/to/outpoutFile.json
```

De manière similiare, le programme `testAnalysisCompleted.py` prend le chemin d'un dossier et 
test si ce dossier contient tous les fichiers générés par le programme `analysis`.

Ce programme peut prendre 2 paramètres :
- `-a` ou `--analysis`  suivi du chemin qui mène au dossier que l'on souhaite tester, le paramètre est obligatoire.
- `-o` ou `--output` suivie d'un chemin avec le nom d'un fichier JSON qui contiendra le résultat du test. Le paramètre étant facultatif, si l'option n'est pas utilisée alors il n'y aura pas de stockage fichier.

Plus précisemment il vérifie que le dossier a bien les fichiers suivants :
- générer par la commande `hadd` : 
```
"DATA.root"
```
- par le programme `prepareBDT` (with XX for ww or bb):
```
"split_XX_e+0_p+0.root",
"split_XX_e-0.8_p+0.3.root",
```
- et par `launchBDT` (with XX for ww or bb):
```
# files created 
"bestSelection_XX_e-0.8_p+0.3.root", 
"scores_XX_e-0.8_p+0.3.root",
"stats_XX_e-0.8_p+0.3.json",
"model_XX_e-0.8_p+0.3.joblib"
```
Là aussi, il retourne les noms des dossiers manquants sur le terminal.

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
| :---: | :---: |
| `testProcessorCompleted.py` | quelques seconds |
| `testAnalysisCompleted.py` | quelques seconds |
| `testProcessorSame.py` | 1h30 |
| `testAnalysisSame.py` | 3h |
