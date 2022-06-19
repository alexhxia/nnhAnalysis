#!/bin/bash

# This program build and run prepare for BDT.
#
# BEFORE : run processor part
# INPUT: directory with once root file by processus 
# OUTPUT: 
#   * DATA.root
#   * split_XX_eXX_pXX.root
#   * model_XX_eXX_pXX.root
#   * scrore_XX_eXX_pXX.root
#   * bestSelection_XX_eXX_pXX.root

# INCLUDE TOOL 

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

# FUNCTION TOOL 

## Display Help
function syntax {
    echo
    echo "Run 'prepareBDT'."
    echo "Prepare for BDT program."
    echo 
    echo "WARNING: input directory deleted"
    echo
    echo "ENTRY: processor output root files"
    echo
    echo "RETURN: DATA.root"
    echo
    echo 'SYNTAX:'
    echo '    ./prepareBDT.sh [options]'
    echo
    
    syntaxOption h c b n i o #help.sh
}

## Test if it need build
function testNeedBuild {
    
    if [ $recompile -eq 1 ]; then
        if ! [ -f $NNH_HOME/analysis/bin/prepareForBDT ]; then 
            echo "prepareForBDT no exist"
            recompile=0
        fi
        if ! [ -f $NNH_ANALYSIS_OUTPUT/DATA.root ]; then 
            echo "DATA.root no exist"
            recompile=0
        fi
    fi
} 

# ENVIRONMENT + in export.sh 

recompile=1

isInputUser=1
isOutputUser=1

## option choice by user
while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0 ;;
        
        c)  recompile=0;;
        
        n)  setPath ${OPTARG};;
            
        b)  setBranch ${OPTARG};;
            
        i)  setAnalysisInput ${OPTARG}
            isInputUser=0;;
            
        o)  setAnalysisOutput ${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

## update env

nnh_export
print_export

## test

test_isValidHome
testNeedBuild

# BUILD 

if [ $recompile -eq 0 ]; then

    echo
    echo "--> BUILD : prepareBDT ($branch) <--"
    echo
    
    if [ -d $NNH_ANALYSIS_OUTPUT ]; then
        rm -R $NNH_ANALYSIS_OUTPUT
    fi
    mkdir $NNH_ANALYSIS_OUTPUT
    #hadd $NNH_ANALYSIS_OUTPUT/DATA.root $NNH_ANALYSIS_INPUT/*.root

    if [ -d $NNH_HOME/analysis/BUILD ]; then
        rm -R $NNH_HOME/analysis/BUILD
    fi
    
    mkdir $NNH_HOME/analysis/BUILD
    cd $NNH_HOME/analysis/BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
fi

# RUN 

echo
echo "--> RUN : prepareBDT ($branch) <--"
#echo

cd $NNH_HOME/analysis/bin
echo "$branch"
if [ "$branch" == "original" ]; then 
    echo "original"
    ./prepareForBDT #\
        # 1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
        # 2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err
else 
    echo "other"
    ./prepareForBDT $NNH_ANALYSIS_OUTPUT #\
        # 1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
        # 2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err
fi

echo
echo "--> END : prepareBDT ($branch) <--"
echo
