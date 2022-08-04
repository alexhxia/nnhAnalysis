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

tab="        "
echo
echo "$tab""        Start launchBDT..."
echo

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
    echo "INPUT: DATA.root files"
    echo " - 1 DATA.root file"
    echo " - 4 split_XX_eXX_pXX.root files"
    echo
    echo "OUTPUT: analysis stat files"
    echo " - 2 model_XX_eXX_pXX.joblib files"
    echo " - 2 score_XX_eXX_pXX.root files"
    echo " - 2 bestSelection_XX_eXX_pXX.root files"
    echo " - 2 stats_XX_eXX_pXX.json"
    echo
    echo 'SYNTAX:'
    echo '    ./launchBDT.sh [options]'
    echo
    
    syntaxOption h v b n q #help.sh
}

# ENVIRONMENT + in export.sh ###

## option choice by user
while getopts hvb:n:q: flag ; do
    case "${flag}" in 
    
        h)  syntax 
            exit 0;;
            
        v)  verbose=0;;
                    
        n)  setPath ${OPTARG};;
            
        b)  setBranch ${OPTARG};;
        
        q)  setAnalysisOutput ${OPTARG};;
                    
        *)  error '     Option no exist';;
            
    esac
done 

## update environment

nnh_export # export.sh
if [ $verbose -eq 0 ]; then
    print_export # export.sh
fi

## test environment

test_isValidHome

# RUN 

echo
echo "$tab""--> Run: launchBDT_XX.py ($branch) <--"
echo

cd $NNH_HOME/analysis/python

particles=("bb" "WW")
for p in ${particles[@]}; do
    echo "$tab""--> Run: launchBDT_$p"
    
    if [ $verbose -eq 0 ]; then
        if [ "$branch" == "original" ]; then 
            # can't change NNH_ANALYSIS_INPUT for original branch
            python3 launchBDT_$p.py 
        else 
            python3 launchBDT_$p.py $NNH_ANALYSIS_OUTPUT
        fi
    else
        if [ "$branch" == "original" ]; then 
            # can't change NNH_ANALYSIS_INPUT for original branch
            python3 launchBDT_$p.py \
                    1> $NNH_ANALYSIS_OUTPUT/launchBDT_$p.out \
                    2> $NNH_ANALYSIS_OUTPUT/launchBDT_$p.err 
        else 
            python3 launchBDT_$p.py $NNH_ANALYSIS_OUTPUT \
                    1> $NNH_ANALYSIS_OUTPUT/launchBDT_$p.out \
                    2> $NNH_ANALYSIS_OUTPUT/launchBDT_$p.err 
        fi
    fi
    echo
done 

echo
echo "$tab""... End launchBDT ($branch)"
echo
