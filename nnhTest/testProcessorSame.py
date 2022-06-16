#!/usr/bin/env python

""" 
Compare the difference between 2 'processor' result  directories 
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

def error(msg):
    """Print error messenger and Stop programme with error"""
    
    print(msg)
    sys.exit(1)


def testDirectory(directory):
    """Test if directory exist."""
    
    if not os.path.exists(directory):
        error('ERROR : ' + directory + ' directory not found')
        
    if not os.path.isdir(directory):
        error('ERROR : directory is not ' + directory)


def distinctBranchTree(tree1, tree2):
    """Return branch name dictionary what are different with Kolmogorov definition."""
    
    nameBranchDistinct = list()
    
    branchs1 = tree1.GetListOfBranches()
    branchs2 = tree2.GetListOfBranches()
    branchs = set(branchs1 + branchs2)
    
    for branch in branchs:
    
        nameBranch = branch.GetName()
        
        tree1.Draw(nameBranch)
        
        htemp = ROOT.gPad.GetPrimitive("htemp")
        xAxis = htemp.GetXaxis()
        xmin = xAxis.GetXmin()
        xmax = xAxis.GetXmax()
    
        hist1 = TH1F("hist1", nameBranch, 200, xmin, xmax)
        hist2 = TH1F("hist2", nameBranch, 200, xmin, xmax)
    
        tree1.Draw(nameBranch + ">>hist1")
        tree2.Draw(nameBranch + ">>hist2")
    
        k = hist1.KolmogorovTest(hist2, "UON")
        if not k == 1.:
            nameBranchDistinct.append({
                    "branch" : nameBranch,
                    "Komogorov" : str(k)
            }

        del hist1
        del hist2
            
    return nameBranchDistinct

def outputStream(processusDistinct):
    """Print result on output stream"""
    
    print("RESULTS: ")
    
    if len(processusDistinct) == 0:
        print("\nProcessus1 and Processus2 are same.")
    else:
        print("\nProcessus1 and Processus2 are distinct for:\n\t" + str(processusDistinct))
        for numP in numProcessus:
            print(str(numP) + ": " + str(processusDistinct[numP]))

def buildOutputFile(outputFile, pathDir1, pathDir2, processusDistinct):
    """Write result on output file"""
        
    jsonData = {
        "directory1": pathDir1,
        "directory2": pathDir2,
        "date": datetime.datetime.now().isoformat(),
        "processusDistinct": processusDistinct
    }
    jsonString = json.dumps(jsonData)
    jsonFile = open(outputFile, "a")
    jsonFile.write(jsonString)
    jsonFile.close()

if __name__ == "__main__":
    
    print("\n----- BEGING TEST_PROCESSOR_SAME -----\n")
    
    # PARAMETERS
    
    ## Input : 2 directories 
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-p1', '--processor1', 
            help='Path of processor directory', 
            required=True)
    
    parser.add_argument(
            '-p2', '--processor2', 
            help='Path of processor directory', 
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
    
    ## analysis directory for test
    pathDir1 = args['processor1']
    testDirectory(pathDir1)
    
    pathDir2 = args['processor2']
    testDirectory(pathDir2)
    
    ## Output: name output file
    nameOutputFile = "testProcessorCompleted.txt"
    
    ## Processus number list
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
        
    nbProcessus = len(numProcessus)    
    
    # add if processus 1 and 2 are different tree
    processusDistinct = {}
    
    # for print, counter of processor remains
    k=1
    
    # FOR EACH PROCESSUS
    
    for numP in numProcessus:
        
        print("[" + str(k) + "/" + str(nbProcessus) + "]"
                + " Processus " + str(numP) + " in progress...")
        numPFileName = str(numP) + ".root"
        path_p1 = os.path.join(pathDir1, numPFileName)
        path_p2 = os.path.join(pathDir2, numPFileName)
        
        if os.path.exists(path_p1) and os.path.exists(path_p2):
            
            # FOR EACH TREE
            
            file1 = TFile.Open(path_p1)
            file2 = TFile.Open(path_p2)
            
            tree1 = file1.Get("tree")
            tree2 = file2.Get("tree")
            
            distinctBranchTreeList = distinctBranchTree(tree1, tree2)
            
            if not len(distinctBranchTreeList) == 0:
                processusDistinct[numP] = distinctBranchTreeList
            
            file1.Close()
            file2.Close()
        
        k = k + 1
    
    # OUTPUT
    
    ## Stream
    outputStream(processusDistinct)
            
    ## Files
    if args['output']:
        buildOutputFile(args['output'], pathDir1, pathDir2, processusDistinct)
    
    # END
    
    print("\n----- END TEST_PROCESSOR_SAME -----\n")
