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

home=~/nnhAnalysis
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=/gridgroup/ilc/nnhAnalysisFiles/result

p_input=$input
p_output=$NNH_HOME/processor/RESULTS
p_build=$NNH_HOME/processor/BUILD

a_input=$NNH_HOME/analysis/DATA
a_output=$NNH_HOME/processor/RESULTS
a_build=$NNH_HOME/analysis/BUILD

export NNH_HOME=$home/$branch

export NNH_INPUTFILES=$input
export NNH_OUTPUTFILES=$output/$branch

export NNH_PROCESSOR_INPUTFILES=$p_input 
export NNH_PROCESSOR_BUILD=$p_build
export NNH_PROCESSOR_OUTPUTFILES=$p_output

export NNH_ANALYSIS_INPUTFILES=$a_input
export NNH_ANALYSIS_BUILD=a_output
export NNH_ANALYSIS_OUTPUTFILES=$a_output
