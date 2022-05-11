#!/bin/bash

# Stop program with error
# Entry: error message
function error {
    echo
    echo 'Error: no valid option!'
    echo $1
    exit 1
}

