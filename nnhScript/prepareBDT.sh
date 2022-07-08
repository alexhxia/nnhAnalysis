#!/bin/bash

# This program build and run prepare for BDT.
#
# BEFORE : run processor part
# INPUT: directory with once root file by processus 
# OUTPUT: 
#   * DATA.root
#   * split_XX_eXX_pXX.root


echo
echo "--> BEGIN - prepareBDT <--"
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
    echo "Run 'prepareBDT'."
    echo "Prepare for BDT program."
    echo 
    echo "ENTRY: processor output root files"
    echo
    echo "RETURN: "
    echo " - DATA.root"
    echo " - 4 split_XX_eYY_pZZ.root files"
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
        elif ! [ -f $NNH_ANALYSIS_OUTPUT/DATA.root ]; then 
            echo "DATA.root no exist"
            recompile=0
        fi
    fi
} 

# ENVIRONMENT + in export.sh 

recompile=1

## option choice by user
while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0 ;;
        
        c)  recompile=0;;
        
        n)  setPath ${OPTARG};;
            
        b)  setBranch ${OPTARG};;
            
        i)  setAnalysisInput ${OPTARG};;
            
        o)  setAnalysisOutput ${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

## update env

nnh_export
#print_export

## Output directory
if [ ! -d $NNH_ANALYSIS_OUTPUT ]; then
    mkdir -vp $NNH_ANALYSIS_OUTPUT
    echo
fi
if [ $recompile -eq 0 ]; then 
    rm -Rf $NNH_ANALYSIS_OUTPUT/*
fi

## Merge all processor files
if [ ! -f $NNH_ANALYSIS_OUTPUT/DATA.root ]; then
    
    echo "    Merge processor files in DATA.root..."
    echo
    hadd $NNH_ANALYSIS_OUTPUT/DATA.root $NNH_ANALYSIS_INPUT/*.root \
            1> $NNH_ANALYSIS_OUTPUT\hadd.out \
            2> $NNH_ANALYSIS_OUTPUT\hadd.err 
    echo
fi

# build 
if [ $recompile -eq 0 ] || [ ! -f $NNH_HOME/analysis/bin/prepareForBDT ]; then

    echo "    Build analysis program in $branch branch..."
    echo
    
    if [ -d $NNH_HOME/analysis/BUILD ]; then
        rm -R $NNH_HOME/analysis/BUILD
    fi
    
    mkdir $NNH_HOME/analysis/BUILD
    cd $NNH_HOME/analysis/BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
    echo
fi

# RUN 


echo "    RUN prepareBDT for $branch branch..."
echo

cd $NNH_HOME/analysis/bin
if [ "$branch" == "original" ]; then 
    ./prepareForBDT \
        1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
        2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err
else 
    ./prepareForBDT \
        $NNH_ANALYSIS_OUTPUT \
        1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
        2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err
fi
echo

echo "--> END - prepareBDT <--"
echo
