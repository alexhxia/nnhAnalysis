#!/usr/bin/env python

""" 
Ce programme compare les résultats de 2 dossiers de sortie du programme 'processor'.
"""

import argparse 
import os, os.path 
import sys 
import ROOT 

from ROOT import TFile, TH1F, TTree

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
    
    parser = argparse.ArgumentParser()
    
    parser.add_argument(
            '-p1', '--processor1', 
            help='Path of processor directory', 
            required=True)
    
    parser.add_argument(
            '-p2', '--processor2', 
            help='Path of processor directory', 
            required=True)
            
    args = vars(parser.parse_args())
    
    p1Directory = args['processor1']
    testInputDirectory(p1Directory)
    
    p2Directory = args['processor2']
    testInputDirectory(p2Directory)
    
    numProcessus = [
        402173, 402182, 402007, 402008, 402176, 402185, 402009, 402010, 402011, 
        402012, 402001, 402002, 402013, 402014, 402003, 402004, 402005, 402006, 
        500006, 500008, 500010, 500012, 500062, 500064, 500066, 500068, 500070, 
        500072, 500074, 500076, 500078, 500080, 500082, 500084, 500101, 500102, 
        500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112, 500086, 
        500088, 500090, 500092, 500094, 500096, 500098, 500100, 500113, 500114, 
        500115, 500116, 500117, 500118, 500119, 500120, 500122, 500124, 500125, 
        500126, 500127, 500128
    ]
    
    #for numP in numProcessus:
    numP = 402173
    numP = str(numP) + ".root"
    path_p1 = os.path.join(p1Directory, numP)
    path_p2 = os.path.join(p2Directory, numP)
    if not os.path.exists(path_p1) or not os.path.exists(path_p2) :
        print(numP + "no exist for 2")
    else :
        print(numP)
        file1 = TFile.Open(path_p1)
        file2 = TFile.Open(path_p2)
        
        tree1 = file1.Get("tree")
        tree2 = file2.Get("tree")
        
        branchs1 = tree1.GetListOfBranches()
        for branch1 in branchs1:
        
            nameBranch = branch1.GetName()
            #nameBranch = "mc_nu_e"
            branch2 = tree2.GetBranch(nameBranch)
        
            tree1.Draw(nameBranch)
            tree2.Draw(nameBranch)
        
            hist1 = TH1F("hist1", nameBranch, 200, 0, 200)
            hist2 = TH1F("hist2", nameBranch, 200, 0, 200)
        
            tree1.Draw(nameBranch + ">>hist1")
            tree2.Draw(nameBranch + ">>hist2")
        
            k = hist1.KolmogorovTest(hist2, "UON")
            print(nameBranch + ": kolmogorov test = " + str(k))

            del hist1
            del hist2

        file1.Close()
        file2.Close()
    

    print("----- END -----")
