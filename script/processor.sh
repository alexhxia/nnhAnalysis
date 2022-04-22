#!/bin/bash
#
#SBATCH --job-name=test
#SBATCH --output=submit.out
#SBATCH --error=submit.err
#
#SBATCH --ntasks=1
#SBATCH --time=1:00:00          # means 1h 00m 00s
#SBATCH --mem-per-cpu=1G
# mail-type=BEGIN, END, FAIL, REQUEUE, ALL, STAGE_OUT, TIME_LIMIT_90
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.hocine@ip2i.in2p3.fr

function usage {
    echo
    echo 'Usage : ./nnh.sh [options]'
    echo '-h : print help'
    echo '-c : recompile'
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

# paramètres
valid_branchs=(original ilcsoft fcc)

home=~/nnhAnalysis
branch=ilcsoft
input=/gridgroup/ilc/nnhAnalysisFiles/AHCAL
output=$home/$branch/processor/RESULTS
recompile=1

while getopts hcn:b:i:o: flag ; do
    case "${flag}" in 
    
        h)  usage
            exit 0;;
            
        c)  recompile=0;;
        
        n)  home=${OPTARG};;
        
        b)  branch=${OPTARG}
            valid=false
            for b in "${valid_branchs[@]}" ; do
                if [ $b == $branch ] ; then
                    valid=true 
                fi
            done
            if ! $valid ; then
                exit 1
            fi
            ;;
            
        i)  input=${OPTARG};;
        
        o)  output=${OPTARG};;
        
        *) error 'option no exist';;
    esac
done 

# environnement

source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh
export NNH_HOME=$home/$branch
export NNH_PROCESSOR_INPUTFILES=$input 
export NNH_PROCESSOR_OUTPUTFILES=$output
export MARLIN_DLL=$MARLIN_DLL:$NNH_HOME/processor/lib/libnnhProcessor.so

# compilation

if [ $recompile -eq 0 ]; then
    if [ -d $NNH_HOME/processor/BUILD ]; then 
        rm -R $NNH_HOME/processor/BUILD
    fi

    mkdir -v $NNH_HOME/processor/BUILD
    cd $NNH_PROCESSOR_BUILD
    cmake -C $ILCSOFT/ILCSoft.cmake .. 
    make
    make install
    echo
fi

# exécution

if [ -d $NNH_PROCESSOR_OUTPUTFILES ]; then 
    #rm -R $NNH_PROCESSOR_OUTPUTFILES
fi    
mkdir -v $NNH_PROCESSOR_OUTPUTFILES

python3 $NNH_HOME/processor/script/launchNNHProcessor.py -p 402004 -i $NNH_PROCESSOR_INPUTFILES -o $NNH_PROCESSOR_OUTPUTFILES

