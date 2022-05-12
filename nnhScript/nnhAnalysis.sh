#!/bin/bash

# This program build and run all analysis part !
#   * prepareForBDT
#   * launchBDT_XX.py with XX = {WW, bb}
#
# INPUT: directory with processus root
#   * 402001.root
#   * 402002.root
#   * 402003.root
#   * ...
# OUTPUT:
#   * prepareForBDT
#       -> DATA.root
#       -> split_XX_eXX_pXX.root
#       -> model_XX_eXX_pXX.root
#       -> scrore_XX_eXX_pXX.root
#       -> bestSelection_XX_eXX_pXX.root
#   * launchBDT_XX.py with XX = {WW ou bb}
#       -> stats_XX_eXX_pXX.json

### INCLUDE TOOL ###

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

### FUNCTION TOOL ###

# Display Help
function syntax {
    echo
    echo "Run 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./nnhAnalysis.sh [options]'
    echo
    
    syntaxOption h d p a b n i o #help.sh
}

### ENVIRONMENT + in export.sh ###

num_processus=1
nb_BDT=1
nb_runByBDT=1

# option choice by user
while  getopts ":b:a:d:n:i:p:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  branch=${OPTARG};;
                
        d)  nb_BDT=${OPTARG};;
        
        a)  nb_runByBDT=${OPTARG};;
        
        n)  home=${OPTARG};;
        
        p)  num_processus=${OPTARG};;
        
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

if [[ $nb_BDT -lt 0 ]] ; then
    error "-p: $nb_processor < 0"
fi

if [[ $nb_runByBDT -lt 0 ]] ; then
    error "-a: $nb_bdt < 0"
fi

outputDir=$NNH_OUTPUT/run_$num_processus
if ! [[ -d $outputDir ]]; then
    error "-p : processor number no exist"
elif ! [[ -d $outputDir/processor ]]; then
    error "-p : processor directory no exist"
fi

nnh_export # && print_export

test_isValidHome

echo
echo "Start nnh on the $branch branch with $nb_bdt BDT for "$num_processus"th processors..."
cd $script

### RUN ###

k=1

if ! [[ -d $outputDir/analysis ]]; then
    mkdir -v $outputDir/analysis
else
    outputDirA=$outputDir/analysis/run_"$num_processus"_"$k"
    while [ -d outputDirA ]; do
        k=$((k + 1))
        outputDirA=$outputDir/analysis/run_"$num_processus"_"$k"
    done 
fi

for ((a = 1; a <= $nb_BDT; a++)); do

    # prepareBDT
    echo
    echo "    Start "$a"th BDT at "$num_processus"th processor: "
    outputDirA=$outputDir/analysis/run_"$num_processus"_"$k"
    mkdir -v $outputDirA
    
    echo "      -> Prepare BDT: ..."
    ./prepareBDT.sh -n $NNH_HOME -b $branch -i $outputDir/processor -o $NNH_ANALYSIS_OUTPUT -c
    cp $NNH_ANALYSIS_OUTPUT $outputDirA
    
    # launchBDT
    for ((t = 1; t <= $nb_BDT; t++)); do
        echo "      -> Launch  BDT: "$t"..."
        ./launchBDT.sh -n $NNH_HOME -b $branch
        outputDirAT=outputDirA/run_"$k"_"$a"_"$t"
        mkdir -v $outputDirAT
        cp $NNH_ANALYSIS_OUTPUT/* $outputDirAT
        echo "      -> Save result in $outputDirAT"
    done 
    rm -R $NNH_ANALYSIS_OUTPUT
    
    mv $NNH_ANALYSIS_OUTPUT/* $outDir
    echo "      -> Save result in $outputDirA"
    rm -R $NNH_ANALYSIS_OUTPUT
done


echo
echo "...Terminate nnh"
echo
