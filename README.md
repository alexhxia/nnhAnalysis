# nnhAnalysis
Stage M2 IP2I (CNRS), groupe FCC (CMS)

Dans ce dépots, le programme permet l'analyse des cannaux :

- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

# La Branche `original`
Dans cette branche, on ajoute des petites corrections aux fichiers originaux de `ggarillot` afin de s'adapter à un stockage local.
## importer 
Pour importer directement de github :
```
git clone --branch original https://github.com/alexhxia/nnhAnalysis.git
```
On considère le dossier que nous venons de créer comme le `NNH_HOME` de notre projet :
```
export NNH_HOME=nnhAnalysis
```
On se place dans le dossier créer :
```
cd nnhAnalysis
```
Pour exécuter ce code, on a aussi besoin de préparer l'environnement grâce à :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
## miniDSTMaker
Comme on utilise les données en local, on a pas besoin du dossier `miniDSTMAKER` :
```
rm -R miniDSTMaker
```
NB : d'autres modifications sont apportées à l'intérieur des dossiers `processor` et `analysis`.
# Rappel : le Projet initial
Ce projet est basé sur le travaille de `ggarillot`. Pour importer directement de son github :
```
git clone --branch refactor https://github.com/ggarillot/nnhAnalysis.git
```
