#!/bin/bash
#
#SBATCH --job-name=processor
#SBATCH --output=processor_submit.out
#SBATCH --error=processor_submit.err
#
#SBATCH --ntasks=1
#SBATCH --time=1:00:00          # means 1h 00m 00s
#SBATCH --mem-per-cpu=1G
# mail-type=BEGIN, END, FAIL, REQUEUE, ALL, STAGE_OUT, TIME_LIMIT_90
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.hocine@ip2i.in2p3.fr

function usage {
    echo
    echo 'Usage : ./nnh.sh [options]'
    echo '-h : print help'
    echo '-c : recompile'
    echo '-n [directory]: nnhAnalysis directory'
    echo '-b [name]: branch'
    echo '-i [directory]: input directory'
    #echo '-o [directory]: output directory'
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
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
    fi;
}

# PARAMETERS

home=~/nnhAnalysis
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
recompile=1

while getopts hcn:b:i: flag ; do
    case "${flag}" in 
        h)  usage && exit 0;;
        c)  recompile=0;;
        n)  home=${OPTARG}
            homeValid;;
        b)  branch=${OPTARG}
            branchValid $branch;;
        i)  input=${OPTARG};;
        *) error 'option no exist';;
    esac
done 

# ENVIRONMENT

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
export NNH_HOME=$home/$branch
export NNH_PROCESSOR_INPUTFILES=$input 
export NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

# COMPILATION

if [ $recompile -eq 0 ]; then
    if [ -d $NNH_HOME/processor/BUILD ]; then 
        rm -R $NNH_HOME/processor/BUILD
    fi

    mkdir $NNH_HOME/processor/BUILD
    cd $NNH_HOME/processor/BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
fi

# RUN

if [ -d $NNH_PROCESSOR_OUTPUTFILES ]; then 
    rm -R $NNH_PROCESSOR_OUTPUTFILES
fi    
mkdir -v $NNH_PROCESSOR_OUTPUTFILES

python3 $NNH_HOME/processor/script/launchNNHProcessor.py \
        -i $NNH_PROCESSOR_INPUTFILES \
        -o $NNH_PROCESSOR_OUTPUTFILES \
        1> $NNH_PROCESSOR_OUTPUTFILES/std_output.txt \
        2> $NNH_PROCESSOR_OUTPUTFILES/error_output.txt

