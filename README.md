# nnhAnalysis
Stage M2 IP2I (CNRS), groupe FCC (CMS)

# github original
On importe la branche du github original
```
git clone --branch refactor https://github.com/ggarillot/nnhAnalysis.git
```
On se place dans le dossier créer : que l'on considèrera comme le dossier `NNH_HOME` de notre projet :
```
export NNH_HOME=nnhAnalysis
```
```
cd nnhAnalysis
```
Comme on utilise les données en local, on a pas besoin du dossier `miniDSTMAKER` :
```
rm -R miniDSTMaker
```
On construit un dossier `ROOT` pour les fichiers ROOTs qui seront généré à la fin de la partie `processor` et un second dossier `DATA` qui sera regroupera ceux qui processus `analysis` :
```
mkdir DATA ROOT
```
Ou 
```
mkdir $NNH_HOME/DATA $NNH_HOME/ROOT
```
