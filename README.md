# nnhAnalysis
Stage M2 IP2I (CNRS), groupe FCC (CMS)

Dans ce dépots, le programme permet l'analyse des cannaux :

- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

# Le Projet initial
Ce projet est basé sur le travaille de `ggarillot`. Pour importer directement de son github :
```
git clone --branch refactor https://github.com/ggarillot/nnhAnalysis.git
```
On se place dans le dossier créer, que l'on considèrera comme le dossier `NNH_HOME` de notre projet :
```
export NNH_HOME=nnhAnalysis
```
```
cd nnhAnalysis
```
Pour exécuter ce code, on a aussi besoin de préparer l'environnement grâce à :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```

# Le Projet personnel
J'ai divisé ce dépot en 3 branches :
- `original` qui regroupe les fichiers originaux de `ggarillot` avec de petites modifications pour s'adapter à un stockage local.
- `ilcsoft` amélioration du code `original`
- `fcc` qui s'adapte aux nouvelles contraintes du projet FCC du CERN.
