#!/bin/bash

### FUNCTION TOOL ###

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

source tools/functions.sh
source tools/export.sh 
source tools/help.sh

# Display Help
function syntax {
    echo
    echo "Prepare BDT program."
    echo
    echo 'SYNTAX:'
    echo '    ./prepareBDT.sh [options]'
    echo
    
    syntaxOption h c b n i # o #help.sh
}

# PARAMETERS

# look environement parameters default in export.sh
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
            
        i)  a_input=${OPTARG}
            isInputUser=0;;
        
        #o)  a_output=${OPTARG}
         #   isOutputUser=0;;
        
        *) error 'option no exist';;
    esac
done 

# ENVIRONMENT

nnh_export
test_isValidHome

#print_export

# COMPILATION

if [ $recompile -eq 1 ]; then 
    if ! [ -f "$NNH_HOME/analysis/bin/prepareForBDT" ]; then
        recompile=0
        echo "$NNH_HOME/analysis/bin/prepareForBDT"
        echo "BUILD FORCE (prepareForBDT program no exist)"
    fi

    if ! [ -f "$NNH_ANALYSIS_OUTPUT/DATA.root" ]; then
        recompile=0
        echo "$NNH_ANALYSIS_OUTPUT/DATA.root"
        echo "BUILD FORCE (DATA.root file no exist)"
    fi
fi

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

cd $NNH_HOME/analysis/bin
./prepareForBDT \
        1> $NNH_ANALYSIS_OUTPUT/prepareBDT.out \
        2> $NNH_ANALYSIS_OUTPUT/prepareBDT.err

echo
echo "--> END : prepareBDT ($branch) <--"
echo
