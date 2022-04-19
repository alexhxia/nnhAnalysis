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
echo "Start nnh analysis BDT..."

# environnement

echo
echo "Loading environment..."

branch=ilcsoft

export NNH_HOME=~/nnhAnalysis/$branch

export NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
export NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/result

export NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES 
export NNH_PROCESSOR_BUILD=$NNH_HOME/processor/BUILD
export NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS

export NNH_ANALYSIS_INPUTFILES=NNH_HOME/processor/RESULTS
export NNH_ANALYSIS_BUILD=$NNH_HOME/analysis/BUILD
export NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA

echo "...environment terminate"
echo

# exécution

echo
echo "Start execution..."

cd $NNH_HOME/analysis

conda activate env_root_python

python3 launchBDT_bb.py
python3 launchBDT_WW.py

conda desactivate

echo "...execution terminate"
echo

# sauvegarde

echo
echo "Start moving..."

i=1
OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/$branch/processor/run_$i
while [ -d $OUTPUT_DIRECTORY ]; do
    i=$((i+1))
    OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/$branch/processor/run_$i
done 
echo "-> $i no exist"
echo "-> sauvegarde in $OUTPUT_DIRECTORY"
mkdir $OUTPUT_DIRECTORY

#OUTPUT_DIRECTORY=$OUTPUT_DIRECTORY/run_$i
#echo $OUTPUT_DIRECTORY
mv $NNH_PROCESSOR_OUTPUTFILES/* $OUTPUT_DIRECTORY

echo "... moving terminate"
echo
echo "...Terminate nnh processor"
echo
