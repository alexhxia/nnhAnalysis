#!/bin/bash
#
#SBATCH --job-name=test
#SBATCH --output=submit.out
#SBATCH --error=submit.err
#
#SBATCH --ntasks=1
#SBATCH --time=1:00:00          # means 1h 00m 00s
#SBATCH --mem-per-cpu=1G
# mail-type=BEGIN, END, FAIL, REQUEUE, ALL, STAGE_OUT, TIME_LIMIT_90
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.hocine@ip2i.in2p3.fr

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo "input : $NNH_INPUTFILES"
    echo "output : $NNH_OUTPUTFILES"
    echo
    echo "processor input : $NNH_PROCESSOR_INPUTFILES"
    echo "processor output : $NNH_PROCESSOR_OUTPUTFILES"
    echo "processor build : $NNH_PROCESSOR_BUILD"
    echo
    echo "analysis input : $NNH_ANALYSIS_INPUTFILES"
    echo "analysis output : $NNH_ANALYSIS_OUTPUTFILES"
    echo "analysis build : $NNH_ANALYSIS_BUILD"
    echo
}

function usage {
    echo
    echo 'Usage : ./analysis_prepareBDT.sh [options]'
    echo '-h : print help'
    echo '-c : compile'
    echo '-n [directory]: nnhAnalysis directory'
    echo '-b [name]: branch'
    echo '-i [directory]: input directory'
    echo '-o [directory]: output directory'
    echo
}

function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    usage
    exit 1
}

#paramètres

valid_branchs=(original ilcsoft fcc)

home=~/nnhAnalysis
branch=ilcsoft
input=$home/$branch/processor/RESULTS
output=$home/$branch/analysis/DATA

recompile=1

while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  usage
            exit 0 ;;
            
        c)  recompile=0;;
        
        n)  home=${OPTARG};;
        
        b)  branch=${OPTARG}
            valid=false
            for b in "${valid_branchs[@]}" ; do
                if [ $b == $branch ] ; then
                    valid=true 
                fi
            done
            if ! $valid ; then
                exit 1
            fi
            ;;
            
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

# environnement

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

export NNH_HOME=$home/$branch

export NNH_ANALYSIS_INPUTFILES=$input
export NNH_ANALYSIS_OUTPUTFILES=$output

print_export

# compilation analysis

if [ -d $NNH_ANALYSIS_OUTPUTFILES ]; then
    rm -R $NNH_ANALYSIS_OUTPUTFILES
fi

mkdir -v $NNH_ANALYSIS_OUTPUTFILES
hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root

if [ $recompile -eq 0 ]; then
    if [ -d $NNH_HOME/analysis/BUILD ]; then
        rm -R $NNH_HOME/analysis/BUILD
    fi
    mkdir $NNH_HOME/analysis/BUILD
    cd $NNH_ANALYSIS_BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
fi

# exécution analysis

cd $NNH_HOME
./analysis/bin/prepareForBDT

