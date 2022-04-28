# `script`

C'est programme cherche à rendre l'utilisation des différents programme plus simple pour l'utilisateur. Le but étant l'analyse des cannaux :
- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

## Fonctionnement généraux des branches 

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
### Branches
Ce qui est appelé 'branche' correspond au type de projet que vous souhaitez exécuter. Elle sont contenue dans le dossier [nnhHome](../nnhHome). Chacune contient un dossier `processus` et `analysis`.

### Run

### Avant propos

Les scripts de cette partie peuvent prendre des options afin de personaliser les programmes à vos besoins :
- 'h' : évidemment pour "help". Cette option détaille le fonctionnement de chaque script.
- 'c' : "compile", si vous souhaitez juste compiler le programme sans l'exécuter. Par défault, la compilation se fera s'il manque des dossiers ou de fichiers nécessaires à l'éxecution du programme.
- 'a' : "all", permet de compiler et d'executer le programme.
- 'n' : "nnhHome", prend en entrée le chemin du dossier qui contient les branches du programme. Par défault : `~/nnhAnalysis/nnhHome`, cad sans modification après importation du dossier dans votre répertoire personnel.
- 'b' : "branch", le nom de la branche que vous souhaitez exécuter.
- 'i' : "inputDirectory", le chemin vers les fichiers ou dossier d'entrée. Par défault :
  - `/gridgroup/ilc/nnhAnalysisFiles/AHCAL` pour [`processor.sh`](processor.sh)
  - `home/branch/processor/RESULTS` pour [`prepareBDT.sh`](prepareBDT.sh)
- 'o' : "outputDirectory", le chemin vers le dossier de sortie du programme.
  - `home/branch/processor/RESULTS` pour [`processor.sh`](processor.sh)
  - `/gridgroup/ilc/nnhAnalysisFiles/result/branch/run?/analysis/run??` pour [`prepareBDT.sh`](prepareBDT.sh) et [`launchBDT.sh`](launchBDT.sh) 

### `processus`
C'est le premier programme à exécuter. En effet, il traite les fichiers LCIO (ranger dans le dossier de leur processus) et permet d'obtenir un fichier ROOT par processus. Il s'agit du script :
```
./processor.sh
```
Il prend les options : 'h', 'c', 'a', 'n', 'b', 'i'

### `analysis`
Une fois les fichier root générer par `processus`, il faut entrainer la BDT.
```
./analysis_prepareBDT.sh
```
Qui prend les options : 'h', 'c', 'a', 'n', 'b', 'i', 'o'

Ensuite on peut analyser nos résultats grâce à :
```
./analysis_launchBDT.sh
```
Qui prend les options : 'h', 'n', 'b'
