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

def distinctBranchTree(tree1, tree2):
    """Return branch name list what are different with Kolmogorov definition."""
    
    nameBranchDistinct = list()
    
    branchs1 = tree1.GetListOfBranches()
    branchs2 = tree2.GetListOfBranches()
    branchs = set(branchs1 + branchs2)
    
    for branch in branchs:
    
        nameBranch = branch.GetName()
        
        tree1.Draw(nameBranch)
        
        htemp=ROOT.gPad.GetPrimitive("htemp")
        xAxis=htemp.GetXaxis()
        xmin=xAxis.GetXmin()
        xmax=xAxis.GetXmax()
    
        hist1 = TH1F("hist1", nameBranch, 200, xmin, xmax)
        hist2 = TH1F("hist2", nameBranch, 200, xmin, xmax)
    
        tree1.Draw(nameBranch + ">>hist1")
        tree2.Draw(nameBranch + ">>hist2")
    
        k = hist1.KolmogorovTest(hist2, "UON")
        if not k == 1.:
            nameBranchDistinct.append(nameBranch)

        del hist1
        del hist2
            
    return nameBranchDistinct

if __name__ == "__main__":
    
    print("\n----- BEGIN TEST_2ANALYSIS -----\n")

    
    # PARAMETERS
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-a1', '--analysis1', 
            help='Path of analysis directory', 
            required=True)
    
    parser.add_argument(
            '-a2', '--analysis2', 
            help='Path of analysis directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    a1Directory = args['analysis1']
    testInputDirectory(a1Directory)
    
    a2Directory = args['analysis2']
    testInputDirectory(a2Directory)
    
    # Get all file name in 2 analysis directory
    
    nameFile1List = os.listdir(a1Directory)
    nameFile2List = os.listdir(a2Directory)
    
    nameFileList = set(nameFile1List + nameFile2List)
    
    # Sort file by type in dictionary
    nameFileByType = {}
    
    for nameFile in nameFileList:
        
        nameFileSplit = os.path.splitext(nameFile)
        
        typeFile=nameFileSplit[1]
        
        if typeFile in nameFileByType:
            t = nameFileByType[typeFile]
            t.append(nameFile)
            nameFileByType.update({typeFile: t})
        else:
            nameFileByType[typeFile] = [nameFile]
           
    # TEST BY FILE TYPE
    analysisDistinct = []
    
    # ROOT files
    
    print("\nROOT files... ")
    rootFiles = nameFileByType[".root"]
    for rootFile  in rootFiles:
        path_p1 = os.path.join(a1Directory, rootFile)
        path_p2 = os.path.join(a2Directory, rootFile)
    
        print(rootFile)
        if os.path.exists(path_p1) and os.path.exists(path_p2):
            
            # FOR EACH TREE
            
            file1 = TFile.Open(path_p1)
            file2 = TFile.Open(path_p2)
            
            tree1 = file1.Get("tree")
            tree2 = file2.Get("tree")
            
            distinctBranchTreeList = distinctBranchTree(tree1, tree2)
            
            if not len(distinctBranchTreeList) == 0:
                analysisDistinct[rootFile] = distinctBranchTreeList
            
            file1.Close()
            file2.Close()
    
    # OUTPUT
    
    print("\n---- RESULTS -----")
    
    if len(analysisDistinct) == 0:
        print("\nAnalysis1 and Analysis2 are same.")
    else:
        print("\nAnalysis1 and Analysis2 are distinct for:\n\t" + str(analysisDistinct))
    
    print("\n----- END TEST_2ANALYSIS -----\n")
