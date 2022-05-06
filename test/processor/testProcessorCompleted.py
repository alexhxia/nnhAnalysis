#!/usr/bin/env python

""" 
Test if 'processor' has once root file by processus
"""

import numpy
import argparse 
import os, os.path 
import sys 
import ROOT 

from ROOT import TCanvas, TFile, TH1F, TTree

def error(msg):
    """Print error messenger and Stop programme with error"""
    
    print(msg)
    sys.exit(1)

def testInputDirectory(directory):
    """Test if input directory exist."""
    
    if not os.path.exists(directory):
        error('ERROR : ' + directory + ' directory not found')
        
    if not os.path.isdir(directory):
        error('ERROR : directory is not ' + directory)

if __name__ == "__main__":
    
    print("\n----- BEGING TEST_PROCESSOR_COMPLETED -----\n")
    
    # PARAMETERS
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-p', '--processor', 
            help='Path of processor directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    pDirectory = args['processor']
    testInputDirectory(pDirectory)
    
    # FOR ALL PROCESSUS
    
    numProcessus = [
        402173, 402182, 402007, 402008, 402176, 402185, 402009, 402010, 402011, 
        402012, 402001, 402002, 402013, 402014, 402003, 402004, 402005, 402006, 
        500006, 500008, 500010, 500012, 500062, 500064, 500066, 500068, 500070, 
        500072, 500074, 500076, 500078, 500080, 500082, 500084, 500101, 500102, 
        500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112, 500086, 
        500088, 500090, 500092, 500094, 500096, 500098, 500100, 500113, 500114, 
        500115, 500116, 500117, 500118, 500119, 500120, 500122, 500124, 500125, 
        500126, 500127, 500128#, 454865
    ]
    
    # add num processus if is missing
    processusMissing = list()
    
    # add if processus 1 and 2 are different tree
    processusDistinct = {}
    
    for numP in numProcessus:
        
        numPFileName = str(numP) + ".root"
        path = os.path.join(pDirectory, numPFileName)
        
        if not os.path.exists(path):
            processusMissing.append(numP)
            print("Processus " + str(numP) + " missing")
        else:
            print("Processus " + str(numP) + " exist")
                        
    
    # OUTPUT
    
    print("\n---- RESULTS -----")
    
    if len(processusMissing) == 0:
        print("\nProcessus is complete.")
    else:
        print("\nProcessus Missing :\n\t" + str(processusMissing))
    
    print("\n----- END TEST_PROCESSOR_COMPLETED -----\n")
