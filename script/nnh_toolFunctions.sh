#!/bin/bash

# Stop program with error
# Entry: error message
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    syntax
    exit 1
}

# Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
    fi
}

# Test if name project exist (no use now)
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
