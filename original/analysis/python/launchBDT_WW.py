import os
import sys
import argparse
import math
import json

import joblib
import pandas as pd
import ROOT as r
import numpy as np

from scipy import optimize

from sklearn.experimental import enable_hist_gradient_boosting
from sklearn.ensemble import HistGradientBoostingClassifier


INPUT_NAMES = ["visible_e",   "nParticles", "eIsoLep", "higgs_e", "higgs_pt", "higgs_m", "higgs_cosTheta",
               "w1_m", "w1_pt", "w1_e", "w1_cosBetw", "w2_m", "w2_pt", "w2_e",
               "w2_cosBetw", "higgs_ww_cosBetw", "y_12", "y_23", "y_34", "y_45", "y_56",
               "y_67", "zz_z1_m", "zz_z2_m", "sl_w_m", "sl_rec_m", "oblateness", "sphericity",
               "cosThrust", "principleThrust", "majorThrust", "minorThrust"]

NNH_HOME = ""
dataPath = ""

CHANNELS = {
    '0': ["Signal", 1],
    '1': ["Other higgs->WW*", 401],
    '2': ["Other higgs", 417],
    '3': ["Other higgs->bb", 401],
    '4': ["Other higgs", 417],
    '5': ["2 fermions leptonic", 435],
    '6': ["2 fermions hadronic", 633],
    '7': ["4 fermions leptonic", 433],
    '8': ["4 fermions hadronic", 625],
    '9': ["4 fermions semileptonic", 591],
    '10': ["Background", 6]
}


def getTrainTree(bigFileName, friendFileName):

    bigFile = r.TFile.Open(bigFileName)
    tree = bigFile.Get("tree")

    friendFile = r.TFile.Open(friendFileName)
    friendTree = friendFile.Get("tree")

    tree.AddFriend(friendTree)

    df = r.RDataFrame(tree)

    df = df.Filter("isTrain && preSelected")

    # transform the ROOT RDataFrame into a format usable by scikit-learn
    npData = df.AsNumpy(columns=INPUT_NAMES+['weight', 'isSignal', 'channelType'])
    npData['isSignal'] = npData['isSignal'].astype('bool')
    data = pd.DataFrame(data=npData, copy=False, columns=INPUT_NAMES+['weight', 'isSignal', 'channelType'])

    # I had a problem with NaN values previously so I put them at 0 instead
    # no sure if the problem persists but I kept this anyway
    data.loc[data['minorThrust'] != data['minorThrust'], 'minorThrust'] = np.float32(0.0)

    # delete the intermediate format for memory usage
    del npData

    return data


def trainModel(trainData):

    print(trainData.info())

    bdt = HistGradientBoostingClassifier(max_iter=300, max_depth=None, max_leaf_nodes=70, learning_rate=0.2, n_iter_no_change=10, validation_fraction=0.3, verbose=2)
    bdt.fit(trainData[INPUT_NAMES], trainData["isSignal"], sample_weight=trainData['weight'])

    return bdt


