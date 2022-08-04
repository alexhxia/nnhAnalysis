#!/bin/bash

# This program build and run prepare for BDT.
#
# BEFORE : run processor part
# INPUT: directory with once root file by processus 
# OUTPUT: 
#   * DATA.root
#   * split_XX_eXX_pXX.root

tab="        "
echo
echo "$tab""Start prepareBDT..."
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
    echo "Merge all root files and Prepare for BDT program."
    echo 
    echo "INPUT: processor output root files"
    echo
    echo "OUTPUT: "
    echo " - DATA.root"
    echo " - 4 split_XX_eYY_pZZ.root files"
    echo
    echo 'SYNTAX:'
    echo '    ./prepareBDT.sh [options]'
    echo
    
    syntaxOption h c v b n k q #help.sh
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
verbose=1 # no verbose

## option choice by user
while getopts hcvn:b:k:q: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0 ;;
        
        c)  recompile=0;;
        
        v)  verbose=0;;

        n)  setPath ${OPTARG};;
            
        b)  setBranch ${OPTARG};;
            
        k)  setAnalysisInput ${OPTARG};;
            
        q)  setAnalysisOutput ${OPTARG};;
        
        *) error '      Option no exist';;
    esac
done 

## update environment

nnh_export # export.sh
if [ $verbose -eq 0 ]; then
    print_export # export.sh
fi

## test environment

test_isValidHome
testNeedBuild

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
    
    echo "$tab""--> Merge processor files in DATA.root..."
    echo
    if [ $verbose -eq 0 ]; then
        hadd $NNH_ANALYSIS_OUTPUT/DATA.root $NNH_ANALYSIS_INPUT/*.root
        echo
    else
        hadd $NNH_ANALYSIS_OUTPUT/DATA.root $NNH_ANALYSIS_INPUT/*.root \
            1> $NNH_ANALYSIS_OUTPUT\hadd.out \
            2> $NNH_ANALYSIS_OUTPUT\hadd.err 
    fi
fi

# build 
if [ $recompile -eq 0 ] || [ ! -f $NNH_HOME/analysis/bin/prepareForBDT ]; then

    echo "$tab""--> Build analysis program in $branch branch..."
    echo
    
    if [ -d $NNH_HOME/analysis/BUILD ]; then
        rm -R $NNH_HOME/analysis/BUILD/*
    else 
        mkdir -pv $NNH_HOME/analysis/BUILD
    fi
    
    cd $NNH_HOME/analysis/BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
    echo
fi

# RUN 


echo "$tab""--> Run: prepareBDT for $branch branch..."
echo

cd $NNH_HOME/analysis/bin
if [ $verbose -eq 0 ]; then
    if [ "$branch" == "original" ]; then 
        ./prepareForBDT
    else 
        ./prepareForBDT $NNH_ANALYSIS_OUTPUT
    fi
else 
    if [ "$branch" == "original" ]; then 
        ./prepareForBDT \
            1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
            2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err
    else 
        ./prepareForBDT $NNH_ANALYSIS_OUTPUT \
            1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
            2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err
    fi
fi
echo

echo "$tab""... End: prepareBDT"
echo
