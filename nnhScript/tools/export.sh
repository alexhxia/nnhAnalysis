#!/bin/bash

# HOME
path=~/nnhAnalysis

home=$path/nnhHome
test=$path/nnhTest
script=$path/nnhScript

branch=original

# SERVER
server=/gridgroup/ilc/nnhAnalysisFiles

input=$server/AHCAL
output=$server/result

# PROCESSOR
p_input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
p_output=$home/$branch/processor/RESULTS

# ANALYSIS
a_input=$p_output
a_output=$home/$branch/analysis/DATA

# Set commands
function setBranch {
    branch=$1
    p_output=$home/$branch/processor/RESULTS
    a_input=$p_output
    a_output=$home/$branch/analysis/DATA
}

function setPath {
    path=$1
    setHome $path/nnhHome
    test=$path/nnhTest
    script=$path/nnhScript
}

function setHome {
    home=$1
    p_output=$home/$branch/processor/RESULTS
    a_input=$p_output
    a_output=$home/$branch/analysis/DATA
}

# Print environment variables
function print_export {
    echo
    echo "home :             $NNH_HOME"
    echo "script :           $NNH_SCRIPT"
    echo "test :             $NNH_TEST"
    echo
    echo "input :            $NNH_INPUT"
    echo "output :           $NNH_OUTPUT"
    echo
    echo "processor input :  $NNH_PROCESSOR_INPUT"
    echo "processor output : $NNH_PROCESSOR_OUTPUT"
    echo
    echo "analysis input :   $NNH_ANALYSIS_INPUT"
    echo "analysis output :  $NNH_ANALYSIS_OUTPUT"
    echo
}

# Add or Update environment variables
function nnh_export {
    
    # HOME
    export NNH_HOME=$home/$branch
    export NNH_SCRIPT=$script
    export NNH_TEST=$test

    # SERVER
    export NNH_INPUT=$input
    export NNH_OUTPUT=$output/$branch

    # FOR RUN PROCESSOR
    export NNH_PROCESSOR_INPUT=$p_input
    export NNH_PROCESSOR_OUTPUT=$p_output

    # FOR RUN ANALYSIS
    export NNH_ANALYSIS_INPUT=$a_input
    export NNH_ANALYSIS_OUTPUT=$a_output
}


# Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-b: home/branch directory no exist'
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
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
