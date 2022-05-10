#!/bin/bash

### FUNCTION TOOL ###

init_err = source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh ||
export_err = source export.sh 
help_err = source help.sh

if ! init_err || ! export_err || ! help_err; then 
    echo("import error")
    exit 1
fi


# Display Help
function syntax {
    echo
    echo "Run 'processor'."
    echo
    echo 'SYNTAX:'
    echo '    ./processor.sh [options]'
    
    syntaxOption h c n b i #help.sh
}

# PARAMETERS

# look environement parameters default in export.sh
recompile=1 # no build

# option choice by user
while getopts hcn:b:i: flag ; do
    case "${flag}" in 
    
        h)  syntax
            exit 0;;
        
        c)  recompile=0;;
        
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        i)  p_input=${OPTARG};;
        
        *)  error 'option no exist';;
    esac
done 

# test parameters

test_isValidHome

if [ $recompile -eq 1 ]; then
    if ! [ -d $NNH_HOME/processor/BUILD ]; then 
        recompile=0
    fi

    if ! [ -d $NNH_HOME/processor/lib ]; then 
        recompile=0
    fi
fi

# ENVIRONMENT

nnh_export
print_export

export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

# COMPILATION

if [ $recompile -eq 0 ]; then
    echo
    echo "--> BUILD : processor ($branch) <--"
    echo
    if [ -d $NNH_HOME/processor/BUILD ]; then 
        rm -R $NNH_HOME/processor/BUILD
    fi
    if [ -d $NNH_HOME/processor/lib ]; then 
        rm -R $NNH_HOME/processor/lib
    fi

    mkdir $NNH_HOME/processor/BUILD
    cd $NNH_HOME/processor/BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
fi

# RUN

echo
echo "--> RUN : processor ($branch) <--"
echo

if [ -d $NNH_PROCESSOR_OUTPUT ]; then 
    rm -R $NNH_PROCESSOR_OUTPUT
fi    
mkdir -v $NNH_PROCESSOR_OUTPUT

python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
        -i $NNH_PROCESSOR_INPUT \
        -o $NNH_PROCESSOR_OUTPUT \
        1> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.out \
        2> $NNH_PROCESSOR_OUTPUT/launchNNHProcessor.err

echo
echo "--> END : processor ($branch) <--"
echo
