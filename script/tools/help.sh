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
            
            c)  echo '   -c                build then run program';;
            
            d)  echo '   -d                desactivate conda command';;
            #d)  echo '   -d                activate conda command';;
            
            # With argument
            b)  echo '   -b [name]         branch (name projet) - example : fcc, ilcsoft'
                echo "                     DEFAULT VALUE: $branch";;
            
            n)  echo '   -n [directory]    nnhHome directory'
                echo "                     DEFAULT VALUE: $home";;
            
            i)  echo '   -i [directory]    input directory'
                echo "                     DEFAULT VALUE: $input";;
            
            o)  echo '   -o [directory]    output directory, where you want new file'
                echo "                     DEFAULT VALUE: $output";;

            p)  echo '   -p [integer]      number of processus run'
                echo "                     DEFAULT VALUE: $nb_runProcessor" ;;   

            t)  echo '   -t [integer]      number of prepareBDT run'
                echo "                     DEFAULT VALUE: $nb_BDT";;
                
            a)  echo '   -a [integer]      number of launchBDT run by BDT'
                echo "                     DEFAULT VALUE: $nb_runByBDT";;
            
            *) echo 'option unidentified';;
        esac
        
        echo
    done
}
