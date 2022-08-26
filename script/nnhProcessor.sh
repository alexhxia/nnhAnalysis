#!/bin/bash

# This program build and run processus program.
#
# INPUT: directory with processus directory with LCIO files
#
# OUTPUT: directory with once root file by processus number

tab="    "

echo
echo "$tab""Start nnhProcessor..."
echo

# INCLUDE TOOLS

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

source nnhExport.sh 

source tools/functions.sh
source tools/help.sh

# FUNCTION TOOLS

## Display Help
function syntax {
    echo
    echo "Run 'nnhProcessor.sh': ie 'launchNNHProcessor.py'."
    echo
    echo "INPUT: directory path which contains directory processus with LCIO files."
    echo
    echo "OUTPUT: one files ROOT by processus number"
    echo
    echo 'SYNTAX:'
    echo '    ./nnhProcessor.sh [options]'
    echo
    
    syntaxOption h c v b n j m #help.sh
}

## Test if it need build
function testNeedBuild {
    
    if [ $recompile -eq 1 ]; then
        if ! [ -f $home/processor/lib/libnnhProcessor.so ]; then 
            recompile=0
        elif ! [ -d $home/processor/BUILD ]; then
            recompile=0
        fi
    fi
} 

# ENVIRONMENT + in export.sh 

recompile=1 # no build
verbose=1 # no verbose

## option choice by user
while getopts hcvb:n:j:m: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
        
        c)  recompile=0;;
        
        v)  verbose=0;;
                    
        b)  setBranch ${OPTARG};; # export.sh
        
        n)  setPath ${OPTARG};; # export.sh
            
        j)  setProcessorInput ${OPTARG};; # export.sh
        
        m)  setProcessorOutput ${OPTARG};; # export.sh
                
        *)  error 'option no exist';;
    esac
done 

## update environment

nnh_export # export.sh
if [ $verbose -eq 0 ]; then
    print_export # export.sh
fi

## test environment

test_isValidHome # export.sh
testNeedBuild

# BUILD 

if [ $recompile -eq 0 ]; then
    echo "$tab""--> Build : processor ($branch)..."
    
    if [ -d $NNH_HOME/processor/lib ]; then 
        rm -R $NNH_HOME/processor/lib
    fi
    
    if [ -d $NNH_PROCESSOR_OUTPUT ]; then 
        rm -R $NNH_PROCESSOR_OUTPUT
    fi
    
    if [ -d $NNH_HOME/processor/BUILD ]; then 
        rm -R $NNH_HOME/processor/BUILD
    fi
    
    mkdir -pv $NNH_HOME/processor/BUILD
    cd $NNH_HOME/processor/BUILD

    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
    
    echo
fi

# RUN 
echo "$tab""--> Run: launchNNHProcessor.py ($branch)..."

if [ -d $NNH_ANALYSIS_OUTPUT ]; then
    rm -R $NNH_PROCESSOR_OUTPUT/*
else 
    mkdir -pv $NNH_PROCESSOR_OUTPUT
    echo
fi

if [ $verbose -eq 0 ]; then
    python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
            -i $NNH_PROCESSOR_INPUT \
            -o $NNH_PROCESSOR_OUTPUT
else 
    python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
            -i $NNH_PROCESSOR_INPUT \
            -o $NNH_PROCESSOR_OUTPUT \
            1>> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.out \
            2>> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.err 
fi 

echo
echo "$tab""... End nnhProcessor ($branch)"
echo
