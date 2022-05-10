#!/bin/bash

### FUNCTION TOOL ###

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
source export.sh 
source help.sh

# Display Help
function syntax {
    echo
    echo "Run 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./nnhAnalysis.sh [options]'
    echo
    
    syntaxOption h d p a b n i o #help.sh
}

### PARAMETER ###

# look environement parameters default in export.sh
num_run=1
nb_BDT=1
nb_runByBDT=1

while  getopts ":b:a:d:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  branch=${OPTARG};;
                
        d)  nb_BDT=${OPTARG};;
        
        a)  nb_runByBDT=${OPTARG};;
        
        n)  home=${OPTARG};;
        
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

# test_isValidBranch $branch
test_isValidHome
test_isValidInputDirectory
test_isValidOutputDirectory

if [[ $nb_BDT -lt 0 ]] ; then
    error "-p: $nb_processor < 0"
fi

if [[ $nb_runByBDT -lt 0 ]] ; then
    error "-a: $nb_bdt < 0"
fi

nnh_export
print_export
exit 0

#print_export
echo "Option OK"
echo
echo "Start nnh on the $branch branch with $nb_bdt BDT for each $nb_processor processors..."
cd $script

### prepare BDT ###
for ((i = 1; i <= $nb_BDT; i++)); do
    echo
    echo "    Start "$i"th BDT:"
    
    # OUTPUT DIRECTORY IN SERVER 
    k=1
    OUTPUT_DIRECTORY=$NNH_OUTPUT/run_$k
    while [ -d $OUTPUT_DIRECTORY ]; do
        k=$((k + 1))
        OUTPUT_DIRECTORY=$NNH_OUTPUT/run_$k
    done 
    mkdir -v $OUTPUT_DIRECTORY $OUTPUT_DIRECTORY/processor $OUTPUT_DIRECTORY/analysis
    
    # RUN PROCESSOR
    if [ $p = 1 ]; then #recompile
        ./processor.sh -n $home -b $branch -i $NNH_INPUT -c
    else
        ./processor.sh -n $home -b $branch -i $NNH_INPUT
    fi 
    
    # MOVE OUTPUT PROCESSOR DATA IN SERVER 
    mv $NNH_PROCESSOR_OUTPUT/* $OUTPUT_DIRECTORY/processor
    rm -R $NNH_PROCESSOR_OUTPUT
    echo "    Output processor files save in $OUTPUT_DIRECTORY/processor"
    
    ### analysis ###
    
    for ((a = 1; a <= $nb_runByBDT; a++)); do
        echo
        echo "    Start "$a"th BDT at "$p"th processor: "
        outDir=$OUTPUT_DIRECTORY/analysis/run_"$k"_"$a"
        mkdir -pv $outDir
        echo "    Prepare BDT..."
        ./analysis_prepareBDT.sh -n $NNH_HOME -b $branch -i $OUTPUT_DIRECTORY/processor -o $NNH_ANALYSIS_OUTPUT -c
        echo "    Launch  BDT..."
        ./analysis_launchBDT.sh -n $NNH_HOME -b $branch
        mv $NNH_ANALYSIS_OUTPUT/* $outDir
        echo "    Output analysis files save in $OUTPUT_DIRECTORY/processor"
        rm -R $NNH_ANALYSIS_OUTPUT
    done
done

echo
echo "...Terminate nnh"
echo
