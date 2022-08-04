#!/bin/bash

# Display Help
# Entry: option list
function syntaxOption {
    
    echo
    echo 'OPTIONS:'
    
    for option in "$@"
    do
        case "${option}" in
            
            # Without argument
            h)  echo '   -h                print help';;
            
            c)  echo '   -c                force build then run program';;
            
            v)  echo '   -v                print details';;
            
            # With argument
            
            ## branch
            b)  echo '   -b [name]         branch (name projet) - example : fcc, ilcsoft'
                echo "                     DEFAULT VALUE: $branch";;
            
            ## home
            n)  echo '   -n [directory]    nnhAnalysis directory'
                echo "                     DEFAULT VALUE: $path";;
            
            ## input directories
            i)  echo '   -i [directory]    input directory on server'
                echo "                     DEFAULT VALUE: $input";;
            
            j)  echo '   -j [directory]    input processor directory'
                echo "                     DEFAULT VALUE: $p_input";;
            
            k)  echo '   -k [directory]    input analysis directory'
                echo "                     DEFAULT VALUE: $a_input";;
            
            ## output directories
            o)  echo '   -o [directory]    output directory, where you want new file on server'
                echo "                     DEFAULT VALUE: $output";;
            
            m)  echo '   -m [directory]    output processor directory'
                echo "                     DEFAULT VALUE: $p_output";;
            
            q)  echo '   -q [directory]    output analysis directory'
                echo "                     DEFAULT VALUE: $a_output";;

            ## multi-program
            p)  echo '   -p [integer]      number of processus run'
                echo "                     DEFAULT VALUE: $nb_runProcessor" ;;   

            a)  echo '   -a [integer]      number of prepareBDT run'
                echo "                     DEFAULT VALUE: $nb_BDT";;
            
            ## other
            *)  echo 'option unidentified';;
        esac
        
        echo
    done
}
