#!/bin/bash

# SERVER
server=/gridgroup/ilc/nnhAnalysisFiles

input=$server/AHCAL
output=$server/result

# HOME
path=~/nnhAnalysis

program=$path/nnhProgram
result=$path/nnhResult
script=$path/nnhScript
test=$path/nnhTest

branch=ilcsoft
home=$program/$branch

# PROCESSOR
p_input=$input
p_output=$home/processor/RESULTS

# ANALYSIS
a_input=$p_output
a_output=$home/analysis/DATA

# Set commands
function setPath {
    path=$1
    
    setProgram $path/nnhProgram
    result=$path/nnhResult
    script=$path/nnhScript
    test=$path/nnhTest
}

function setServer {
    server=$1

    input=$server/AHCAL
    output=$server/result
}

function setServerInput {
    input=$1
    
    p_input=$input
}

function setServerOutput { 
    p_output=$1
}

function setProgram {
    program=$1

    setHome $program/$branch
}

function setResult {
    result=$1
}

function setScript {
    script=$1
}

function setTest {
    test=$1
}

function setHome {
    home=$1
    
    setProcessorOutput $home/processor/RESULTS
    setAnalysisOutput $home/analysis/DATA
}

function setBranch {
    branch=$1
    
    setHome $program/$branch
}

function setProcessorInput {
    p_input=$1
}

function setProcessorOutput {
    p_output=$1

    a_input=$p_output
}

function setAnalysisInput {
    a_input=$1
}

function setAnalysisOutput {
    a_output=$1
}

# Print environment variables
function print_export {
    echo
    echo "path:             $path"
    echo "branch:           $branch"
    echo
    echo "home:             $NNH_HOME"
    echo 
    echo "program:          $NNH_PROGRAM"
    echo "result:           $NNH_RESULT"
    echo "script:           $NNH_SCRIPT"
    echo "test:             $NNH_TEST"
    echo
    echo "server:           $NNH_SERVER"
    echo "input:            $NNH_INPUT"
    echo "output:           $NNH_OUTPUT"
    echo
    echo "processor input:  $NNH_PROCESSOR_INPUT"
    echo "processor output: $NNH_PROCESSOR_OUTPUT"
    echo
    echo "analysis input:   $NNH_ANALYSIS_INPUT"
    echo "analysis output:  $NNH_ANALYSIS_OUTPUT"
    echo
}

# Add or Update environment variables
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
    
    export NNH_INPUT=$NNH_SERVER/AHCAL
    export NNH_OUTPUT=$NNH_SERVER/result
    
    # FOR RUN PROCESSOR
    export NNH_PROCESSOR_INPUT=$p_input
    export NNH_PROCESSOR_OUTPUT=$p_output
    
    # FOR RUN ANALYSIS
    export NNH_ANALYSIS_INPUT=$a_input
    export NNH_ANALYSIS_OUTPUT=$a_output
}


# Test if the directory home is valid
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

# Test if name project exist (no use now)
# Entry : $1, a name branch 
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
