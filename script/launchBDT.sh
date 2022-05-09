#!/bin/bash

### FUNCTION TOOL ###

source export.sh
source help.sh

# Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./launchBDT.sh [options]'
    echo
    
    syntaxOption h d b n  #help.sh
}

# PARAMETERS

# look environement parameters default in export.sh
conda=0

while getopts hdn:b: flag ; do
    case "${flag}" in 
    
        h)  syntax 
            exit 0;;
        
        d)  conda=1;;
            
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        *)  error 'option no exist';;
            
    esac
done 

test_isValidHome

# ENVIRONMENT

nnh_export
print_export
exit 0

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
echo "    launch launchBDT_bb "
python3 launchBDT_bb.py \
        1> $NNH_HOME/analysis/DATA/launchBDT_bb.out \
        2> $NNH_HOME/analysis/DATA/launchBDT_bb.err 
echo "    launch launchBDT_WW "
python3 launchBDT_WW.py 1> $NNH_HOME/analysis/DATA/launchBDT_WW.err 2> $NNH_HOME/analysis/DATA/launchBDT_WW.err 

if [ $conda -eq 0 ]; then
    conda deactivate
fi

echo
echo "--> END : launchBDT ($branch) <--"
echo
