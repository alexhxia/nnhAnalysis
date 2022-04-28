#!/usr/bin/env python

""" 
Ce programme compare les résultats de plusieurs dossiers de sortie du programme 
launchBDT_xx.py.
"""

import argparse
import os, os.path
import sys

def testEnv():
    """Test if variable env is set."""
    
    if 'NNH_HOME' not in os.environ:
        print('ERROR : env variable NNH_HOME is not set')
        sys.exit(1)

def testDirectory(directory):
    """Test if input directory exist."""
    
    if not os.path.exists(directory):
        print('ERROR : directory not found')
        sys.exit(1)
        
    if not os.path.isdir(directory):
        print('ERROR :  is not directory')
        sys.exit(1)

if __name__ == "__main__":
    
    testEnv()
    NNH_HOME = os.environ['NNH_HOME']
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-i', '--inputDirectory', 
            help='Path of input directory', 
            #dest="inputDirectory",
            required=True)
    
    parser.add_argument(
            '-o', '--outputDirectory', 
            help='Path of output directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    inputDirectory = args['inputDirectory']
    testDirectory(inputDirectory)
    
    outputDirectory = args['outputDirectory']
    testDirectory(outputDirectory)

    
    runDirectories = os.listdir(inputDirectory)
    for runDirectory in runDirectories:
        runDirectory = os.path.join(inputDirectory, runDirectory)
        if not os.path.isdir(runDirectory):
            print('ERROR : ' + runDirectory + ' is not directory')
        else:
            print(runDirectory)
        
    #print(sys.argv)
    #print(parser.parse_args())
    
    #print("Hello, world")
