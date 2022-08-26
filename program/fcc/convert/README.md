# `convert`

Converti les fichiers LCIO en EDM4hep.

## `k4MarlinWrapper`
Le projet source est : https://github.com/key4hep/k4MarlinWrapper.
```
git clone https://github.com/key4hep/k4MarlinWrapper.git
```

Attention, le projet est en cours de développement (en juin 2022) donc si vous 
actualisez le code, des erreurs pourront survenir.

### Configuring, compiling and installing

```
source /cvmfs/sw.hsf.org/key4hep/setup.sh
source /cvmfs/sw.hsf.org/spackages6/key4hep-stack/2022-06-09/x86_64-centos7-gcc11.2.0-opt/pkhfu/setup.sh
```
```
source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh
source /cvmfs/sw-nightlies.hsf.org/spackages5/key4hep-stack/master-2022-06-21/x86_64-centos7-gcc11.2.0-opt/qvsnn/setup.sh
```
```
source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh
```
```
cd k4MarlinWrapper
```
```
mkdir build install; cd build
```
```
cmake ..
```
```
cmake -DCMAKE_INSTALL_PREFIX=../install ..
```
```
make -j 4
```
```
make install
```

### Running

Téléchargement d'un fichier d'exemple :
```
wget https://github.com/AIDASoft/DD4hep/blob/master/DDTest/inputFiles/muons.slcio
```

Placer ce fichier dans le bon dossier : 
```
mv muons.slcio ../test/inputFiles
```

Lancer le processor de `k4MarlinWrapper` avec un fichier d'entré LCIO et EDM4hep  :
```
k4run ../k4MarlinWrapper/examples/runit.py
```

### Format

```
find . -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;
```

### Testing and examples

Affiche les tests possibles :
```
ctest -N
```

Lance tous les tests :
```
ctest
```

Lancer un test spécifique :
```
ctest --verbose -R edm_converters
```
```
```
```
```
```
```
