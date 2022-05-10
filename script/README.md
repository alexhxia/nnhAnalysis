# `script`

Ces programmes scripts bash cherchent à rendre l'utilisation des différents programmes plus simple pour l'utilisateur. 
Le but étant l'analyse des cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

## Fonctionnement général du projet 

### Données initiales
Les données LCIO sont stockées localement dans le dossier : `/gridgroup/ilc/nnhAnalysisFiles/AHCAL`.
Puis chaque fichier est trié dans des sous-dossiers en fonction de leur numéro de processus.
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
### [`nnhHome`](../nnhHome) directory 
Il contient les différents branches. Ce que j'appelle 'branche' correspond à un nom de projet : `ìlcsoft`, `fcc`... Sachant que ces projets numériques sont idépendants les uns, les autres. C'est pourquoi chacune doit au moins contenir un dossier `processus` et `analysis`, qui contient les codes du programmes.

### [`nnhScript`](.)

#### [`tools`](tools) directory

Afin de pouvoir modifier le code plus facilement et de limiter la redondance du code, j'ai développé plusieurs scripts outils.
Pour ajouter toutes les fonctions outils, ajouter dans votre script bash :
```
source tools/function.sh
source tools/export.sh 
source tools/help.sh
```

##### `export.sh` script
`export.sh` stocke les variables de chemin vers les dossiers utilisés par les autres scripts :
- `home` le chemin vers le dossier `nnhHome`
- `branche` le nom de la branche
- `server` le chemin vers le server, où sont stockés les données du projet
- `input` le chemin sur le server, où sont stockés les données initiales du projet
- `output` le chemin sur le server, où sont stockés les résultats
- `p_input` le chemin où le programme `processus` va trouver les dossiers de numéro de processus qui contiennent les fichiers LCIO 
- `p_output` le chemin où le programme `processus` va stocker les fichiers ROOT qui aura générer (un par dossier)
- `a_input` le chemin où le programme `analysis` va prendre les fichiers ROOT générés en sorti par un `processus`
- `a_output` le chemin où le programme `analysis` va stocker les fichiers qui aura générer 

Grâce à la méthode `nnh_export`, ce script permet de mettre à jour les variables d'environnement qui sont :
- `NNH_HOME` (avec la méthode `test_isValidHome` vous pouvez vérifier si votre dossier est conforme à ce qui est attendu
- pour l'entré-sorti sur le server : `NNH_INPUT`, `NNH_OUTPUT`
- pour l'entré-sorti du programme `processor` : `NNH_PROCESSOR_INPUT`, `NNH_PROCESSOR_OUTPUT`
- pour l'entré-sorti du programme `analysis` : `NNH_ANALYSIS_INPUT`, `NNH_ANALYSIS_OUTPUT` 

Vous pouvez les afficher avec la méthode `print_export`.

##### `help.sh` script
`help.sh` permet l'affichage des options qui sont les mêmes des tous les scripts, grâce à la méthode `syntaxOption`.

Donc les scripts de cette partie peuvent prendre des options afin de personaliser les programmes à vos besoins :
- 'h' : évidemment pour "help". Cette option détaille le fonctionnement de chaque script.
- 'c' : "compile", si vous souhaitez compiler le programme avant de l'exécuter. Par défault, la compilation se fera s'il manque les fichiers nécessaires à l'éxecution du programme.
- 'd' : désactive conda si vous n'en avez pas besoin pour avoir python 3 ou ROOT.
- 'n' : prend en entrée le chemin du dossier "nnhHome" qui contient les branches du programme. Par défault : `~/nnhAnalysis/nnhHome`, cad si le dossier a été importer dans votre répertoire personnel.
- 'b' : "branch", le nom de la branche que vous souhaitez exécuter, par défault `original`.
- 'i' : "inputDirectory", le chemin vers les fichiers ou dossier d'entrée. Par défault :
  - `/gridgroup/ilc/nnhAnalysisFiles/AHCAL` pour [`processor.sh`](processor.sh)
  - `home/branch/processor/RESULTS` pour [`prepareBDT.sh`](prepareBDT.sh)
- 'o' : "outputDirectory", le chemin vers le dossier de sortie du programme.
  - `home/branch/processor/RESULTS` pour [`processor.sh`](processor.sh)
  - `/gridgroup/ilc/nnhAnalysisFiles/result/branch/run?/analysis/run??` pour [`prepareBDT.sh`](prepareBDT.sh) et [`launchBDT.sh`](launchBDT.sh) 

##### `function.sh` script
Regroupe des méthodes utiles :
- `error msg` qui affiche le message d'error passer en paramètres et arrête le programme avec 1

#### `nnhProcessus.sh`
C'est le premier programme à exécuter. Pour rappel, il traite les fichiers LCIO (ranger dans le dossier de leur numéro de processus) et permet d'obtenir un fichier ROOT par processus. Il s'agit du script :
```
./nnhProcessor.sh
```
Il prend les options : 'h', 'c', 'n', 'b', 'i'

Il recompile tout seul si la bibliotèque `$NNH_HOME/processor/lib/libnnhProcessor.so` n'existe pas.

Attention : avant de l'utiliser, sachez qu'il effacera tout le dossier `RESULTS` avant de s'exécuter.

#### `nnhAnalysis.sh`
Une fois les fichier root générer par `processus`, il faut entrainer la BDT avant de lancer l'a lancé. Il va donc exécuter 2 scripts :
```
./analysis_prepareBDT.sh
```
Qui prend les options : 'h', 'c', 'a', 'n', 'b', 'i', 'o'

Ensuite on peut analyser nos résultats grâce à :
```
./analysis_launchBDT.sh
```
Qui prend les options : 'h', 'n', 'b'
