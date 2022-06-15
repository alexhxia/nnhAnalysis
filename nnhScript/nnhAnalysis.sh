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
#   * launchBDT_XX.py with XX = {WW ou bb}
#       -> stats_XX_eXX_pXX.json
#       -> model_XX_eXX_pXX.root
#       -> scrore_XX_eXX_pXX.root
#       -> bestSelection_XX_eXX_pXX.root

# INCLUDE TOOL 

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

# FUNCTION TOOL

## Display Help
function syntax {
    echo
    echo "Run 'nnhAnalysis': ie 'prepareBDT' and 'launchBDT'."
    echo
    echo "WARNING: delete analysis output last directory"
    echo
    echo "ENTRY: root files create by processor"
    echo
    echo "RETURN: statistic analysis files"
    echo
    echo 'SYNTAX:'
    echo '    ./nnhAnalysis.sh [options]'
    echo
    
    syntaxOption h c b n i o #help.sh
}

# ENVIRONMENT + in export.sh 

recompile=1 # no build

## option choice by user
while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
            
        c)  recompile=0;;
            
        b)  setBranch ${OPTARG};;
                                
        n)  setPath ${OPTARG};;
                
        i)  setAnalysisInput ${OPTARG};;
        
        o)  setAnalysisOutput ${OPTARG};;
        
        *) error '    Option no exist';;
    esac
done 

## update environment

nnh_export
print_export

## test environment

test_isValidHome

# RUN

echo
echo "Start nnhAnalysis on the $branch branch..."
echo "here $NNH_ANALYSIS_OUTPUT"
if [ -d $NNH_ANALYSIS_OUTPUT ]; then
    rm -R $NNH_ANALYSIS_OUTPUT
fi
mkdir $NNH_ANALYSIS_OUTPUT

## prepareBDT

echo "  -> Prepare BDT ..."
if [ $recompile -eq 0 ]; then
    ./prepareBDT.sh -c \
            -n $path \
            -b $branch \
            -i $NNH_ANALYSIS_INPUT \
            -o $NNH_ANALYSIS_OUTPUT
else 
    ./prepareBDT.sh \
            -n $path \
            -b $branch \
            -i $NNH_ANALYSIS_INPUT \
            -o $NNH_ANALYSIS_OUTPUT
fi 

## launchBDT

echo "  -> Launch  BDT..."
./launchBDT.sh -n $path -b $branch -i $NNH_ANALYSIS_INPUT

echo
echo "...Terminate nnhAnalysis"
echo
