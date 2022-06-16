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

def distinctBranchTree(tree1, tree2):
    """
    Return branch name list what are different with Kolmogorov definition.
    
    Parameters:
    -------------------
    tree1 : TTree
    tree2 : TTree
    
    Return :
    -------------------
    nameBranchDistinct : list of dictionary {"branch", "Kolmogorov"}
        
    """
    
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
            nameBranchDistinct.append({
                    "branch" : nameBranch,
                    "Komogorov" : str(k)
            })

        del hist1
        del hist2
            
    return nameBranchDistinct



def outputStream(analysisDistinct):
    """Print stream output"""
    
    print("RESULTS: ")
    
    if len(analysisDistinct) == 0:
        print("\tAnalysis1 and Analysis2 are same.")
    else:
        print("\tAnalysis1 and Analysis2 are distinct for:\n")
        
        keys = analysisDistinct.keys()
        for key in analysisDistinct:
            print("\t" + key + ": " + str(analysisDistinct[key]))
            
            
    
def outputFile(nameFile, pathDir1, pathDir2, analysisDistinct):
    """Do a file output"""
    
    f = open(nameOutputFile, "a")
    f.write("Test 2 directories are same.\n")
    f.write(str(datetime.datetime.now()) + "\n\n")
    f.write("Directory 1:" + pathDir1 + "\n")
    f.write("Directory 2:" + pathDir2 + "\n\n")
    
    if len(analysisDistinct) == 0:
        f.write("\tSame.")
    else:
        f.write("\tDistinct for:\n")
        
        keys = analysisDistinct.keys()
        for key in analysisDistinct:
            print("\t\t" + key + ": " + str(analysisDistinct[key]))
    
    f.write("\n------------------------------------------------------------\n")
    f.close() 


def sortNameFileByTypeFile(nameFileList):
    """ Sort the name files by type file (root, json, ...)"""
    
    nameFileByType = {} # dictionary {key = extension, value = name file list}
    for nameFile in nameFileList:
        
        nameFileSplit = os.path.splitext(nameFile) # split name and extension
        typeFile = nameFileSplit[1] # file extension
        
        if typeFile in nameFileByType: # if typeFile exist then append name file
            t = nameFileByType[typeFile]
            t.append(nameFile)
            nameFileByType.update({typeFile: t})
        else: # create new "key" and add name file in list
            nameFileByType[typeFile] = [nameFile]
            
    return nameFileByType


if __name__ == "__main__":
    
    print("\n----- BEGIN TEST_ANALYSIS_SAME -----\n")

    # PARAMETERS
    
    ## Entry: 2 directories
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-d1', '--directory1', 
            help='Path of analysis directory', 
            required=True)
    
    parser.add_argument(
            '-d2', '--directory2',
            help='Path of analysis directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    pathDir1 = args['directory1']
    testDirectory(pathDir1)
    
    pathDir2 = args['directory2']
    testDirectory(pathDir2)
    
    ## Output
    nameOutputFile = "testAnalysSame.txt"
    
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
            
            if not len(distinctBranchTreeList) == 0:
                analysisDistinct[rootFile] = distinctBranchTreeList
            
            file1.Close()
            file2.Close()
            
    # OUTPUT
    
    ## Stream
    outputStream(analysisDistinct)
            
    ## Files
    outputFile(nameOutputFile, pathDir1, pathDir2, analysisDistinct)
            
    # END
    
    print("\n----- END TEST_ANALYSIS_SAME -----\n")
