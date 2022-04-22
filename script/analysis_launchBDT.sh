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

# paramètres

valid_branchs=(original ilcsoft fcc)

home=~/nnhAnalysis
branch=ilcsoft

while getopts hn:b: flag ; do
    case "${flag}" in 
    
        h)  usage
            exit 0 ;;
            
        n)  home=${OPTARG};;
        
        b)  branch=${OPTARG}
            valid=false
            for b in "${valid_branchs[@]}" ; do
                if [ $b == $branch ] ; then
                    valid=true 
                fi
            done
            if ! $valid ; then
                error "-b $branch : no valid branch"
                exit 1
            fi
            ;;
        
        *)  error 'option no exist'
            exit 1;;
    esac
done 

# environnement

export NNH_HOME=$home/$branch

# exécution

cd $NNH_HOME/analysis/python

source ~/miniconda3/etc/profile.d/conda.sh
conda activate env_root_python

python3 launchBDT_bb.py 1> bb_std_output.txt 2> bb_error_output.txt 
python3 launchBDT_WW.py 1> WW_std_output.txt 2> WW_error_output.txt 

