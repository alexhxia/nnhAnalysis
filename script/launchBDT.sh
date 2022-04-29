#!/bin/bash

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo
}


function usage {
    echo
    echo 'Usage : ./analysis_prepareBDT.sh [options]'
    echo '-h : print help'
    echo '-d : deactivate conda'
    echo '-n [directory]: nnhAnalysis directory'
    echo '-b [name]: branch'
    echo
}

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    usage
    exit 1
}

valid_branchs=(original ilcsoft fcc)
function branchValid {
    valid=false
    for b in "${valid_branchs[@]}" ; do
        if [ $b == $1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-b $branch"
    fi
}

function homeValid {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
   # elif ! [ -f $home/$branch/analysis/DATA/data.root ]; then 
    #    error '-n: home/branch/analysis/DATA/data.root file no exist (prepare BDT first)'
    fi
}

# PARAMETERS

home=~/nnhAnalysis/nnhHome
branch=ilcsoft
conda=0

while getopts hdn:b: flag ; do
    case "${flag}" in 
    
        h)  usage 
            exit 0;;
        
        d)  conda=1;;
            
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        *)  error 'option no exist';;
            
    esac
done 

homeValid
branchValid $branch

# ENVIRONMENT

export NNH_HOME=$home/$branch

#print_export

# RUN

cd $NNH_HOME/analysis/python

if [ $conda -eq 0 ]; then
    source ~/miniconda3/etc/profile.d/conda.sh
    conda activate env_root_python
fi

python3 launchBDT_bb.py 1> $NNH_HOME/analysis/DATA/launchBDT_bb.out 2> $NNH_HOME/analysis/DATA/launchBDT_bb.err 
python3 launchBDT_WW.py 1> $NNH_HOME/analysis/DATA/launchBDT_WW.err 2> $NNH_HOME/analysis/DATA/launchBDT_WW.err 

