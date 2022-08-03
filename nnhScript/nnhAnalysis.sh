#!/bin/bash

# This program build and run all analysis part !
#   * prepareForBDT
#   * launchBDT_XX.py with XX = {WW, bb}
#
# INPUT: directory with processus root
#   * 402001.root
#   * 402002.root
#   * 402003.root
#   * ...
#
# OUTPUT:
#   * prepareForBDT
#       -> DATA.root
#       -> split_XX_eXX_pXX.root
#   * launchBDT_XX.py with XX = {WW or bb}
#       -> model_XX_eYY_pYY.root
#       -> score_XX_eYY_pYY.root
#       -> bestSelection_XX_eYY_pYY.root
#       -> stats_XX_eYY_pYY.json

tab="    "

echo
echo "$tab""Start nnhAnalysis..."
echo

# INCLUDE TOOL 

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

# FUNCTION TOOL

## Display Help
function syntax {
    echo
    echo "Run 'nnhAnalysis.sh': ie 'prepareBDT' and 'launchBDT'."
    echo
    echo "INPUT: root files create by processor"
    echo
    echo "OUTPUT: statistic analysis files"
    echo
    echo 'SYNTAX:'
    echo '    ./nnhAnalysis.sh [options]'
    echo
    
    syntaxOption h c v b n k q #help.sh
}

# ENVIRONMENT + in export.sh 

recompile=1 # no build
verbose=1 # no verbose

## option choice by user
while getopts hcvb:n:k:q: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
            
        c)  recompile=0;;
        
        v)  verbose=0;;
            
        b)  setBranch ${OPTARG};;
                                
        n)  setPath ${OPTARG};;
                
        k)  setAnalysisInput ${OPTARG};;
        
        q)  setAnalysisOutput ${OPTARG};;
        
        *) error '    Option no exist';;
    esac
done 

## update environment

nnh_export # export.sh
if [ $verbose -eq 0 ]; then
    print_export # export.sh
fi

## test environment

test_isValidHome

# RUN

if [ -d $NNH_ANALYSIS_OUTPUT ]; then
    rm -R $NNH_ANALYSIS_OUTPUT/*
else 
    mkdir -pv $NNH_ANALYSIS_OUTPUT
    echo
fi

## prepareBDT

echo "$tab""--> Run: prepareBDT.sh ..."
if [ $verbose -eq 0 ]; then
    if [ $recompile -eq 0 ]; then
        ./prepareBDT.sh -c -v \
                -n $path \
                -b $branch \
                -i $NNH_ANALYSIS_INPUT \
                -o $NNH_ANALYSIS_OUTPUT
    else 
        ./prepareBDT.sh -v \
                -n $path \
                -b $branch \
                -i $NNH_ANALYSIS_INPUT \
                -o $NNH_ANALYSIS_OUTPUT
    fi 
else
    if [ $recompile -eq 0 ]; then
        ./prepareBDT.sh -c \
                -n $path \
                -b $branch \
                -i $NNH_ANALYSIS_INPUT \
                -o $NNH_ANALYSIS_OUTPUT
    else 
        ./prepareBDT.sh \
                -n $path \
                -b $branch \
                -i $NNH_ANALYSIS_INPUT \
                -o $NNH_ANALYSIS_OUTPUT
    fi 
fi 

## launchBDT

echo "$tab""--> Run: launchBDT.sh..."
if [ $verbose -eq 0 ]; then
    ./launchBDT.sh -v \
            -n $path \
            -b $branch \
            -i $NNH_ANALYSIS_INPUT
else
    ./launchBDT.sh \
            -n $path \
            -b $branch \
            -i $NNH_ANALYSIS_INPUT
fi

echo
echo "$tab""...End nnhAnalysis"
echo
