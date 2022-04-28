#!/bin/bash

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo "processor input : $NNH_PROCESSOR_INPUTFILES"
    echo "processor output : $NNH_PROCESSOR_OUTPUTFILES"
    echo
}

# Display Help
function syntax {
    echo
    echo "Run 'processor' program."
    echo
    echo 'Syntax : ./processor.sh [options]'
    echo 'options:'
    echo '    -h : print help'
    echo '    -c : build just'
    echo '    -a : build and run'
    echo '    -n [directory]: nnhHome directory'
    echo '    -b [name]: type project or branch'
    echo '    -i [directory]: input directory'
    #echo '    -o [directory]: output directory'
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
valid_branchs=(original ilcsoft fcc)
function branchValid {
    valid=false
    for b in "${valid_branchs[@]}" ; do
        if [ $b == $1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-p $branch"
    fi
}

# Test if project have the good directory
function homeValid {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
    fi;
}

# PARAMETERS

# default value
home=~/nnhAnalysis/nnhHome
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
recompile=1 # no build
run=0       # run

# option choice by user
while getopts hcan:b:i: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
        
        c)  recompile=0
            run=1;;
        
        a)  recompile=0;;
        
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        i)  input=${OPTARG};;
        
        *)  error 'option no exist';;
        
    esac
done 

# TEST PARAMETER VALUES

branchValid $branch
homeValid

if ! [ -d $NNH_HOME/processor/BUILD ]; then 
    recompile=0
fi

if ! [ -d $NNH_HOME/processor/lib ]; then 
    recompile=0
fi

# ENVIRONMENT

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
export NNH_HOME=$home/$branch
export NNH_PROCESSOR_INPUTFILES=$input 
export NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

# COMPILATION

if [ $recompile -eq 0 ]; then
    if [ -d $NNH_HOME/processor/BUILD ]; then 
        rm -R $NNH_HOME/processor/BUILD
    fi
    if [ -d $NNH_HOME/processor/lib ]; then 
        rm -R $NNH_HOME/processor/lib
    fi

    mkdir $NNH_HOME/processor/BUILD
    cd $NNH_HOME/processor/BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
fi

# RUN
if [ $run -eq 0 ]; then
    if [ -d $NNH_PROCESSOR_OUTPUTFILES ]; then 
        rm -R $NNH_PROCESSOR_OUTPUTFILES
    fi    
    mkdir -v $NNH_PROCESSOR_OUTPUTFILES

    python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
            -i $NNH_PROCESSOR_INPUTFILES \
            -o $NNH_PROCESSOR_OUTPUTFILES \
            1> $NNH_PROCESSOR_OUTPUTFILES/launchNNHProcessor.out \
            2> $NNH_PROCESSOR_OUTPUTFILES/launchNNHProcessor.err
fi