def applyModel(bigFileName, friendFileName, model, scoresFileName):

    bigFile = r.TFile.Open(bigFileName)
    tree = bigFile.Get("tree")

    friendFile = r.TFile.Open(friendFileName)
    friendTree = friendFile.Get("tree")

    tree.AddFriend(friendTree)

    df = r.RDataFrame(tree)

    # read the DATA in batches because converting the whole ROOT TTree into a pandas dataframe at once takes too much memory
    BATCH_SIZE = 6_000_000
    nEvents = df.Count().GetValue()
    # nEvents = BATCH_SIZE

    print(f'{nEvents = }')
    BDT_scores = np.array([], dtype=np.float32)
    weights = np.array([], dtype=np.float32)
    isTrain = np.array([], dtype=np.bool)
    isSignal = np.array([], dtype=np.bool)
    isPreSelected = np.array([], dtype=np.bool)

    statsDict = {}

    batchBegin = 0

    # loop on batches
    while batchBegin < nEvents:

        batchEnd = min(nEvents, batchBegin + BATCH_SIZE)

        batchData = df.Range(batchBegin, batchEnd)

        npData = batchData.AsNumpy(columns=INPUT_NAMES + ['weight', 'isTrain', 'isSignal', 'channelType', 'preSelected'])
        npData['isSignal'] = npData['isSignal'].astype('bool')
        npData['isTrain'] = npData['isTrain'].astype('bool')
        npData['preSelected'] = npData['preSelected'].astype('bool')

        pdData = pd.DataFrame(data=npData, copy=False, columns=INPUT_NAMES + ['weight', 'isTrain', 'isSignal', 'channelType', 'preSelected'])
        pdData.loc[pdData['minorThrust'] != pdData['minorThrust'], 'minorThrust'] = np.float32(0.0)

        groupBy = pdData.loc[~pdData['isTrain']].groupby(['channelType', 'preSelected'])['weight']

        for name, group in groupBy:

            chan, preSelected = name
            chan = str(chan)

            if not chan in statsDict:
                statsDict[chan] = {'stat': 0, 'sum': 0.0, 'statPreSel': 0, 'sumPreSel': 0.0, 'statSel': 0, 'sumSel': 0.0}

            count = group.count()
            sum = group.sum()

            statsDict[chan]['stat'] += count
            statsDict[chan]['sum'] += sum

            if preSelected:
                statsDict[chan]['statPreSel'] += count
                statsDict[chan]['sumPreSel'] += sum

        # print(statsDict)

        # apply the BDT on all events (even training events and non preSelected events)
        # very lazy and inefficient
        y = np.array(model.decision_function(pdData[INPUT_NAMES]), dtype=np.float32)

        BDT_scores = np.append(BDT_scores, y)
        weights = np.append(weights, npData['weight'])
        isTrain = np.append(isTrain, npData['isTrain'])
        isSignal = np.append(isSignal, npData['isSignal'])
        isPreSelected = np.append(isPreSelected, npData['preSelected'])

        print(f'{batchEnd} events processed : {100.*batchEnd/nEvents:.2f} %')
        batchBegin = batchEnd

    b = {'BDTscore': BDT_scores, 'weight': weights, 'isSignal': isSignal, 'isTrain': isTrain, 'preSelected': isPreSelected}
    temp = pd.DataFrame(data=b, copy=False)

    # search for the BDT cut that maximizes significance

    def computeSignificance(cutValue, dataFrame):
        selected = (dataFrame['BDTscore'] > cutValue) & ~dataFrame['isTrain'] & dataFrame['preSelected']

        totalSelectedSum = dataFrame.loc[selected, 'weight'].sum()
        signalSelectedSum = dataFrame.loc[selected & dataFrame['isSignal'], 'weight'].sum()

        significance = 0
        if totalSelectedSum > 0:
            significance = signalSelectedSum/math.sqrt(totalSelectedSum)

        return -significance

    minScore = np.nanquantile(temp['BDTscore'], 0.05)
    maxScore = np.nanquantile(temp['BDTscore'], 0.999)

    res = optimize.minimize_scalar(computeSignificance, args=(temp,), bounds=(minScore, maxScore))

    print(f'minimization success: {res.success}')
    bestCut = res.x
    bestSignificance = -res.fun
    print(f'{bestCut=}')
    print(f'{bestSignificance=}')

    selected = np.array(BDT_scores > bestCut, dtype=np.int)
    toWrite = {'BDTscore': BDT_scores, 'sel': selected}

    rdf = r.RDF.MakeNumpyDataFrame(toWrite)
    rdf = rdf.Define("selected", "(bool)sel")

    # write the scores friend TTree
    rdf.Snapshot("tree", scoresFileName, r.std.vector('string')(["BDTscore", "selected"]))

    scoresFile = r.TFile.Open(scoresFileName)
    scoresTree = scoresFile.Get("tree")
    tree.AddFriend(scoresTree)

    df = r.RDataFrame(tree)
    df = df.Filter("!isTrain && preSelected && selected")

    BATCH_SIZE = 6_000_000
    nEvents = df.Count().GetValue()
    # nEvents = BATCH_SIZE

    batchBegin = 0
    while batchBegin < nEvents:

        batchEnd = min(nEvents, batchBegin + BATCH_SIZE)

        batchData = df.Range(batchBegin, batchEnd)

        npData = batchData.AsNumpy(columns=['weight', 'channelType'])
        pdData = pd.DataFrame(data=npData, copy=False, columns=['weight', 'channelType'])

        groupBy = pdData.groupby('channelType')['weight']

        for name, group in groupBy:
            count = group.count()
            sum = group.sum()

            chan = str(name)

            statsDict[chan]['statSel'] += count
            statsDict[chan]['sumSel'] += sum

        print(f'{batchEnd} events processed : {100.*batchEnd/nEvents:.2f} %')
        batchBegin = batchEnd

    print(f"{'channel':>25s} {'stat':>10s} {'statPreSel':>10s} {'statSel':>10s} {'sum':>15s} {'sumPreSel':>15s} {'sumSel':>15s} {'selection':>15s}%")
    for key in sorted([int(c) for c in statsDict.keys()]):
        key = str(key)

        channelName = CHANNELS[key][0]

        t = statsDict[key]
        t['name'] = channelName

        stat, statPreSel, statSel, sum, sumPreSel, sumSel = t['stat'], t['statPreSel'], t['statSel'], t['sum'], t['sumPreSel'], t['sumSel']
        print(f"{channelName:>25s} {stat:>10} {statPreSel:>10} {statSel:>10} {sum:>15.3f} {sumPreSel:>15.3f} {sumSel:>15.3f} {100.0*sumSel/sum:>15.3f}%")

        t['effPreSel'] = 1.0*sumPreSel/sum
        t['effSel'] = 1.0*sumSel/sum

    df.Snapshot("tree", f'{dataPath}/bestSelection_ww_e{ePol:+}_p{pPol:+}.root')

    class NpEncoder(json.JSONEncoder):
        def default(self, obj):
            if isinstance(obj, np.integer):
                return int(obj)
            if isinstance(obj, np.floating):
                return float(obj)
            if isinstance(obj, np.ndarray):
                return obj.tolist()
            return super(NpEncoder, self).default(obj)

    with open(f'{dataPath}/stats_ww_e{ePol:+}_p{pPol:+}.json', "w") as jsonFile:
        json.dump(statsDict, jsonFile, indent=2, cls=NpEncoder)


if __name__ == "__main__":

    if 'NNH_HOME' not in os.environ:
        print('ERROR : env variable NNH_HOME is not set')
        sys.exit(1)

    NNH_HOME = os.environ['NNH_HOME']

    parser = argparse.ArgumentParser()
    args = vars(parser.parse_args())

    ePol = -0.8
    pPol = 0.3

    dataPath = f'{NNH_HOME}/analysis/DATA'

    trainData = getTrainTree(f'{dataPath}/DATA.root', f'{dataPath}/split_ww_e{ePol:+}_p{pPol:+}.root')
    model = trainModel(trainData)

    joblib.dump(model, f"{dataPath}/model_ww_e{ePol:+}_p{pPol:+}.joblib")

    model = joblib.load(f"{dataPath}/model_ww_e{ePol:+}_p{pPol:+}.joblib")

    y = applyModel(f'{dataPath}/DATA.root', f'{dataPath}/split_ww_e{ePol:+}_p{pPol:+}.root', model, f'{dataPath}/scores_ww_e{ePol:+}_p{pPol:+}.root')
