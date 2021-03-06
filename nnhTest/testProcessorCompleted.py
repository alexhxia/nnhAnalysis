#!/usr/bin/env python

""" 
Test if 'processor' has once root file by processus
"""

import numpy
import argparse 
import os, os.path 
import sys 
import ROOT 
import datetime
import json 

from ROOT import TCanvas, TFile, TH1F, TTree

from tools.tools import *

def outputStream(pathDir, processusMissing):
    """Print result on output stream"""
    
    print("RESULTS for: " + pathDir)
    
    if len(processusMissing) == 0:
        print("\tProcessus is completed.")
    else:
        print("\tProcessus Missing :")
        for p in processusMissing:
            print("\t\t" + str(p))


def buildOutputFile(nameOutputFile, pathDir, processusMissing):
    """Write result on output file"""
        
    jsonData = {
        "pathDirectory": pathDir,
        "date": datetime.datetime.now().isoformat(),
        "numProcessusMissing": processusMissing
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
    
    print("\n----- BEGING TEST_PROCESSOR_COMPLETED -----\n")
    
    # PARAMETERS
    
    ## Entry: directory path to test
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-p', '--processor', 
            help='Path to processor directory', 
            required=True)
     
    parser.add_argument(
            '-s', '--server', 
            help='Path to server input directory', 
            required=False)
                       
    parser.add_argument(
            '-o', '--output', 
            help='Path to output file', 
            required=False)
            
    args = vars(parser.parse_args())
    
    ## processor directory for test
    pathDir = args['processor']
    testInputDirectory(pathDir)
    
    ## num processus list
    numProcessusList = [
        402173, 402182, 402007, 402008, 402176, 402185, 402009, 402010, 
        402011, 402012, 402001, 402002, 402013, 402014, 402003, 402004, 
        402005, 402006, 500006, 500008, 500010, 500012, 500062, 500064, 
        500066, 500068, 500070, 500072, 500074, 500076, 500078, 500080, 
        500082, 500084, 500101, 500102, 500103, 500104, 500105, 500106, 
        500107, 500108, 500110, 500112, 500086, 500088, 500090, 500092, 
        500094, 500096, 500098, 500100, 500113, 500114, 500115, 500116, 
        500117, 500118, 500119, 500120, 500122, 500124, 500125, 500126, 
        500127, 500128
    ]
    
    if args['server']:
        serverDir = args['server']
        try:
            # Get all processus in local server 
            numProcessus = os.listdir(serverDir)
        except:
            numProcessus = numProcessusList
    else:
        numProcessus = numProcessusList
    
    ## num processus list missing
    processusMissing = list()
    
    # TEST : all processus file exit
    for numP in numProcessus:
        
        numPFileName = str(numP) + ".root"
        path = os.path.join(pathDir, numPFileName)
        
        if not os.path.exists(path):
            processusMissing.append(numP)
                        
    # OUTPUT
    
    ## Stream
    outputStream(pathDir, processusMissing)
            
    ## Files
    if args['output']:
        buildOutputFile(args['output'], pathDir, processusMissing)
    
    # END
    
    print("\n----- END TEST_PROCESSOR_COMPLETED -----\n")
