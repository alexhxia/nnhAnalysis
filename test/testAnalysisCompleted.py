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
import enum

from ROOT import TCanvas, TFile, TH1F, TTree

from tools.tools import *

def outputStream(pathDir, fileMissing):
    """Print result on output stream"""

    print("RESULTS for: " + pathDir)
    
    if len(fileMissing) == 0:
        print("\tAnalysis is completed.")
    else:
        print("\tAnalysis files Missing :")
        for nameFile in fileMissing:
            print("\t\t" + nameFile + "")


def buildOutputFile(nameOutputFile, pathDir, fileMissing):
    """Write result on output file"""
        
    jsonData = {
        "pathDirectory": pathDir,
        "date": datetime.datetime.now().isoformat(),
        "fileMissing": fileMissing
    }
    
    jsonString = json.dumps(
            jsonData, 
            indent = 4, 
            ensure_ascii = False, 
            sort_keys = True)
    
    jsonFile = open(nameOutputFile, "a")
    jsonFile.write(jsonString)
    jsonFile.write("\n")
    jsonFile.close()



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
    outputStream(pathDir, fileMissing)
            
    ## Files
    if args['output']:
        buildOutputFile(outputFile, pathDir, fileMissing)
    
    # END
    
    print("\n----- END TEST_ANALYSIS_COMPLETED -----\n")
