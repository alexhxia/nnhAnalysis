# Create all directories for sort output test files by branch type

source ~/nnhAnalysis/nnhScript/nnhExport.sh
nnh_export

# loop on branch in $NNH_PROGRAM
for branch1 in $(ls $NNH_PROGRAM --hide=*.md); do

    # one directory by branch
    mkdir -p $NNH_RESULT/complete/"$branch1"/processor
    mkdir -p $NNH_RESULT/complete/"$branch1"/analysis
    
    # loop on branch in $NNH_PROGRAM 
    for branch2 in $(ls $NNH_PROGRAM --hide=*.md); do
    
        # comparison is symmetrical
        dir1=$NNH_RESULT/same/"$branch1"'_VS_'"$branch2" 
        dir2=$NNH_RESULT/same/"$branch2"'_VS_'"$branch1" 
        if [ ! -d "$dir1" ] && [ ! -d "$dir2" ]; then
            mkdir -p "$dir2"/processor
            mkdir -p "$dir2"/analysis
        fi
    done
done
