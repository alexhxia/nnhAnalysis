#!/usr/bin/env python

""" 
Test if "-a path/to/directory" is a analysis directory.
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

def testDirectory(directory):
    """Test if input directory exist."""
    
    if not os.path.exists(directory):
        error('ERROR : ' + directory + ' directory not found')
        
    if not os.path.isdir(directory):
        error('ERROR : directory is not ' + directory)

if __name__ == "__main__":
    
    print("\n----- BEGING TEST_ANALYSIS_COMPLETED -----\n")
    
    # PARAMETERS
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-a', '--analysis', 
            help='Path of analysis directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    aDirectory = args['analysis']
    testDirectory(aDirectory)
    
    # Files created by analysis program
    
    nameFileList = [
        "DATA.root"
    ]
    
    particleTypeList = ["ww", "bb"]
    
    for particleType in particleTypeList:
    
        nameFileList_p = [
            "bestSelection_" + particleType + "_e-0.8_p+0.3.root", 
            "split_" + particleType + "_e+0_p+0.root",
            "split_" + particleType + "_e-0.8_p+0.3.root",
            "scores_" + particleType + "_e-0.8_p+0.3.root",
            "stats_" + particleType + "_e-0.8_p+0.3.json",
            "model_" + particleType + "_e-0.8_p+0.3.joblib"
        ]
        
        nameFileList = nameFileList + nameFileList_p

    # list analysis file which missing
    fileMissing = list()
    
    for nameFile in nameFileList:
        
        path = os.path.join(aDirectory, nameFile)
        
        if not os.path.exists(path):
            fileMissing.append(nameFile)
            print("Analysis files " + str(nameFile) + " missing")
        else:
            print("Analysis files " + str(nameFile) + " exist")
                        
    
    # OUTPUT STREAM
    
    print("\n---- RESULTS -----")
    
    if len(fileMissing) == 0:
        print("\nAnalysis is complete.")
    else:
        print("\nAnalysis files Missing :\n\t" + str(fileMissing))
            
    # OUTPUT FILES 
    
    f = open("testAnalysis_isCompleted.txt", "a")
    f.write("Test directory " + aDirectory + "containt all files created by analysis program.\n")
    
    if len(fileMissing) == 0:
        f.write("Completed.")
    else:
        for nameFile in fileMissing:
            f.write(nameFile + " missing.")
            
    f.write("\n------------------------------------------------------------\n")
    f.close() 
    
    # END
    
    print("\n----- END TEST_ANALYSIS_COMPLETED -----\n")
