#!/bin/bash

function print_export {
    echo
    echo "home :             $NNH_HOME"
    echo
    echo "processor input :  $NNH_PROCESSOR_INPUT"
    echo "processor output : $NNH_PROCESSOR_OUTPUT"
    echo
}

# Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./processor.sh [options]'
    echo
    echo 'OPTIONS:'
    echo '   -h                print help'
    echo 
    echo '   -c                build then run'
    echo
    echo '   -b [name]         branch'
    echo "                     DEFAULT VALUE: $branch"
    echo
    echo '   -n [directory]    nnhAnalysis directory'
    echo "                     DEFAULT VALUE: $home"
    echo
    echo '   -i [directory]    input directory'
    echo "                     DEFAULT VALUE: $input"
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
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
    fi
}

# Test if the directory input is valid
function test_isValidInputDirectory {
    if ! [ -d $input ]; then 
        error '-i: input directory no exist'
    fi
}

# PARAMETERS

# default value

valid_branchs=(original ilcsoft fcc)
home=~/nnhAnalysis/nnhHome
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
recompile=1 # no build

# option choice by user
while getopts hcn:b:i: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
        
        c)  recompile=0;;
        
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        i)  input=${OPTARG};;
        
        *)  error 'option no exist';;
    esac
done 

# test parameters

test_isValidBranch $branch
test_isValidHome
test_isValidInputDirectory

if [ $recompile -eq 1 ]; then
    if ! [ -d $NNH_HOME/processor/BUILD ]; then 
        recompile=0
    fi

    if ! [ -d $NNH_HOME/processor/lib ]; then 
        recompile=0
    fi
fi

# ENVIRONMENT

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
export NNH_HOME=$home/$branch
export NNH_PROCESSOR_INPUT=$input 
export NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/RESULTS
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

# COMPILATION

if [ $recompile -eq 0 ]; then
    echo
    echo "--> BUILD : processor ($branch) <--"
    echo
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

echo
echo "--> RUN : processor ($branch) <--"
echo

if [ -d $NNH_PROCESSOR_OUTPUT ]; then 
    rm -R $NNH_PROCESSOR_OUTPUT
fi    
mkdir -v $NNH_PROCESSOR_OUTPUT

python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
        -i $NNH_PROCESSOR_INPUT \
        -o $NNH_PROCESSOR_OUTPUT \
        1> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.out \
        2> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.err

echo
echo "--> END : processor ($branch) <--"
echo
