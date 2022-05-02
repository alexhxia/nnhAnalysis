#!/bin/bash

function print_export {
    echo
    echo "home :             $NNH_HOME"
    echo
    echo "analysis input :   $NNH_ANALYSIS_INPUT"
    echo "analysis output :  $NNH_ANALYSIS_OUTPUT"
    echo
}

# Display Help
function syntax {
    echo
    echo "Prepare BDT program."
    echo
    echo 'SYNTAX:'
    echo '    ./prepareBDT.sh [options]'
    echo
    echo 'OPTIONS:'
    echo '   -h                print help'
    echo 
    echo '   -c                build and run'
    echo
    echo '   -b [name]         branch'
    echo "                     DEFAULT VALUE: $branch"
    echo
    echo '   -n [directory]    nnhAnalysis directory'
    echo "                     DEFAULT VALUE: $home"
    echo
    echo '   -i [directory]    input directory'
    echo "                     DEFAULT VALUE: $input"
    echo
    echo '   -o [directory]    output directory'
    echo "                     DEFAULT VALUE: $output"
    echo
    
}

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    syntax
    exit 1
}

# Test if name project exist
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

# Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    fi
}

# Test if the directory input is valid
function test_isValidInputDirectory {
    if ! [ -d $input ]; then 
        error '-i: input directory no exist'
    fi
}

# Test if the directory output is valid
function test_isValidOutputDirectory {
    if ! [ -d $output ]; then 
        error '-o: output directory no exist'
    fi
}

# PARAMETERS

branchsValid=(original ilcsoft fcc)
home=~/nnhAnalysis/nnhHome
branch=ilcsoft
input=processor/RESULTS
output=analysis/DATA

recompile=1

isInputUser=1
isOutputUser=1

while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0 ;;
        
        c)  recompile=0;;
        
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        i)  input=${OPTARG}
            isInputUser=0;;
        
        o)  output=${OPTARG}
            isOutputUser=0;;
        
        *) error 'option no exist';;
    esac
done 

# TEST PARAMETERS

test_isValidBranch $branch
test_isValidHome

if [ $isInputUser -eq 1 ]; then 
    input=$home/$branch/$input
fi

test_isValidInputDirectory

if [ $isOutputUser -eq 1 ]; then 
    output=$home/$branch/$output
fi

test_isValidOutputDirectory
    
if [ $recompile -eq 1 ]; then 
    if ! [ -d $NNH_HOME/analysis/BUILD ]; then
        recompile=0
    fi

    if ! [ -d $NNH_HOME/analysis/bin ]; then
        recompile=0
    fi

    if ! [ -f $NNH_ANALYSIS_OUTPUT/DATA.root ]; then
        recompile=0
    fi
fi

# ENVIRONMENT

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
export NNH_HOME=$home/$branch
export NNH_ANALYSIS_INPUT=$input
export NNH_ANALYSIS_OUTPUT=$output

#print_export && exit 0

# COMPILATION

if [ $recompile -eq 0 ]; then
    echo
    echo "--> BUILD : prepareBDT ($branch) <--"
    echo
    if [ -d $NNH_ANALYSIS_OUTPUT ]; then
        rm -R $NNH_ANALYSIS_OUTPUT
    fi
    mkdir $NNH_ANALYSIS_OUTPUT
    hadd $NNH_ANALYSIS_OUTPUT/DATA.root $NNH_ANALYSIS_INPUT/*.root

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
echo

cd $NNH_HOME
./analysis/bin/prepareForBDT \
        1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
        2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err

echo
echo "--> END : prepareBDT ($branch) <--"
echo
