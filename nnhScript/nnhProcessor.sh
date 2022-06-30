#!/bin/bash

# This program build and run for all processus project.
# WARNING : NNH_PROCESSOR_OUTPUT directory deleted
#
# INPUT: directory with processus directory with LCIO files
# OUTPUT: directory with once root file by processus number

echo
echo "--> BEGIN : processor <--"
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
    echo "Run 'nnhProcessor'."
    echo
    echo "WARNING : -o directory will be ecrased."
    echo
    echo "ENTRY: directory path which contains directory processus with LCIO files."
    echo
    echo "RETURN: one files ROOT by processus number"
    echo
    echo 'SYNTAX:'
    echo '    ./nnhProcessor.sh [options]'
    
    syntaxOption h c n b i o #help.sh
}

## Test if it need build
function testNeedBuild {
    
    if [ $recompile -eq 1 ]; then
        if ! [ -f $home/processor/lib/libnnhProcessor.so ]; then 
            recompile=0
        fi
    elif ! [ -d $home/processor/BUILD ]; then
        recompile=0
    fi
} 

# ENVIRONMENT + in export.sh 

recompile=1 # no build

## option choice by user
while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
        
        c)  recompile=0;;
        
        n)  setPath ${OPTARG};; # export.sh
            
        b)  setBranch ${OPTARG};; # export.sh
            
        i)  setProcessorInput ${OPTARG};; # export.sh
        
        o)  setProcessorOutput ${OPTARG};; # export.sh
                
        *)  error 'option no exist';;
    esac
done 

## update env

nnh_export # export.sh
print_export # export.sh

export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

## test env

test_isValidHome # export.sh
testNeedBuild

# BUILD 

if [ $recompile -eq 0 ]; then
    echo
    echo "--> BUILD : processor ($branch) <--"
    echo
    
    if [ -d $NNH_HOME/processor/lib ]; then 
        rm -R $NNH_HOME/processor/lib
    fi
    
    if [ -d $NNH_HOME/processor/BUILD ]; then 
        rm -R $NNH_HOME/processor/BUILD
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

#if [ -d $NNH_PROCESSOR_OUTPUT ]; then 
#    rm -R $NNH_PROCESSOR_OUTPUT
#fi    
#mkdir $NNH_PROCESSOR_OUTPUT

python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
        -i $NNH_PROCESSOR_INPUT \
        -o $NNH_PROCESSOR_OUTPUT \
        -p 402001 \
        1>> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.out \
        2>> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.err 
        

echo
echo "--> END : processor ($branch) <--"
echo

