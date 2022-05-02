#!/bin/bash

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

export NNH_INPUT=$input
export NNH_OUTPUT=$output/$branch

export NNH_PROCESSOR_INPUT=$p_input 
export NNH_PROCESSOR_BUILD=$p_build
export NNH_PROCESSOR_OUTPUT=$p_output

export NNH_ANALYSIS_INPUT=$a_input
export NNH_ANALYSIS_BUILD=a_output
export NNH_ANALYSIS_OUTPUT=$a_output
