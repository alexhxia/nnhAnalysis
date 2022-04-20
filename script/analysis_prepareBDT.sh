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

#paramètres

valid_branchs=(original ilcsoft fcc)
branch=ilcsoft

home=~/nnhAnalysis
input=$NNH_HOME/processor/RESULTS
output=$NNH_HOME/analysis/DATA

recompile=1

while getopts h:c:n:b:i:o: flag ; do
    case "${flag}" in 
        h)
            echo "./processor [-c 0|1] [-n $NNH_HOME] [-b $branch] [-i $INPUTDIRECTORY] [-o $OUTPUTDIRECTORY]"
            exit 0 ;;
        c)
            recompile=${OPTARG};;
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
        i)
            input=${OPTARG} 
            ;;
        o)
            output=${OPTARG} 
            ;;
    esac
done 

# environnement

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

export NNH_HOME=$home/$branch

export NNH_ANALYSIS_INPUTFILES=$input
export NNH_ANALYSIS_BUILD=$NNH_HOME/analysis/BUILD
export NNH_ANALYSIS_OUTPUTFILES=$output

# compilation analysis

[[ -d $NNH_ANALYSIS_OUTPUTFILES ]] && rm $NNH_ANALYSIS_OUTPUTFILES
hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root

if [ $recompile -eq 0 ]; then
    [[ -d $NNH_ANALYSIS_BUILD ]] && rm $NNH_ANALYSIS_BUILD
    mkdir $NNH_ANALYSIS_BUILD
    cd $NNH_ANALYSIS_BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
fi

# exécution analysis

./$NNH_HOME/analysis/bin/prepareForBDT

