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
    
    print("\n----- BEGING TEST_ANALYSIS_COMPLETED -----\n")
    
    # PARAMETERS
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-a', '--analysis', 
            help='Path of analysis directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    aDirectory = args['analysis']
    testInputDirectory(aDirectory)
    
    # FOR ALL PROCESSUS
    
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

    # add num processus if is missing
    fileMissing = list()
    
    for nameFile in nameFileList:
        
        path = os.path.join(aDirectory, nameFile)
        
        if not os.path.exists(path):
            fileMissing.append(nameFile)
            print("Analysis files " + str(nameFile) + " missing")
        else:
            print("Analysis files " + str(nameFile) + " exist")
                        
    
    # OUTPUT
    
    print("\n---- RESULTS -----")
    
    if len(fileMissing) == 0:
        print("\nAnalysis is complete.")
    else:
        print("\nAnalysis files Missing :\n\t" + str(fileMissing))
    
    print("\n----- END TEST_ANALYSIS_COMPLETED -----\n")
