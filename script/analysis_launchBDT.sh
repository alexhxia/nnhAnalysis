#!/bin/bash
#
#SBATCH --job-name=launchBDT
#SBATCH --output=launchBDT_submit.out
#SBATCH --error=launchBDT_submit.err
#
#SBATCH --ntasks=1
#SBATCH --time=5:00:00          # means 5h 00m 00s
#SBATCH --mem-per-cpu=1G
# mail-type=BEGIN, END, FAIL, REQUEUE, ALL, STAGE_OUT, TIME_LIMIT_90
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.hocine@ip2i.in2p3.fr

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo
}


function usage {
    echo
    echo 'Usage : ./analysis_prepareBDT.sh [options]'
    echo '-h : print help'
    echo '-n [directory]: nnhAnalysis directory'
    echo '-b [name]: branch'
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
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    elif ! [ -f $home/$branch/analysis/DATA/data.root ]; then 
        error '-n: home/branch/analysis/DATA/data.root file no exist (prepare BDT first)'
    fi
}

# PARAMETERS

home=~/nnhAnalysis
branch=ilcsoft

while getopts hn:b: flag ; do
    case "${flag}" in 
    
        h)  usage && exit 0 ;;
            
        n)  home=${OPTARG};;
            
        b)  branch=${OPTARG};;
            
        *)  error 'option no exist';;
            
    esac
done 

homeValid
branchValid $branch

# ENVIRONMENT

export NNH_HOME=$home/$branch

#print_export

# RUN

cd $NNH_HOME/analysis/python

source ~/miniconda3/etc/profile.d/conda.sh
conda activate env_root_python

python3 launchBDT_bb.py 1> $NNH_HOME/analysis/DATA/bb_std_output.txt 2> $NNH_HOME/analysis/DATA/bb_error_output.txt 
python3 launchBDT_WW.py 1> $NNH_HOME/analysis/DATA/WW_std_output.txt 2> $NNH_HOME/analysis/DATA/WW_error_output.txt 

