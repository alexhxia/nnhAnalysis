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
    
    syntaxOption h d b n i o #help.sh
}

### ENVIRONMENT + in export.sh ###

nb_BDT=1
nb_runByBDT=1

# option choice by user
while  getopts ":b:t:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  branch=${OPTARG};;
                
        t)  nb_runByBDT=${OPTARG};;
                
        n)  home=${OPTARG};;
                
        i)  a_input=${OPTARG};;
        
        o)  a_output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

if [[ $nb_BDT -lt 0 ]] ; then
    error "-a: $nb_BDT < 0"
fi

if [[ $nb_runByBDT -lt 0 ]] ; then
    error "-t: $nb_runByBDT < 0"
fi

nnh_export && print_export

test_isValidHome

echo
echo "Start nnhAnalysis on the $branch branch with $nb_bdt BDT..."
cd $script

### RUN ###

if [ -d $NNH_ANALYSIS_OUTPUT ]; then
    rm -R $NNH_ANALYSIS_OUTPUT
fi

mkdir $NNH_ANALYSIS_OUTPUT

# prepareBDT
echo
echo "    Start BDT: "

echo "      -> Prepare BDT: ..."
./prepareBDT.sh -n $NNH_HOME -b $branch -i$NNH_ANALYSIS_INPUT -o $NNH_ANALYSIS_OUTPUT -c

# launchBDT
for ((t = 1; t <= $nb_BDT; t++)); do
    echo "      -> Launch  BDT: "$t"..."
    ./launchBDT.sh -n $NNH_HOME -b $branch
    
    mkdir run_$t
    cp $NNH_ANALYSIS_OUTPUT/* run_$t
done 

echo
echo "...Terminate nnh"
echo
