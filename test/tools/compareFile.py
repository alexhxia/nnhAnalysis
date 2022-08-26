#!/usr/bin/env python

class codeFile(enum.Enum):
    different = -1
    identical = 0
    similar = 1

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
            })

        del hist1
        del hist2
            
    return nameBranchDistinct


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
    
    
def getAnalysisNameFiles():
    """
    Get name files created by analysis program.
    
    Return:
    ------------------
    nameFileList : set
    """
    
    nameFileList = [{
        "file": "DATA.root",
        "compare": codeFile.identical
    }]
    
    particleTypeList = ["ww", "bb"]
    
    for particleType in particleTypeList:
        nameFileList_p = [
            {
            "file": "split_" + particleType + "_e+0_p+0.root",
            "compare": codeFile.identical
            }, 
            {
            "file": "split_" + particleType + "_e-0.8_p+0.3.root",
            "compare": codeFile.identical
            },
            {
            "file": "model_" + particleType + "_e-0.8_p+0.3.joblib",
            "compare": codeFile.different
            },
            {
            "file": "bestSelection_" + particleType + "_e-0.8_p+0.3.root",
            "compare": codeFile.different
            }, 
            {
            "file": "scores_" + particleType + "_e-0.8_p+0.3.root",
            "compare": codeFile.similar
            }, 
            {
            "file": "stats_" + particleType + "_e-0.8_p+0.3.json",
            "compare": codeFile.similar
            }
        ]
        nameFileList = nameFileList + nameFileList_p
    
    return nameFileList
