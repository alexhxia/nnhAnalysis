#!/bin/bash

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo
    echo "analysis input : $NNH_ANALYSIS_INPUTFILES"
    echo "analysis output : $NNH_ANALYSIS_OUTPUTFILES"
    echo
}

function usage {
    echo
    echo 'Usage : ./analysis_prepareBDT.sh [options]'
    echo 'options'
    echo '    -h : print help'
    echo '    -c : just build'
    echo '    -a : build and run'
    echo '    -n [directory]: nnhAnalysis directory'
    echo '    -b [name]: branch'
    echo '    -i [directory]: input directory'
    echo '    -o [directory]: output directory'
    echo
}

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    usage
    exit 1
}

branchsValid=(original ilcsoft fcc)
function testBranchValid {
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

function testHomeValid {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif  ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    fi;
}

# PARAMETERS

home=~/nnhAnalysis/nnhHome
branch=ilcsoft
input=processor/RESULTS
output=analysis/DATA

recompile=1
run=0

isInputUser=1
isOutputUser=1

while getopts hcan:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  usage && exit 0 ;;
        
        c)  recompile=0
            run=1;;
        
        a)  recompile=0;;
        
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

testBranchValid $branch
testHomeValid

if [ $isInputUser -eq 1 ]; then 
    input=$home/$branch/$input
fi

if [ $isOutputUser -eq 1 ]; then 
    output=$home/$branch/$output
fi
    
if [ $run -eq 1 ]; then 
    if ! [ -d $NNH_HOME/analysis/BUILD ]; then
        recompile=0
    fi

    if ! [ -d $NNH_HOME/analysis/bin ]; then
        recompile=0
    fi

    if ! [ -f $NNH_ANALYSIS_OUTPUTFILES/DATA.root ]; then
        recompile=0
    fi
fi

# ENVIRONMENT

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

export NNH_HOME=$home/$branch

export NNH_ANALYSIS_INPUTFILES=$input
export NNH_ANALYSIS_OUTPUTFILES=$output

#print_export

# COMPILATION

if [ $recompile -eq 0 ]; then
    if [ -d $NNH_ANALYSIS_OUTPUTFILES ]; then
        rm -R $NNH_ANALYSIS_OUTPUTFILES
    fi
    mkdir $NNH_ANALYSIS_OUTPUTFILES
    hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root

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
if [ $run -eq 0 ]; then
    cd $NNH_HOME
    ./analysis/bin/prepareForBDT \
            1> $NNH_ANALYSIS_OUTPUTFILES/prepareBDT.out \
            2> $NNH_ANALYSIS_OUTPUTFILES/prepareBDT.err
fi

echo "fin $branch"
