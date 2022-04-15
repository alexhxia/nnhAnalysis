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
echo "Start nnh..."

./processor

./analysis_prepareBDT.sh

./analysis_launchBDT.sh

echo "...Terminate nnh"
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
