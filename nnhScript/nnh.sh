#!/bin/bash

# This program build and run all project:
#   * processus
#   * analysis
#       -> prepareForBDT
#       -> launchBDT_XX.py with XX = {WW, bb}
#
# INPUT: directory with processus directory with LCIO files
# OUTPUT: 
#   * processus
#       -> 402001.root
#       -> 402002.root
#       -> 402003.root
#       -> ...
#   * analysis
#       -> prepareForBDT
#           * DATA.root
#           * split_XX_eXX_pXX.root
#           * model_XX_eXX_pXX.root
#           * scrore_XX_eXX_pXX.root
#           * bestSelection_XX_eXX_pXX.root
#       -> launchBDT_XX.py with XX = {WW ou bb}
#           * stats_XX_eXX_pXX.json

### INCLUDE TOOL ###

source export.sh
source help.sh

### FUNCTION TOOL ###

# Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./nnh.sh [options]'
    echo
    
    syntaxOption h p t a b n i o #help.sh
}

### ENVIRONMENT + in export.sh ###

nb_runProcessor=1
nb_BDT=1
nb_runByBDT=1

# option choice by user
while  getopts ":b:p:a:t:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  branch=${OPTARG};;
        
        p)  nb_runProcessor=${OPTARG};;
        
        a)  nb_BDT=${OPTARG};;
        
        t)  nb_runByBDT=${OPTARG};;
        
        n)  home=${OPTARG};;
        
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

if [[ $nb_runProcessor -lt 0 ]] ; then
    error "-p: $nb_processor < 0"
fi

if [[ $nb_BDT -lt 0 ]] ; then
    error "-a: $nb_BDT < 0"
fi

if [[ $nb_runByBDT -lt 0 ]] ; then
    error "-t: $nb_runByBDT < 0"
fi

nnh_export # && print_export

test_isValidHome

echo
echo "Start nnh on the $branch branch with $nb_bdt BDT for each $nb_processor processors..."

### RUN ###

### processor
for ((p = 1; p <= $nb_runProcessor; p++)); do
    echo
    echo "    Start "$p"th processor:"
    
    # OUTPUT DIRECTORY IN SERVER 
    k=1
    outputDir=$NNH_OUTPUT/run_$k
    while [ -d $outputDir ]; do
        k=$((k + 1))
        outputDir=$NNH_OUTPUT/run_$k
    done 
    mkdir -v $outputDir \
             $outputDir/processor \
             $outputDir/analysis
    
    # RUN PROCESSOR
    if [ $p = 1 ]; then # build
        ./nnHProcessor.sh -n $home -b $branch -i $NNH_INPUT -c
    else
        ./nnhProcessor.sh -n $home -b $branch -i $NNH_INPUT
    fi 
    
    # MOVE OUTPUT PROCESSOR DATA IN SERVER 
    mv $NNH_PROCESSOR_OUTPUT/* $outputDir/processor
    rm -R $NNH_PROCESSOR_OUTPUT
    echo "    Output processor files save in $OUTPUT_DIRECTORY/processor"
    
    ### analysis 
    
    for ((a = 1; a <= $nb_BDT; a++)); do
    
        # prepareBDT
        echo
        echo "    Start "$a"th BDT at "$p"th processor: "
        outputDirA=$outputDir/analysis/run_"$k"_"$a"
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
done

echo
echo "...Terminate nnh"
echo
