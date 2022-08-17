#!/bin/bash

source tools/functions.sh
source tools/help.sh

################################## ATTRIBUTES ##################################

# SERVER

server=/gridgroup/ilc/nnhAnalysisFiles # path to server

input=$server/AHCAL # path to data
output=$server/result # path to result

# LOCAL

## PROJET

path=~/nnhAnalysis # path to projet

program=$path/nnhProgram # path to programs 
result=$path/nnhResult # path to results
script=$path/nnhScript # path to script for run program easier
test=$path/nnhTest # path to result file tests

### HOME

original=$program/original
ilcsoft=$program/ilcsoft
fcc=$program/fcc

#branch=original
branch=ilcsoft # default
#branch=fcc

home=$program/$branch

#### PROCESSOR

p_input=$input
p_output=$home/processor/RESULTS # default, local

#### ANALYSIS

a_input=$p_output
a_output=$home/analysis/DATA # default, local

################################### COMMANDS ###################################

# SERVER

function setServer {
    #test_isValidPath $1
    server=$1

    setServerInput $server/AHCAL
    setServerOutput $server/result
}

function setServerInput {
    #test_isValidPath $1
    input=$1
}

function setServerOutput { 
    #test_isValidPath $1
    output=$1
}

# LOCAL

## PROJECT

function setPath {
    test_isValidPath $1
    path=$1
    
    setProgram $path/nnhProgram
    setResult $path/nnhResult
    setScript $path/nnhScript
    setTest $path/nnhTest
}

function setProgram {
    test_isValidPath $1
    program=$1
    
    original=$program/original
    ilcsoft=$program/ilcsoft
    fcc=$program/fcc
}

function setResult {
    test_isValidPath $1
    result=$1
}

function setScript {
    test_isValidPath $1
    script=$1
}

function setTest {
    test_isValidPath $1
    test=$1
}

## HOME

function setBranch {
    branch=$1
    
    setHome $program/$branch
}

function setHome {
    test_isValidHome $1
    home=$1
    
    setProcessorOutput $home/processor/RESULTS
    setAnalysisOutput $home/analysis/DATA
}

### PROCESSOR

function setProcessorInput {
    p_input=$1
}

function setProcessorOutput {
    p_output=$1

    setAnalysisInput $p_output
}

### ANALYSIS

function setAnalysisInput {
    a_input=$1
}

function setAnalysisOutput {
    a_output=$1
}

# Add or Update environment variables
function nnh_export {
   
    # SERVER
    export NNH_SERVER=/gridgroup/ilc/nnhAnalysisFiles
    
    export NNH_INPUT=$input
    export NNH_OUTPUT=$output
     
    # PROJECT
    export NNH_PATH=~/nnhAnalysis
    
    export NNH_PROGRAM=$program
    export NNH_RESULT=$result
    export NNH_SCRIPT=$script
    export NNH_TEST=$test

    ## PROGRAM
    export NNH_ORIGINAL=$original
    export NNH_ILCSOFT=$ilcsoft
    export NNH_FCC=$fcc
    
    ## HOME
    export NNH_HOME=$home
    
    ### PROCESSOR
    export NNH_PROCESSOR_INPUT=$p_input
    export NNH_PROCESSOR_OUTPUT=$p_output
    
    ### ANALYSIS
    export NNH_ANALYSIS_INPUT=$a_input
    export NNH_ANALYSIS_OUTPUT=$a_output
}

################################### REQUESTS ###################################

# Print environment variables
function print_variables {
    echo 
    echo "Script variables: "
    echo
    echo "server:           $server"
    echo "- input:          $input"
    echo "- output:         $output"
    echo
    echo "project:          $path"
    echo "- program:        $program"
    echo "- result:         $result"
    echo "- script:         $script"
    echo "- test:           $test"
    echo
    echo "home:             $home"
    echo "- path:           $path"
    echo "- branch:         $branch"
    echo
    echo "processor:"
    echo "- input:          $p_input"
    echo "- output:         $p_output"
    echo
    echo "analysis:"
    echo "- input:          $a_input"
    echo "- output:         $a_output"
    echo
}

function print_export {
    echo 
    echo "Environment variables : "
    echo
    echo "NNH_SERVER:               $NNH_SERVER"
    echo "NNH_INPUT:                $NNH_INPUT"
    echo "NNH_OUTPUT:               $NNH_OUTPUT"
    echo
    echo "NNH_PATH:                 $NNH_PATH"
    echo 
    echo "NNH_PROGRAM:              $NNH_PROGRAM"
    echo "NNH_RESULT:               $NNH_RESULT"
    echo "NNH_SCRIPT:               $NNH_SCRIPT"
    echo "NNH_TEST:                 $NNH_TEST"
    echo
    echo "NNH_ORIGINAL:             $NNH_ORIGINAL"
    echo "NNH_ILCSOFT:              $NNH_ILCSOFT"
    echo "NNH_FCC:                  $NNH_FCC"
    echo
    echo "NNH_HOME:                 $NNH_HOME"
    echo 
    echo "NNH_PROCESSOR_INPUT:      $NNH_PROCESSOR_INPUT"
    echo "NNH_PROCESSOR_OUTPUT:     $NNH_PROCESSOR_OUTPUT"
    echo 
    echo "NNH_ANALYSIS_INPUT:       $NNH_ANALYSIS_INPUT"
    echo "NNH_ANALYSIS_OUTPUT:      $NNH_ANALYSIS_OUTPUT"
    echo
}

################################ REQUESTS: TEST ################################

# Test if the directory home is valid
# Entry: $1, a path like $path
function test_isValidPath {
    if ! [ -d $1 ]; then 
        echo "Path $path is not find"
        exit 1
    fi
}

# Stop a program if the directory home or program directory are not find
# Entry: $1, a path like /$path/$branch
function test_isValidHome {
    
    if ! [ -d $1 ]; then 
        echo "home $1 directory no exist"
        exit 1
    fi 
    
    shopt -s nocasematch

    case $1 in

        $NNH_ORIGINAL | $NNH_ILCSOFT)
            if ! [ -d $1/analysis ]; then 
                echo 'No containt analysis directory'
                exit 1
            elif ! [ -d $1/processor ]; then 
                echo 'No containt processor directory'
                exit 1
            fi
            ;;

        $NNH_FCC)
            ;;

        *)
            echo "$1"
            echo "WARNING: the path is valid, but the sub-directories can't check."
            ;;
    esac
}

# Test if name project exist (no use now)
# Entry : $1, a name branch 
function test_isValidBranch {
    valid=false
    for b in $NNH_PROGRAM/*; do
        echo "$b"
        if [ -d $NNH_PROGRAM/$1 ] && [ $b == $NNH_PROGRAM/$1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-b $branch"
    fi
}
