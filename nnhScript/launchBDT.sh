#!/bin/bash

# This program run BDT.
#
# BEFORE : run processor and prepareForBDT part
# INPUT: directory with once root file by processus 
# OUTPUT: 
#   * stats_XX_eXX_pXX.json

# INCLUDE TOOL 

source tools/export.sh
source tools/help.sh
source tools/functions.sh

# FUNCTION TOOL 

## Display Help
function syntax {
    echo
    echo "Run 'launchBDT', ie '$NNH_HOME/analysis/bin/prepareBDT'."
    echo 
    echo "NB: input and output directories are same."
    echo
    echo "WARNING: delete last analysis output directory"
    echo
    echo "ENTRY: DATA.root files"
    echo
    echo "RETURN: analysis stat files"
    echo
    echo 'SYNTAX:'
    echo '    ./launchBDT.sh [options]'
    echo
    
    syntaxOption h d b n i #help.sh
}

# ENVIRONMENT + in export.sh ###

conda=0

## option choice by user
while getopts hdn:b:i: flag ; do
    case "${flag}" in 
    
        h)  syntax 
            exit 0;;
        
        d)  conda=1;;
            
        n)  setPath ${OPTARG};;
            
        b)  setBranch ${OPTARG};;
        
        i)  setAnalysisInput ${OPTARG};;
                    
        *)  error 'option no exist';;
            
    esac
done 

nnh_export
print_export

test_isValidHome

# RUN 

echo
echo "--> RUN : launchBDT ($branch) <--"
echo

cd $NNH_HOME/analysis/python

if [ $conda -eq 0 ]; then
    source ~/miniconda3/etc/profile.d/conda.sh
    conda activate env_root_python
    echo "    conda activate"
fi

particles=("bb" "WW")
for p in ${particles[@]}; do
    echo "    launch launchBDT_$p"
    python3 launchBDT_$p.py #-i $NNH_ANALYSIS_INPUT #\
            # 1> $NNH_ANALYSIS_INPUT/launchBDT_$p.out \
            # 2> $NNH_ANALYSIS_INPUT/launchBDT_$p.err 
done 

if [ $conda -eq 0 ]; then
    conda deactivate
    echo "    conda deactivate"
fi

echo
echo "--> END : launchBDT ($branch) <--"
echo
