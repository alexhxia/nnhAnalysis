#!/bin/bash

# This program build and run for all processus project.
#
# INPUT: directory with processus directory with LCIO files
# OUTPUT: directory with once root file by processus number

echo
echo "--> BEGIN : processor ($branch) <--"
echo

# INCLUDE TOOL 

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

# FUNCTION TOOL 

## Display Help
function syntax {
    echo
    echo "Run 'processor'."
    echo
    echo "Entry: directory path which contains directory processus with LCIO files."
    echo "Exit: one files ROOT by processus number"
    echo
    echo 'SYNTAX:'
    echo '    ./nnhProcessor.sh [options]'
    
    syntaxOption h c n b i #help.sh
}

## Test if it need build
function testNeedBuild {
    
    if [ $recompile -eq 1 ]; then
        if ! [ -f $NNH_HOME/processor/lib/libnnhProcessor.so ]; then 
            recompile=0
        fi
    fi
} 

# ENVIRONMENT + in export.sh 

recompile=1 # no build

## option choice by user
while getopts hcn:b:i: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
        
        c)  recompile=0;;
        
        n)  setHome ${OPTARG};; # export.sh
            
        b)  setBranch ${OPTARG};; # export.sh
            
        i)  p_input=${OPTARG};;
                
        *)  error 'option no exist';;
    esac
done 

## update env

nnh_export && print_export
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

## test env

#test_isValidHome && error "home is no valid."
testNeedBuild

# BUILD 

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

if [ -d $NNH_PROCESSOR_OUTPUT ]; then 
    rm -R $NNH_PROCESSOR_OUTPUT
fi    
mkdir $NNH_PROCESSOR_OUTPUT

python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
        -i $NNH_PROCESSOR_INPUT \
        -o $NNH_PROCESSOR_OUTPUT \
        1> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.out \
        2> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.err

echo
echo "--> END : processor ($branch) <--"
echo
