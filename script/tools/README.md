# [`tools`](tools) directory

Ces petits scripts batch sont des outils de factorisation de code (limite la redondance de code et donc permet de modifier plus facilement).

Pour utiliser toutes ces fonctions outils, il suffit ajouter dans votre script bash :
```
source tools/function.sh
source tools/export.sh 
source tools/help.sh
```

## `export.sh` script
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

## `help.sh` script
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
  - `/gridgroup/ilc/nnhAnalysisFiles/result/branch/run_?/analysis/run_?` pour [`prepareBDT.sh`](prepareBDT.sh) et [`launchBDT.sh`](launchBDT.sh) 
- 'p' : "processus", le nombre de fois que vous souhaitez exécuter `processus`, par défault 1.
- 'a' : "analysis", le nombre de fois que vous souhaitez exécuter `prepareBDT` par `processus`, par défault 1.
- 't' : le nombre de fois que vous souhaitez exécuter `launchBDT` par `analysis`, par défault 1.

## `function.sh` script
Regroupe des méthodes utiles :
- `error msg` qui affiche le message d'error passer en paramètres et arrête le programme avec 1
