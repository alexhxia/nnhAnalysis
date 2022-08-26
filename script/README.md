# [`nnhScript`](.)
Ces programmes scripts bash cherchent à rendre l'utilisation des différents programmes plus simple pour l'utilisateur. 

## `nnhProcessus.sh`
C'est le premier programme à exécuter. Pour rappel, il traite des fichiers LCIO (ranger dans le dossier de leur numéro de processus) afin d'obtenir un fichier ROOT par processus. Pour cela, il faut exécuter :
```
./nnhProcessor.sh
```
Il prend les options : 'h', 'c', 'n', 'b', 'i' (cf [help options](tools/README.md)).

Il recompile tout seul si la bibliotèque `$NNH_HOME/processor/lib/libnnhProcessor.so` n'existe pas.

Attention : il effacera le dossier `RESULTS` s'il en existe déjà un.

## `nnhAnalysis.sh`
Une fois les fichiers ROOTs générés par `processus`, on peut exécuter `analysis` avec les nouveaux fichiers en entré :
```
./nnhAnalysis.sh
```
Qui prend les options : 'h', 'd', 'p', 'a', 'b', 'n', 'i', 'o' (cf [help options](tools/README.md)).

Ce programme entrainer la BDT avant de lancer l'analyse. 

Il va donc exécuter en premier :
```
./prepareBDT.sh
```
Qui prend les options : 'h', 'c', 'a', 'n', 'b', 'i', 'o' (cf [help options](tools/README.md)).

Ensuite on peut analyser nos résultats grâce à :
```
./launchBDT.sh
```
Qui prend les options : 'h', 'n', 'b' (cf [help options](tools/README.md)).

C'est 2 programmes peuvent être exécuter indépendamment.

## `nnh.sh`

Ce dernier programme est va exécuter l'ensemble de ces autres programmes : `nnhProcessor` et `nnhAnalysis`.
```
./nnh.sh
```
Qui prend les options : 'h', 'p', 't', 'a', 'b', 'n', 'i', 'o' (cf [help options](tools/README.md)).
