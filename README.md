# nnhAnalysis
Stage M2 IP2I (CNRS), groupe FCC (CMS)


Dans ce dépots, le programme permet l'analyse des cannaux :

- e+e- &rarr; &nu;&nu;h (h &rarr; WW &rarr; qqqq)
- e+e- &rarr; &nu;&nu;h (h &rarr; b bbar)

Dans cette branche, on place le code original développer par ggarillot, que l'on peut retrouver dans la branche `refactor` sur :
https://github.com/ggarillot/nnhAnalysis.git

# github original
Pour importer le programme original :
```
git clone --branch refactor https://github.com/ggarillot/nnhAnalysis.git
```

# environnement
Afin de pouvoir l'utiliser, on a besoin d'exécuter :
```
source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
```
Ainsi que :
```
export NNH_HOME=/path/to/nnhAnalysis
```
Ou, juste après la commande `git clone`
```
export NNH_HOME=nnhAnalysis
```
