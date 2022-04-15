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

echo
echo "Start nnh processor..."

# environnement

echo
echo "Loading environment..."

branch=ilcsoft

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

export NNH_HOME=~/nnhAnalysis/$branch

export NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
export NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/result

export NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES 
export NNH_PROCESSOR_BUILD=$NNH_HOME/processor/BUILD
export NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS

export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

echo "...environment terminate"
echo

# compilation

echo
echo "During compilation..."

if [ -d $NNH_PROCESSOR_BUILD ]; then 
    rm -R $NNH_PROCESSOR_BUILD
fi

mkdir -v $NNH_PROCESSOR_BUILD
cd $NNH_PROCESSOR_BUILD
cmake -C $ILCSOFT/ILCSoft.cmake .. 
make
make install

echo "...compilation terminate"
echo

# exécution

echo
echo "Start execution..."

mkdir -v $NNH_PROCESSOR_OUTPUTFILES
#touch $NNH_PROCESSOR_OUTPUTFILES/test.root
python3 $NNH_HOME/processor/script/launchNNHProcessor.py -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES

echo "...execution terminate"
echo

echo "...Terminate nnh processor"
echo
