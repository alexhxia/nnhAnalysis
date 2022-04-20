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

# HELP cmd
for x in $* ; do
    if [ $x = "-h" ] || [ $x = "--help" ] ; then
        echo "./processor -c -n $NNH_HOME -b $branch -i $INPUTDIRECTORY -o $OUTPUTDIRECTORY"
        exit 0
    fi
done 

# paramètres
valid_branchs=(original ilcsoft fcc)

branch=ilcsoft
recompile=1
home=~/nnhAnalysis
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=$home/processor/RESULTS

while getopts h:c:n:b:i:o: flag ; do
    case "${flag}" in 
        h)
            echo "./processor [-c 0] [-n $NNH_HOME] [-b $branch] [-i $INPUTDIRECTORY] [-o $OUTPUTDIRECTORY]"
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
export NNH_PROCESSOR_INPUTFILES=$input 
export NNH_PROCESSOR_OUTPUTFILES=$output
export NNH_PROCESSOR_BUILD=$NNH_HOME/processor/BUILD
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

# compilation

if [ $recompile -eq 0 ]; then
    if [ -d $NNH_PROCESSOR_BUILD ]; then 
        rm -R $NNH_PROCESSOR_BUILD
    fi

    mkdir -v $NNH_PROCESSOR_BUILD
    cd $NNH_PROCESSOR_BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
    echo
fi

# exécution

[[ -d $NNH_PROCESSOR_OUTPUTFILES ]] && rm $NNH_PROCESSOR_OUTPUTFILES
mkdir -v $NNH_PROCESSOR_OUTPUTFILES
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES

