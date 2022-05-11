# [`nnhScript`](.)
Ces programmes scripts bash cherchent à rendre l'utilisation des différents programmes plus simple pour l'utilisateur. 

## `nnhProcessus.sh`
C'est le premier programme à exécuter. Pour rappel, il traite les fichiers LCIO (ranger dans le dossier de leur numéro de processus) et permet d'obtenir un fichier ROOT par processus. Il s'agit du script :
```
./nnhProcessor.sh
```
Il prend les options : 'h', 'c', 'n', 'b', 'i'

Il recompile tout seul si la bibliotèque `$NNH_HOME/processor/lib/libnnhProcessor.so` n'existe pas.

Attention : avant de l'utiliser, sachez qu'il effacera tout le dossier `RESULTS` avant de s'exécuter.

## `nnhAnalysis.sh`
Une fois les fichiers root générer par `processus`, il faut entrainer la BDT avant de lancer l'a lancé. Il va donc exécuter 2 scripts :
```
./analysis_prepareBDT.sh
```
Qui prend les options : 'h', 'c', 'a', 'n', 'b', 'i', 'o'

Ensuite on peut analyser nos résultats grâce à :
```
./analysis_launchBDT.sh
```
Qui prend les options : 'h', 'n', 'b'
