#!/bin/bash
#
#SBATCH --job-name=prepareBDT
#SBATCH --output=_prepareBDTsubmit.out
#SBATCH --error=prepareBDT_submit.err
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
    echo
    echo "analysis input : $NNH_ANALYSIS_INPUTFILES"
    echo "analysis output : $NNH_ANALYSIS_OUTPUTFILES"
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

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    usage
    exit 1
}

valid_branchs=(original ilcsoft fcc)
function branchValid {
    valid=false
    for b in "${valid_branchs[@]}" ; do
        if [ $b == $1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-b $branch"
    fi
}

function homeValid {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif  ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    fi;
}

# PARAMETERS

home=~/nnhAnalysis
branch=ilcsoft
input=$home/$branch/processor/RESULTS
output=$home/$branch/analysis/DATA

recompile=1

while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  usage && exit 0 ;;
        
        c)  recompile=0;;
        
        n)  home=${OPTARG}
            homeValid;;
            
        b)  branch=${OPTARG}
            branchValid $branch;;
            
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

# ENVIRONMENT

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

export NNH_HOME=$home/$branch

export NNH_ANALYSIS_INPUTFILES=$input
export NNH_ANALYSIS_OUTPUTFILES=$output

#print_export

# COMPILATION

if [ -d $NNH_ANALYSIS_OUTPUTFILES ]; then
    rm -R $NNH_ANALYSIS_OUTPUTFILES
fi
mkdir $NNH_ANALYSIS_OUTPUTFILES
hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root

if [ $recompile -eq 0 ]; then
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

cd $NNH_HOME
./analysis/bin/prepareForBDT 1> $NNH_ANALYSIS_OUTPUTFILES/bdt_std_output.txt 2> $NNH_HOME/analysis/DATA/bdt_error_output.txt

