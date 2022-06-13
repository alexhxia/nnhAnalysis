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
    echo "Run 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./nnhAnalysis.sh [options]'
    echo
    
    syntaxOption h b n i o #help.sh
}

# ENVIRONMENT + in export.sh 

## option choice by user
while  getopts ":b:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  setBranch ${OPTARG};;
                                
        n)  setHome ${OPTARG};;
                
        i)  a_input=${OPTARG};;
        
        o)  a_output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

## update environment

nnh_export && print_export

## test environment

test_isValidHome

# RUN

echo
echo "Start nnhAnalysis on the $branch branch..."

if [ -d $NNH_ANALYSIS_OUTPUT ]; then
    rm -R $NNH_ANALYSIS_OUTPUT
fi
mkdir $NNH_ANALYSIS_OUTPUT

## prepareBDT

echo "  -> Prepare BDT ..."
./prepareBDT.sh -n $home -b $branch -i $NNH_ANALYSIS_INPUT -c

## launchBDT

echo "  -> Launch  BDT..."
echo "$home, $branch"
./launchBDT.sh -n $home -b $branch

echo
echo "...Terminate nnhAnalysis"
echo
