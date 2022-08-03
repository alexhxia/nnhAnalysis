#!/bin/bash

################################## ATTRIBUTES ##################################

# SERVER
server=/gridgroup/ilc/nnhAnalysisFiles # path to server

input=$server/AHCAL # path to data
output=$server/result # path to result

# PROJET
path=~/nnhAnalysis # path to projet

## SUB-PROJET
program=$path/nnhProgram # path to programs 
result=$path/nnhResult # path to results
script=$path/nnhScript # path to script for run program easier
test=$path/nnhTest # path to result file tests

### HOME
original=$program/original
ilcsoft=$program/ilcsoft
fcc=$program/fcc

home=""

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

# PROJET

function setPath {
    test_isValidPath $1
    path=$1
    
    setProgram $path/nnhProgram
    setResult $path/nnhResult
    setScript $path/nnhScript
    setTest $path/nnhTest
}

# SUB-PROJET

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

function setHome {
    test_isValidHome $1
    home=$1
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
    echo "choices for home path :"
    echo "- original:       $original"
    echo "- ilcsoft:        $ilcsoft"
    echo "- fcc:            $fcc"
    echo
    echo "home:             $home"
    echo
}

function print_export {
    echo 
    echo "Environment variables : "
    echo
    echo "NNH_SERVER:       $NNH_SERVER"
    echo "NNH_INPUT:        $NNH_INPUT"
    echo "NNH_OUTPUT:       $NNH_OUTPUT"
    echo
    echo "NNH_PATH:         $NNH_PATH"
    echo 
    echo "NNH_PROGRAM:      $NNH_PROGRAM"
    echo "NNH_RESULT:       $NNH_RESULT"
    echo "NNH_SCRIPT:       $NNH_SCRIPT"
    echo "NNH_TEST:         $NNH_TEST"
    echo
    echo "NNH_ORIGINAL:     $NNH_ORIGINAL"
    echo "NNH_ILCSOFT:      $NNH_ILCSOFT"
    echo "NNH_FCC:          $NNH_FCC"
    echo
    echo "NNH_HOME:         $NNH_HOME"
    echo
}

################################ REQUESTS: TEST ################################

# Test if the directory home is valid
function test_isValidPath {
    if ! [ -d $path ]; then 
        echo "Path $path is not find"
        exit 1
    fi
}

# Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $1 ]; then 
        echo "home $1 directory no exist"
        exit 1
    elif ! [ -d $1/analysis ]; then 
        echo 'analysis directory no exist'
        exit 1
    elif ! [ -d $1/processor ]; then 
        echo 'processor directory no exist'
        exit 1
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
        echo "-b $branch"
        exit 1
    fi
}
