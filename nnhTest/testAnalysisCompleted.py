#!/usr/bin/env python

""" 
Test if "-a path/to/directory" is a analysis directory.
"""

import numpy
import argparse 
import os, os.path 
import sys 
import ROOT 
import datetime
import json 

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

def outputStream(fileMissing):
    """Print result on output stream"""

    print("RESULTS: ")
    
    if len(fileMissing) == 0:
        print("\tAnalysis is completed.")
    else:
        print("\tAnalysis files Missing :")
        for nameFile in fileMissing:
            print("\t" + nameFile + "")


def buildOutputFile(nameOutputFile, pathDir, fileMissing):
    """Write result on output file"""
        
    jsonData = {
        "pathDirectory": pathDir,
        "date": datetime.datetime.now().isoformat(),
        "fileMissing": fileMissing
    }
    jsonString = json.dumps(jsonData)
    jsonFile = open(nameOutputFile, "a")
    jsonFile.write(jsonString)
    jsonFile.write("\n\n")
    jsonFile.close()
    
    
def getAnalysisNameFiles():
    """
    Get name files created by analysis program.
    
    Return:
    ------------------
    nameFileList : set
    """
    
    nameFileList = ["DATA.root"]
    
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
    
    return nameFileList
    
if __name__ == "__main__":
    
    print("\n----- BEGING TEST_ANALYSIS_COMPLETED -----\n")
    
    # PARAMETERS
    
    ## Entry: directory path to test
    parser = argparse.ArgumentParser()
    parser.add_argument(
            '-a', '--analysis', 
            help='Path to analysis directory', 
            required=True)
                       
    parser.add_argument(
            '-o', '--output', 
            help='Path to output file', 
            required=False)
    
    args = vars(parser.parse_args())
    
    ## analysis directory for test
    pathDir = args['analysis']
    testDirectory(pathDir)
    
    ## output file
    if args['output']:
        outputFile = args['output']
    else:
        outputFile = "testAnalysisCompleted.json"
    
    ## list of files created by analysis program
    nameFileList = getAnalysisNameFiles()

    ## list analysis file which missing
    fileMissing = list()

    # TEST 
    for nameFile in nameFileList:
        
        path = os.path.join(pathDir, nameFile)
        
        if not os.path.exists(path):
            fileMissing.append(nameFile)
            #print("Analysis files " + str(nameFile) + " missing")
        #else:
            #print("Analysis files " + str(nameFile) + " exist")
                        
    
    # OUTPUT 
    
    ## Stream
    outputStream(fileMissing)
            
    ## Files
    if args['output']:
        buildOutputFile(outputFile, pathDir, fileMissing)
    
    # END
    
    print("\n----- END TEST_ANALYSIS_COMPLETED -----\n")
