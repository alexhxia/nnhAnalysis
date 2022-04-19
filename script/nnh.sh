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

export NNH_HOME=~/nnhAnalysis/$branch

export NNH_INPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
export NNH_OUTPUTFILES=/gridgroup/ilc/nnhAnalysisFiles/result/$branch

export NNH_ANALYSIS_INPUTFILES=NNH_HOME/processor/RESULTS
export NNH_ANALYSIS_BUILD=$NNH_HOME/analysis/BUILD
export NNH_ANALYSIS_OUTPUTFILES=$NNH_HOME/analysis/DATA

valid_branchs=(original ilcsoft fcc)

# HELP cmd
for x in $* ; do
    if [ $x = "-h" ] || [ $x = "--help" ] ; then
        echo "nnh.sh -b branch -p nb_processus -t nb_BDT"
        exit 0
    fi
done 

# paramètres
branch=ilcsoft
nb_processor=1
nb_bdt=1
while getopts h:b:p:a: flag ; do
    case "${flag}" in 
        h)
            echo "nnh.sh -b branch -p nb_processus -t nb_BDT"
            exit 0 ;;
        b)
            branch=${OPTARG}
            valid=false
            for b in "${valid_branchs[@]}" ; do
                if [ $b == $branch ] ; then
                    valid=true 
                fi
            done
            if ! $valid ; then
                #echo "-b $branch : no valid branch"
                exit 1
            fi
            #echo "-b $branch : valid branch"
            ;;
        p)
            nb_processor=${OPTARG} 
            if [[ $nb_processor -lt 0 ]] ; then
                echo "-p $nb_processor : no valid processor nb"
                exit 1
            fi
            ;;
        a)
            nb_bdt=${OPTARG} 
            if [[ $nb_bdt -lt 0 ]] ; then
                echo "-a $nb_bdt : no valid BDT nb"
                exit 1
            fi
            ;;
    esac
done 

echo "branch : $branch"
echo "processor : $nb_processor"
echo "bdt : $nb_bdt"

echo
echo "Start nnh on the $branch branch with $nb_bdt BDT for each $nb_processor processors..."

echo
echo "............................................."
echo

# processor
for ((p = 1; p <= $nb_processor; p++)); do
echo "Start "$p"th processor:"
#./processor

    # analysis
    for ((a = 1; a <= $nb_bdt; a++)); do
        echo "--> Start "$a"th BDT at "$p"th processor:"
        #./analysis_prepareBDT.sh
        #./analysis_launchBDT.sh
        echo
    done
    
    #sauvegarde
    echo "--> Start moving..."
    i=1
    OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/$branch/run_$i
    while [ -d $OUTPUT_DIRECTORY ]; do
        i=$((i+1))
        OUTPUT_DIRECTORY=$NNH_OUTPUTFILES/$branch/run_$i
    done 
    echo "-> sauvegarde in $OUTPUT_DIRECTORY"
    mkdir $OUTPUT_DIRECTORY

    OUTPUT_DIRECTORY=$OUTPUT_DIRECTORY/run_$i
    echo $OUTPUT_DIRECTORY
    mv $NNH_PROCESSOR_OUTPUTFILES/* $OUTPUT_DIRECTORY

    echo
    echo "............................................."
    echo
done

echo "...Terminate nnh"
echo



