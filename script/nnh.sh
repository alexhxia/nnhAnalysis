#!/bin/bash

# HELP cmd

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo "input : $NNH_INPUTFILES"
    echo "output : $NNH_OUTPUTFILES"
    echo
    echo "processor output : $NNH_PROCESSOR_OUTPUTFILES"
    echo
    echo "analysis input : $NNH_ANALYSIS_INPUTFILES"
    echo "analysis output : $NNH_ANALYSIS_OUTPUTFILES"
    echo
}

function usage {
    echo
    echo "Run 'processor', 'prepareBDT', and 'analysis'."
    echo
    echo 'Usage : ./nnh.sh [options]'
    echo '-h : print help'
    echo '-p [integer]: number of processus run'
    echo '-a [integer]: number of analysis run'
    echo '-n [directory]: nnhAnalysis directory'
    echo '-b [name]: branch'
    echo '-i [directory]: input directory'
    echo '-o [directory]: output directory'
    echo
    
}

# Stop program with error
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    usage
    exit 1
}

valid_branchs=(original ilcsoft fcc)
function branchValid {
    valid=false
    for b in "${valid_branchs[@]}" ; do
        if [ $b == $1 ] ; then
            valid=true 
        fi
    done
    if ! $valid ; then
        error "-b $branch"
    fi
}

function homeValid {
    if ! [ -d $home ]; then 
        error '-n: home directory no exist'
    elif ! [ -d $home/$branch ]; then 
        error '-n: home/branch directory no exist'
    elif ! [ -d $home/$branch/processor ]; then 
        error '-n: home/branch/processor directory no exist'
    elif  ! [ -d $home/$branch/analysis ]; then 
        error '-n: home/branch/analysis directory no exist'
    fi;
}

# paramètres

home=~/nnhAnalysis/nnhHome
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=/gridgroup/ilc/nnhAnalysisFiles/result

nb_processor=1
nb_bdt=1

while  getopts ":b:p:a:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  usage
            exit 0;;
            
        b)  branch=${OPTARG};;
        
        p)  nb_processor=${OPTARG};;
        
        a)  nb_bdt=${OPTARG};;
        
        n)  home=${OPTARG};;
        
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

branchValid $branch
homeValid

if ! [ -d $input ]; then 
    error '-i: input directory no exist'
fi 

if ! [ -d $output ]; then 
    error '-o: output directory no exist'
fi 

if [[ $nb_processor -lt 0 ]] ; then
    error "-p: $nb_processor < 0"
fi

if [[ $nb_bdt -lt 0 ]] ; then
    error "-a: $nb_bdt < 0"
fi

NNH_HOME=$home/$branch

NNH_INPUTFILES=$input
NNH_OUTPUTFILES=$output/$branch

NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS

NNH_ANALYSIS_INPUTFILES=$NNH_PROCESSOR_OUTPUTFILES
NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA

#print_export

echo
echo "Start nnh on the $branch branch with $nb_bdt BDT for each $nb_processor processors..."
echo

# processor
for ((p = 1; p <= $nb_processor; p++)); do
    echo
    echo "    Start "$p"th processor:"
    
    # OUTPUT DIRECTORY IN SERVER 
    k=1
    OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/run_$k
    while [ -d $OUTPUT_DIRECTORY ]; do
        k=$((k + 1))
        OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/run_$k
    done 
    mkdir -pv $OUTPUT_DIRECTORY
    mkdir $OUTPUT_DIRECTORY/processor
    mkdir $OUTPUT_DIRECTORY/analysis
    
    # RUN PROCESSOR
    if [ $p = 1 ]; then #recompile
        ./processor.sh -n $home -b $branch -i $NNH_INPUTFILES -a
    else
        ./processor.sh -n $home -b $branch -i $NNH_INPUTFILES 
    fi 
    
    # MOVE OUTPUT PROCESSOR DATA IN SERVER 
    mv $NNH_PROCESSOR_OUTPUTFILES/* $OUTPUT_DIRECTORY/processor
    rm -R $NNH_PROCESSOR_OUTPUTFILES
    echo "    Output processor files save in $OUTPUT_DIRECTORY/processor"
    
    # analysis
    
    for ((a = 1; a <= $nb_bdt; a++)); do
        echo
        echo "        Start "$a"th BDT at "$p"th processor: "
        outDir=$OUTPUT_DIRECTORY/analysis/run_"$k"_"$a"
        mkdir -pv $outDir
        ./analysis_prepareBDT.sh -n $NNH_HOME -b $branch -i $OUTPUT_DIRECTORY/processor -o $NNH_ANALYSIS_OUTPUTFILES -a
        ./analysis_launchBDT.sh -n $NNH_HOME -b $branch
        mv $NNH_ANALYSIS_OUTPUTFILES/* $outDir
        echo "        Output analysis files save in $OUTPUT_DIRECTORY/processor"
        rm -R $NNH_ANALYSIS_OUTPUTFILES
    done
    
done

echo
echo "...Terminate nnh"
echo
