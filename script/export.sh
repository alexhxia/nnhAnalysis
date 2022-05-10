#!/bin/bash

home=~/nnhAnalysis/nnhHome
branch=original
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=/gridgroup/ilc/nnhAnalysisFiles/result

p_input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
p_output=$home/$branch/processor/RESULTS
a_input=$p_output
a_output=$home/$branch/analysis/DATA

function print_export {
    echo
    echo "home :             $NNH_HOME"
    echo "input :            $NNH_INPUT"
    echo "output :           $NNH_OUTPUT"
    echo
    echo "processor input :  $NNH_PROCESSOR_INPUT"
    echo "processor output : $NNH_PROCESSOR_OUTPUT"
    echo
    echo "analysis input :   $NNH_ANALYSIS_INPUT"
    echo "analysis output :  $NNH_ANALYSIS_OUTPUT"
    echo
}

function nnh_export {
    
    # HOME
    export NNH_HOME=$home/$branch

    # SERVER
    export NNH_INPUT=$input
    export NNH_OUTPUT=$output/$branch

    # FOR RUN PROCESSOR
    export NNH_PROCESSOR_INPUT=$p_input
    export NNH_PROCESSOR_OUTPUT=$p_output

    # FOR RUN ANALYSIS
    export NNH_ANALYSIS_INPUT=$a_input
    export NNH_ANALYSIS_OUTPUT=$a_output
}

#nnh_export
#print_export
