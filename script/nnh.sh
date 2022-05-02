#!/bin/bash

### FUNCTION TOOL ###

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

# Display Help
function syntax {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'SYNTAX:'
    echo '    ./nnh.sh [options]'
    echo
    echo 'OPTIONS:'
    echo '   -h                print help'
    echo
    echo '   -p [integer]      number of processus run'
    echo "                     DEFAULT VALUE: $nb_processus"
    echo
    echo '   -a [integer]      number of analysis run'
    echo "                     DEFAULT VALUE: $nb_analysis"
    echo
    echo '   -b [name]         branch'
    echo "                     DEFAULT VALUE: $branch"
    echo
    echo '   -n [directory]    nnhAnalysis directory'
    echo "                     DEFAULT VALUE: $home"
    echo
    echo '   -i [directory]    input directory'
    echo "                     DEFAULT VALUE: $input"
    echo
    echo '   -o [directory]    output directory'
    echo "                     DEFAULT VALUE: $output"
    echo
    
}

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    syntax
    exit 1
}

# Test if name project exist (no use now)
function test_isValidBranch {
    valid=false
    for b in "${branchsValid[@]}" ; do
        if [ $b == $1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-b $branch"
    fi
}

# Test if the directory home is valid
function test_isValidHome {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
    elif ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    fi
}

# Test if the directory input is valid 
function test_isValidInputDirectory {
    if ! [ -d $input ]; then 
        error '-i: input directory no exist'
    fi
}

# Test if the directory output is valid
function test_isValidOutputDirectory {
    if ! [ -d $output ]; then 
        error '-o: output directory no exist'
    fi
}

### PARAMETER ###

valid_branchs=(original ilcsoft fcc)
branch=ilcsoft
home=~/nnhAnalysis/nnhHome
script=~/nnhAnalysis/script
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=/gridgroup/ilc/nnhAnalysisFiles/result
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

NNH_HOME=$home/$branch

NNH_INPUT=$input
NNH_OUTPUT=$output/$branch

NNH_PROCESSOR_INPUT=$NNH_INPUT
NNH_PROCESSOR_OUTPUT=$NNH_HOME/processor/RESULTS

NNH_ANALYSIS_INPUT=$NNH_PROCESSOR_OUTPUT
NNH_ANALYSIS_OUTPUT=$NNH_HOME/analysis/DATA

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
