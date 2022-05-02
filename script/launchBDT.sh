#!/bin/bash

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo
}

# Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./launchBDT.sh [options]'
    echo
    echo 'OPTIONS:'
    echo '   -h                print help'
    echo
    echo '   -d                deactivate conda'
    echo
    echo '   -b [name]         branch'
    echo "                     DEFAULT VALUE: $branch"
    echo
    echo '   -n [directory]    nnhAnalysis directory'
    echo "                     DEFAULT VALUE: $home"
    echo
    
}

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    syntax
    exit 1
}

# Test if name project exist
function test_isValidBranch {
    valid=false
    for b in "${branchsValid[@]}" ; do
        if [ $b == $1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-b $branch"
    fi
}

# Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    #elif ! [ -f $home/$branch/analysis/DATA/data.root ]; then 
        #error '-n: home/branch/analysis/DATA/data.root file no exist (prepare BDT first)'
    fi
}

# PARAMETERS

valid_branchs=(original ilcsoft fcc)
home=~/nnhAnalysis/nnhHome
branch=ilcsoft
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

#test_isValidBranch $branch
test_isValidHome

# ENVIRONMENT

export NNH_HOME=$home/$branch

#print_export

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
