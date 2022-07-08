#!/bin/bash

# This program run BDT.
#
# BEFORE : run processor and prepareForBDT part
# INPUT: 
#   * 1 DATA.root file
#   * 4 split_XX_eXX_pXX.root files
# OUTPUT: 
#   * 2 model_XX_eXX_pXX.joblib files
#   * 2 score_XX_eXX_pXX.root files
#   * 2 bestSelection_XX_eXX_pXX.root files
#   * 2 stats_XX_eXX_pXX.json files

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
    echo "ENTRY: DATA.root files"
    echo " - 1 DATA.root file"
    echo " - 4 split_XX_eXX_pXX.root files"
    echo
    echo "RETURN: analysis stat files"
    echo " - 2 model_XX_eXX_pXX.joblib files"
    echo " - 2 score_XX_eXX_pXX.root files"
    echo " - 2 bestSelection_XX_eXX_pXX.root files"
    echo " - 2 stats_XX_eXX_pXX.json"
    echo
    echo 'SYNTAX:'
    echo '    ./launchBDT.sh [options]'
    echo
    
    syntaxOption h b n i #help.sh
}

# ENVIRONMENT + in export.sh ###

conda=0

## option choice by user
while getopts hdn:b:i: flag ; do
    case "${flag}" in 
    
        h)  syntax 
            exit 0;;
                    
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

particles=("bb" "WW")
for p in ${particles[@]}; do
    echo "    launch launchBDT_$p"
    python3 launchBDT_$p.py #-i $NNH_ANALYSIS_INPUT #\
            # 1> $NNH_ANALYSIS_INPUT/launchBDT_$p.out \
            # 2> $NNH_ANALYSIS_INPUT/launchBDT_$p.err 
done 

echo
echo "--> END : launchBDT ($branch) <--"
echo
