#!/usr/bin/env python

""" 
Compare the difference between 2 'analysis' result  directories 
with KolmogorowTest
"""

import numpy 
import argparse 
import os, os.path 
import sys 
import ROOT 
import datetime
import json 

from ROOT import TCanvas, TFile, TH1F, TTree

from tools.compareFile import *
from tools.tools import *


def outputStream(pathDir1, pathDir2, analysisDistinct):
    """Print stream output"""
    
    print("RESULTS: ")
    print("\tDirectory 1: " + pathDir1)
    print("\tDirectory 2: " + pathDir2)
    
    if len(analysisDistinct) == 0:
        print("\t\tSame.")
    else:
        print("\t\tDifferent for:\n")
        
        keys = analysisDistinct.keys()
        for key in analysisDistinct:
            print("\t\t\t" + key + ": " + str(analysisDistinct[key]))

    
def buildOutputFile(outputFile, pathDir1, pathDir2, analysisDistinct):
    """Write result on output file"""
        
    jsonData = {
        "directory1": pathDir1,
        "directory2": pathDir2,
        "date": datetime.datetime.now().isoformat(),
        "analysisDistinct": analysisDistinct
    }
    
    jsonString = json.dumps(
            jsonData, 
            indent = 4, 
            ensure_ascii = False, 
            sort_keys = True)
    
    jsonFile = open(outputFile, "a")
    jsonFile.write(jsonString)
    jsonFile.write("\n")
    jsonFile.close()


if __name__ == "__main__":
    
    print("\n----- BEGIN TEST_ANALYSIS_SAME -----\n")

    # PARAMETERS
    
    ## Entry: 2 directories
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-a1', '--analysis1', 
            help='Path of analysis directory', 
            required=True)
    
    parser.add_argument(
            '-a2', '--analysis2',
            help='Path of analysis directory', 
            required=True)
            
    parser.add_argument(
            '-o', '--output', 
            help='Path to output file', 
            required=False)
            
    args = vars(parser.parse_args())
    
    pathDir1 = args['analysis1']
    testDirectory(pathDir1)
    
    pathDir2 = args['analysis2']
    testDirectory(pathDir2)
        
    ## Get all name files in 2 analysis directories
    
    nameFile1List = os.listdir(pathDir1)
    nameFile2List = os.listdir(pathDir2)
    
    nameFileList = set(nameFile1List + nameFile2List) # set (without double)
    
    # TEST
    
    ## SORT NAME FILE IN THE DICTONARY BY TYPE FILE
    nameFileByType = sortNameFileByTypeFile(nameFileList)
           
    ## TEST BY FILE TYPE
    analysisDistinct = {}
    
    ## For ROOT files
    #print("\nROOT files... ")
    rootFiles = nameFileByType[".root"]

    for rootFile  in rootFiles: 
        print("\nroot File: " + rootFile + "...")
        path_p1 = os.path.join(pathDir1, rootFile)
        path_p2 = os.path.join(pathDir2, rootFile)
    
        #print(rootFile)
        if os.path.exists(path_p1) and os.path.exists(path_p2):
            
            # FOR EACH TREE
            
            file1 = TFile.Open(path_p1)
            file2 = TFile.Open(path_p2)
            
            tree1 = file1.Get("tree")
            tree2 = file2.Get("tree")
            
            distinctBranchTreeList = distinctBranchTree(tree1, tree2)
            
            analysisDistinct[rootFile] = distinctBranchTreeList
            
            file1.Close()
            file2.Close()
            
    # OUTPUT
    
    ## Stream
    outputStream(pathDir1, pathDir2, analysisDistinct)
            
    ## Files
    if args['output']:
        buildOutputFile(args['output'], pathDir1, pathDir2, analysisDistinct)
            
    # END
    
    print("\n----- END TEST_ANALYSIS_SAME -----\n")
