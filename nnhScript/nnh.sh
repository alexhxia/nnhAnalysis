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

# INCLUDE TOOL 

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

# FUNCTION TOOL 

## Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo "WARNING: processor and analysis output directories deleted"
    echo
    echo "ENTRY: lcio files in processor number directories"
    echo
    echo "RETURN: statistic analysis files"
    echo
    echo 'SYNTAX:'
    echo '    ./nnh.sh [options]'
    echo
    
    syntaxOption h b p a n i o # help.sh
}

# ENVIRONMENT + in export.sh 

nb_processor=1
nb_BDT=1

## option choice by user
while  getopts ":b:p:a:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  setBranch ${OPTARG};;
        
        p)  nb_processor=${OPTARG};;
        
        a)  nb_BDT=${OPTARG};;
                
        n)  setPath ${OPTARG};;
        
        i)  setServerInput ${OPTARG};;
        
        o)  setServerOutput ${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

## Update env

## Test env

nnh_export && print_export

test_isValidHome

if [[ $nb_processor -lt 0 ]] ; then
    error "-p: $nb_processor < 0"
fi

if [[ $nb_BDT -lt 0 ]] ; then
    error "-a: $nb_BDT < 0"
fi

# RUN 

echo
echo "Start nnh on the $branch branch with $nb_BDT BDT for each $nb_processor processors..."

## processor
serverOutputDir=$NNH_INPUT/$branch
for ((p = 1; p <= $nb_processor; p++)); do
    echo
    echo "    Start "$p"th processor:"
    
    ### OUTPUT DIRECTORY IN SERVER 
    k=1
    serverOutputDir=$serverOutputDir/run_$k
    while [ -d $serverOutputDir ]; do
        k=$((k + 1))
        serverOutputDir=$serverOutputDir/run_$k
    done 
    
    mkdir -v $serverOutputDir \
             $serverOutputDir/processor \
             $serverOutputDir/analysis
    
    setProcessorOutput $serverOutputDir/processor # export.sh: a_input change too
    nnh_export # export.sh: export update
    
    ### RUN PROCESSOR
    if [ $p = 1 ]; then # build
        ./nnhProcessor.sh -n $path -b $branch -i $NNH_PROCESSOR_INPUT -o $NNH_PROCESSOR_OUTPUT -c
    else
        ./nnhProcessor.sh -n $path -b $branch -i $NNH_PROCESSOR_INPUT -o $NNH_PROCESSOR_OUTPUT
    fi 
    
    ### analysis 
    for ((a = 1; a <= $nb_BDT; a++)); do
        setAnalysisOutput $serverOutputDir/analysis/run_$a # export.sh
        nnh_export # export.sh : export update
        mkdir -v $NNH_ANALYSIS_OUTPUT
        ./nnhAnalysis -c -n $path -b $branch -i $NNH_ANALYSIS_INPUT -o $NNH_ANALYSIS_OUTPUT
    done
done

echo
echo "...Terminate nnh"
echo
