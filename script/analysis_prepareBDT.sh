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
echo "Prepare BDT..."

# environnement

echo
echo "    Loading environment..."

branch=ilcsoft

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh

export NNH_HOME=~/nnhAnalysis/$branch

export NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
export NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/result/$branch

export NNH_ANALYSIS_INPUTFILES=NNH_HOME/processor/RESULTS
export NNH_ANALYSIS_BUILD=$NNH_HOME/analysis/BUILD
export NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA


echo "    ... environment completed"
echo 

# compilation analysis

echo
echo "    During analysis compilation..."

hadd $NNH_ANALYSIS_OUTPUTFILES/DATA.root $NNH_ANALYSIS_INPUTFILES/*.root

cd $NNH_ANALYSIS_BUILD
cmake -C $ILCSOFT/ILCSoft.cmake .. 
make
make install

echo "    ...compilation terminate"
echo

# exécution analysis

echo
echo "    Start analysis execution..."

./$NNH_HOME/analysis/bin/prepareForBDT

echo "    ...execution terminate"
echo

echo "...BDT ready"
echo
