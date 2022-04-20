#!/bin/bash
#
#SBATCH --job-name=test
#SBATCH --output=submit.out
#SBATCH --error=submit.err
#
#SBATCH --ntasks=1
#SBATCH --time=2:00:00          # means 2h 00m 00s
#SBATCH --mem-per-cpu=1G
# mail-type=BEGIN, END, FAIL, REQUEUE, ALL, STAGE_OUT, TIME_LIMIT_90
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.hocine@ip2i.in2p3.fr

# paramètres

valid_branchs=(original ilcsoft fcc)
branch=ilcsoft
home=~/nnhAnalysis

while getopts h:n:b: flag ; do
    case "${flag}" in 
        h)
            echo "./analysis_launchBDT [-n $NNH_HOME] [-b branch]"
            exit 0 ;;
        n)
            home=${OPTARG};;
        b)
            branch=${OPTARG}
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
    esac
done 

# environnement

export NNH_HOME=$home/$branch

# exécution

cd $NNH_HOME/analysis

conda activate env_root_python

python3 launchBDT_bb.py
python3 launchBDT_WW.py

conda desactivate

