#!/bin/bash

### FUNCTION TOOL ###

source export.sh
source help.sh

# Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./nnh.sh [options]'
    echo
    
    syntaxOption h p a b n i o #help.sh
}

### PARAMETER ###

nb_processor=1
nb_analysis=1

while  getopts ":b:p:a:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  syntax
            exit 0;;
            
        b)  branch=${OPTARG};;
        
        p)  nb_processor=${OPTARG};;
        
        a)  nb_analysis=${OPTARG};;
        
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

if [[ $nb_processor -lt 0 ]] ; then
    error "-p: $nb_processor < 0"
fi

if [[ $nb_analysis -lt 0 ]] ; then
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
pwd
### processor ###
for ((p = 1; p <= $nb_processor; p++)); do
    echo
    echo "    Start "$p"th processor:"
    
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
    
    for ((a = 1; a <= $nb_analysis; a++)); do
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
