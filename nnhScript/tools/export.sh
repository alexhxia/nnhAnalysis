#!/bin/bash

## Add or Update environment variables
function nnh_export {
    
    # PROJECT
    export NNH_PATH=~/nnhAnalysis
    
    export NNH_PROGRAM=$NNH_PATH/nnhProgram
    export NNH_RESULT=$NNH_PATH/nnhResult
    export NNH_SCRIPT=$NNH_PATH/nnhScript
    export NNH_TEST=$NNH_PATH/nnhTest

    ## PROGRAM
    export NNH_ORIGINAL=$NNH_PROGRAM/original
    export NNH_ILCSOFT=$NNH_PROGRAM/ilcsoft
    export NNH_FCC=$NNH_PROGRAM/fcc

    # HOME (need absolutly for run)
    export NNH_HOME=$NNH_PROGRAM/$branch

    export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

    # SERVER
    export NNH_SERVER=/gridgroup/ilc/nnhAnalysisFiles
    
    export NNH_INPUT=$input
    export NNH_OUTPUT=$output
    
    # FOR RUN PROCESSOR
    export NNH_PROCESSOR_INPUT=$p_input
    export NNH_PROCESSOR_OUTPUT=$p_output
    
    # FOR RUN ANALYSIS
    export NNH_ANALYSIS_INPUT=$a_input
    export NNH_ANALYSIS_OUTPUT=$a_output
}


## Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $path ]; then 
        error '-p: path is not find'
    elif ! [ -d $home ]; then 
        error '-b: home directory no exist'
    elif ! [ -d $home/analysis ]; then 
        error '-n: home/analysis directory no exist'
    elif ! [ -d $home/processor ]; then 
        error '-n: home/processor directory no exist'
    fi
}

## Test if name project exist (no use now)
## Entry : $1, a name branch 
branchsValid=(original ilcsoft fcc)
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
