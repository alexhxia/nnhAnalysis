#!/bin/bash
#
#SBATCH --job-name=nnh
#SBATCH --output=nnh_submit.out
#SBATCH --error=nnh_submit.err
#
#SBATCH --ntasks=1
#SBATCH --time=3-00:00:00          # means 48h 00m 00s
#SBATCH --mem-per-cpu=1G
# mail-type=BEGIN, END, FAIL, REQUEUE, ALL, STAGE_OUT, TIME_LIMIT_90
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.hocine@ip2i.in2p3.fr

# HELP cmd

function print_export {
    echo
    echo "home : $NNH_HOME"
    echo "input : $NNH_INPUTFILES"
    echo "output : $NNH_OUTPUTFILES"
    echo
    echo "processor input : $NNH_PROCESSOR_INPUTFILES"
    echo "processor output : $NNH_PROCESSOR_OUTPUTFILES"
    echo "processor build : $NNH_PROCESSOR_BUILD"
    echo
    echo "analysis input : $NNH_ANALYSIS_INPUTFILES"
    echo "analysis output : $NNH_ANALYSIS_OUTPUTFILES"
    echo "analysis build : $NNH_ANALYSIS_BUILD"
    echo
}

function usage {
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

home=~/nnhAnalysis
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=/gridgroup/ilc/nnhAnalysisFiles/result

nb_processor=1
nb_bdt=1

while  getopts ":b:p:a:n:i:o:h" option ; do
    case "${option}" in 
    
        h)  usage && exit 0;;
        b)  branch=${OPTARG};;
        p)  nb_processor=${OPTARG};;
        a)  nb_bdt=${OPTARG};;
        n)  home=${OPTARG};;
        i)  input=${OPTARG};;
        o)  output=${OPTARG};;
        *) error 'option no exist';;
    esac
done 

homeValid
branchValid $branch

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

NNH_PROCESSOR_INPUTFILES=$NNH_INPUTFILES 
NNH_PROCESSOR_OUTPUTFILES=$NNH_HOME/processor/RESULTS

NNH_ANALYSIS_INPUTFILES=$NNH_PROCESSOR_OUTPUTFILES
NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA

#print_export

echo
echo "Start nnh on the $branch branch with $nb_bdt BDT for each $nb_processor processors..."
echo

# processor
for ((p = 1; p <= $nb_processor; p++)); do
    echo "Start "$p"th processor:"
    
    # OUTPUT DIRECTORY IN SERVER 
    k=1
    OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/run_$k
    while [ -d $OUTPUT_DIRECTORY ]; do
        k=$((k+1))
        OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/run_$k
    done 
    mkdir -pv $OUTPUT_DIRECTORY
    mkdir $OUTPUT_DIRECTORY/processor
    mkdir $OUTPUT_DIRECTORY/analysis
    
    # RUN PROCESSOR
    if [ $p = 1 ]; then #recompile
        ./processor.sh -n $home -b $branch -i $NNH_PROCESSOR_INPUTFILES -c
    else
        ./processor.sh -n $home -b $branch -i $NNH_PROCESSOR_INPUTFILES 
    fi 
    
    # COPY SERVER PROCESSOR
    cp $NNH_PROCESSOR_OUTPUTFILES/*.root $OUTPUT_DIRECTORY/processor
    mv $NNH_PROCESSOR_OUTPUTFILES/*_output.txt $OUTPUT_DIRECTORY/processor
    echo "Output processor files save in $OUTPUT_DIRECTORY/processor"
    echo
    
    # analysis
    for ((a = 1; a <= $nb_bdt; a++)); do
        echo "--> Start "$a"th BDT at "$p"th processor:"
        mkdir -pv $OUTPUT_DIRECTORY/analysis/run_$k_$a
        echo "$k : $OUTPUT_DIRECTORY/analysis/run_$k_$a"
        ./analysis_prepareBDT.sh -c $NNH_HOME -b $branch -i $NNH_ANALYSIS_INPUTFILES -o $NNH_ANALYSIS_OUTPUTFILES
        ./analysis_launchBDT.sh $NNH_HOME -b $branch
        cp $NNH_ANALYSIS_OUTPUTFILES/* $OUTPUT_DIRECTORY/analysis/run_$k_$a
        echo "Output analysis files save in $OUTPUT_DIRECTORY/processor"
        echo
        rm -R $NNH_ANALYSIS_OUTPUTFILES
    done
    
    # DELETE OUTPUT DIRECTORY PROCESSOR
    rm -R $NNH_PROCESSOR_OUTPUTFILES
done

echo "...Terminate nnh"
echo
echo "Output files is in directory : $OUTPUT_DIRECTORY"
echo



