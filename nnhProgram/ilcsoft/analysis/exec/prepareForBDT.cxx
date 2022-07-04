/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "Channels.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "json.hpp"

#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

//#include <cassert>

using namespace std;

// dirty global variables

// these following channels won't change depending if it's h->bb or h->WW study
const set<int> CHANNELS_FERMIONS_2_L = {
    500006, 500008
};

const set<int> CHANNELS_FERMIONS_2_H = {
    500010, 500012
};

const set<int> CHANNELS_FERMIONS_4_H = {
    500062, 500064, 500066, 500068, 500070, 500072
};

const set<int> CHANNELS_FERMIONS_4_SL = {
    500074, 500076, 500078, 500080, 500082, 500084, 500101, 500102,
    500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112
};

const set<int> CHANNELS_FERMIONS_4_L = {
    500086, 500088, 500090, 500092, 500094, 500096, 500098, 500100,
    500113, 500114, 500115, 500116, 500117, 500118, 500119, 500120,
    500122, 500124, 500125, 500126, 500127, 500128
};

// these two will change
set<int> CHANNELS_SIGNAL = {
    402007
};

set<int> CHANNELS_OTHERHIGGS = {
    402001, 402002, 402003, 402004, 402005, 402006, 402008, 402009,
    402010, 402011, 402012, 402013, 402014, 402182, 402185
};

Channel getOtherChannel(int p) {
    
    auto isInSet = [](set<int> set, int element) -> bool { 
        return set.find(element) != set.end(); 
    };

    if (isInSet(CHANNELS_FERMIONS_2_L, p)) {
        return F2_L;
    } else if (isInSet(CHANNELS_FERMIONS_2_H, p)) {
        return F2_H;
    } else if (isInSet(CHANNELS_FERMIONS_4_L, p)) {
        return F4_L;
    } else if (isInSet(CHANNELS_FERMIONS_4_H, p)) {
        return F4_H;
    } else if (isInSet(CHANNELS_FERMIONS_4_SL, p)) {
        return F4_SL;
    } else {
        throw("Channel unknown");
    }
}

Channel getChannelBB(int p, int hd, int /*hsd*/) {
    
    auto isInSet = [](set<int> set, int element) -> bool { 
        return set.find(element) != set.end(); 
    };

    if (isInSet(CHANNELS_SIGNAL, p)) {
        if (hd == 5) {
            return SIGNAL;
        } else {
            return OTHER_H_NOT_BB;
        }
    } else if (isInSet(CHANNELS_OTHERHIGGS, p)) {
        if (hd == 5) {
            return OTHER_H_BB;
        } else {
            return OTHER_H_NOT_BB;
        }
    } else {
        return getOtherChannel(p);
    }
}

Channel getChannelWW(int p, int hd, int hsd) {
    
    auto isInSet = [](set<int> set, int element) -> bool { 
        return set.find(element) != set.end(); 
    };

    if (isInSet(CHANNELS_SIGNAL, p)) {
        if (hd == 24 && hsd == 1) {
            return SIGNAL;
        } else if (hd == 24) {
            return OTHER_H_WW;
        } else {
            return OTHER_H_NOT_WW;
        }
    } else if (isInSet(CHANNELS_OTHERHIGGS, p)) {
        if (hd == 24) {
            return OTHER_H_WW;
        } else {
            return OTHER_H_NOT_WW;
        }
    } else {
        return getOtherChannel(p);
    }
}

/**
 * Create a Friend Tree friendFileName at bigFileName.
 *  - bool isBB, 
 *  - float trainProportion, 
 *  - float ePol --> polarisation, 
 *  - float pPol --> polarisation
 */
void createFriendTree(
        const string dataPATH, string bigFileName, string friendFileName, 
        bool isBB, float trainProportion, float ePol, float pPol) {
            
         
    cout    << "Working directory: " << friendFileName << endl << endl;
    
    cout    << "Create Friend Tree with "
            << "ePol = " << ePol << ", "
            << "pPol = " << pPol << ", "
            << "trainProportion = " << trainProportion 
            << endl;

    // depending on bb or ww case, add the corresponding signal and background 
    // channelIDs
    if (isBB) { 
        CHANNELS_SIGNAL.insert(402173);
        CHANNELS_OTHERHIGGS.insert(402176);
    } else {
        CHANNELS_SIGNAL.insert(402176);
        CHANNELS_OTHERHIGGS.insert(402173);
    }

    TFile *bigFile = TFile::Open(bigFileName.c_str(), "READ");
    TTree* bigTree = bigFile->Get<TTree>("tree");

    ROOT::RDataFrame dataFrame = ROOT::RDataFrame(*bigTree);

    function<Channel(int, int, int)> getChannel;

    if (isBB) {
        getChannel = getChannelBB;
    } else {
        getChannel = getChannelWW;
    }

    auto lambda_getChannel = [&](int pID, int hd, int hsd) -> int { 
        return static_cast<int>(getChannel(pID, hd, hsd)); 
    };

    auto lambda_isSignal = [&](int pID, int hd, int hsd) -> bool { 
        return getChannel(pID, hd, hsd) == static_cast<int>(SIGNAL); 
    };

    // RNG for train and test split
    random_device rd;
    mt19937_64 gen(rd());
    uniform_real_distribution<float> dis(0.0f, 1.0f);

    auto lambda_isTrain = [&]() -> bool { 
        return dis(gen) < trainProportion; 
    };

    // Pre selection
    vector<string> cols;
    if (isBB) {
        cols = {"eIsoLep", "higgs_m", "isValid_bb"};
    } else {
        cols = {"eIsoLep", "higgs_m", "isValid_ww"};
    }

    auto preCut = [](
            const float& eIsoLep, 
            const float& higgs_m, 
            const bool& isValid) -> bool {
        
        return isValid && eIsoLep <= 0 && higgs_m >= 70 && higgs_m <= 220;
    };

    ROOT::RDF::RInterface df = dataFrame            
            .Define("isSignal", lambda_isSignal, 
                    {"processID", "mc_higgs_decay", "mc_higgs_subDecay"})
            .Define("channelType", lambda_getChannel, 
                    {"processID", "mc_higgs_decay", "mc_higgs_subDecay"})
            .Define("isTrain", lambda_isTrain)
            .Define("preSelected", preCut, cols);

    // Do a temporary save of the file to fix the RNG generation otherwise
    // it will change on each operation on the dataframe
    string tempNameFile = (dataPATH + "/temp.root");
    //cout << "1 : create " << tempNameFile << endl;
    //return;
    df.Snapshot("tempTree", tempNameFile.c_str(), 
            {"isSignal", "channelType", "isTrain", "preSelected"});
    //cout << "2 : Snapshot" << endl;
    
    TFile* tempFile = TFile::Open(tempNameFile.c_str());
    //cout << "3 : Open" << endl;
    if (!tempFile) {
       cerr << "Error opening file" << endl;
       exit(-1);
    }
    //cout << "4 : Open success" << endl;

    TTree* tempTree = tempFile->Get<TTree>("tempTree");
    //cout << "5 : getTree" << endl;

    bigTree->AddFriend(tempTree, "temp");
    //cout << "6 : AddFriend" << endl;

    df = ROOT::RDataFrame(*bigTree);
    //cout << "7 : RDataFrame" << endl;

    stringstream toto;
    toto << getenv("NNH_HOME") << "/analysis/channels.json";
    //cout << "8 : toto" << endl;
    
    const string JSON_FILE = toto.str();
    //cout << "9 : toto str" << endl;
    
    ifstream ifs(JSON_FILE);
    nlohmann::json json = nlohmann::json::parse(ifs);
    //cout << "10 : nlohmann" << endl;

    const float eR = 0.5 * (ePol + 1.);
    const float eL = 0.5 * (1. - ePol);
    const float pR = 0.5 * (pPol + 1.);
    const float pL = 0.5 * (1. - pPol);

    const map<string, float> weightsPol = {
        {"eL.pR", eL * pR}, {"eR.pL", eR * pL}, 
        {"eL.pL", eL * pL}, {"eR.pR", eR * pR}
    };

    map<int, float> xSectMap = {};

    const set<set<int>> allProcesses = {
        CHANNELS_SIGNAL,       CHANNELS_OTHERHIGGS,   CHANNELS_FERMIONS_2_L,
        CHANNELS_FERMIONS_2_H, CHANNELS_FERMIONS_4_H, CHANNELS_FERMIONS_4_SL,
        CHANNELS_FERMIONS_4_L
    };

    set<int> PROCESSES;

    for (set<int> set : allProcesses) {
        PROCESSES.insert(set.begin(), set.end());
    }

    // compute event weights
    for (int processID : PROCESSES) {
        // exclude those two for now because they have overlap with exclusive bb and WW decays
        if (processID == 402007 || processID == 402008) {
            continue;
        }
        auto channelInfo = json[to_string(processID)];
        float xSect = channelInfo["xsect"].get<float>();
        string polarization = channelInfo["Polarization"].get<string>();

        xSectMap[processID] = xSect * weightsPol.at(polarization);
    }

    // handle overlap of higgs inclusive and exclusive bb and WW decays
    const tuple<int, int, int, string> polL = {402007, 402173, 402176, "eL.pR"};
    const tuple<int, int, int, string> polR = {402008, 402182, 402185, "eR.pL"};

    for (const auto& [processAll, processBB, processWW, polarization] : {polL, polR}) {
        const auto channelInfoBB = json[to_string(processBB)];
        const auto channelInfoWW = json[to_string(processWW)];
        const auto channelInfoAll = json[to_string(processAll)];

        const float xSectBB = channelInfoBB["xsect"].get<float>();
        const float xSectWW = channelInfoWW["xsect"].get<float>();
        const float xSectIncl = channelInfoAll["xsect"].get<float>() - xSectBB - xSectWW;

        xSectMap[processAll] = xSectIncl * weightsPol.at(polarization);
    }

    cout << "Cross sections : " << endl;
    for (const auto& [processID, xSect] : xSectMap) {
        cout    << "Process : " << processID << ", "
                << "xSect = " << xSect << endl;
    }
    
    // Now we count the number of events for each process for the training and the testing sets

    // <<processID, is train or test>, number of events>
    map<pair<int, bool>, unsigned int> nEventsMap = {};

    const auto getRealProcess = [](int p, int hd) -> int {
        if (p != 402007 && p != 402008) {
            return p;
        }
        if (p == 402007) {
            if (hd == 5) {
                return 402173;
            } else if (hd == 24) {
                return 402176;
            } else {
                return 402007;
            }
        }

        if (p == 402008) {
            if (hd == 5) {
                return 402182;
            } else if (hd == 24) {
                return 402185;
            } else {
                return 402008;
            }
        }

        throw("unknown process ID");
    };

    auto lambda_countEvents = [&](bool isTrain, int p, int hd) -> void {
        const auto realProcess = getRealProcess(p, hd);
        nEventsMap[{realProcess, isTrain}]++;
    };

    df.Foreach(lambda_countEvents, {"isTrain", "processID", "mc_higgs_decay"});

    // Then we compute the event weights for each process for the training and the testing sets

    // <<processID, is train or test>, weight>
    map<pair<int, bool>, float> weightsMap = {};

    for (const auto& [pair, nEvents] : nEventsMap) {
        const int processID = pair.first;
        weightsMap[pair] = xSectMap.at(processID) / nEvents;
    }

    // for (const auto& [pair, nEvents] : nEventsMap)
    // {
    //     if (pair.second)
    //         std::cout << "ProcessID : " << pair.first << ", nEventsTrain : " << nEvents << std::endl;
    //     else
    //         std::cout << "ProcessID : " << pair.first << ", nEventsTest : " << nEvents << std::endl;
    // }

    // std::cout << "weightsTrainMap : " << std::endl;
    // for (const auto& [pair, weight] : weightsMap)
    //     if (pair.second)
    //         std::cout << "Process : " << pair.first << ", weight : " << weight << std::endl;

    // std::cout << "weightsTestMap : " << std::endl;
    // for (const auto& [pair, weight] : weightsMap)
    //     if (!pair.second)
    //         std::cout << "Process : " << pair.first << ", weight : " << weight << std::endl;

    auto lambda_weights = [&](bool isTrain, int p, int hd) -> float {
        const int realProcess = getRealProcess(p, hd);
        return weightsMap.at({realProcess, isTrain});
    };

    // write the weights for all the events (training and testing sets)
    ROOT::RDF::RInterface df_firstPassForWeights = df.Define(
            "weightFirstPass", 
            lambda_weights, 
            {"isTrain", "processID", "mc_higgs_decay"}
    );

    // Then we apply a correction for the training set to equilibrate the weights between background and signal,
    // otherwise the training set will be overdominated by the background events and this will impact training

    auto lambda_isSignalAndTrain = [](bool isSignal, bool isTrain) { 
        return isSignal && isTrain; 
    };
    
    auto lambda_isNotSignalAndTrain = [](bool isSignal, bool isTrain) { 
        return !isSignal && isTrain; 
    };

    const int sumSignal = df_firstPassForWeights
            .Filter(lambda_isSignalAndTrain, {"isSignal", "isTrain"})
            .Sum("weightFirstPass")
            .GetValue();
    const int sumBkg = df_firstPassForWeights
            .Filter(lambda_isNotSignalAndTrain, {"isSignal", "isTrain"})
            .Sum("weightFirstPass")
            .GetValue();

    // The correction factor to apply to signal events to equilibrate with the background
    const float corr = sumBkg / sumSignal;

    cout << "sumSignal = " << sumSignal << endl;
    cout << "sumBkg = " << sumBkg << endl;
    cout << "corr = " << corr << endl;

    auto lambda_weightCorr = [&](bool isSignal, bool isTrain, int p, int hd) -> float {
        const int realProcess = getRealProcess(p, hd);

        float weight = weightsMap.at({realProcess, isTrain});

        if (isSignal && isTrain) {
            weight *= corr;
        }

        return weight;
    };

    // write the weights a second time for all the events (with the corrected training weights for signal events)
    ROOT::RDF::RInterface finalDF = df_firstPassForWeights.Define(
            "weight", lambda_weightCorr,
            {"isSignal", "isTrain", "processID", "mc_higgs_decay"});

    // write the output friend tree
    finalDF.Snapshot("tree", friendFileName, 
            {"isSignal", "channelType", "isTrain", "preSelected", "weight"});

    ROOT::RDF::RInterface finalTrain = finalDF.Filter([](bool b) { return b; }, {"isTrain"});

    ROOT::RDF::RInterface finalTest = finalDF.Filter([](bool b) { return !b; }, {"isTrain"});

    cout    << "Train : signal = "
            << finalTrain
                    .Filter([](bool b) { return b; }, {"isSignal"})
                    .Sum("weight").GetValue()
            << ", bkg = " 
            << finalTrain
                    .Filter([](bool b) { return !b; }, {"isSignal"})
                    .Sum("weight").GetValue()
            << endl;

    cout    << "Test : signal = " 
            << finalTest
                    .Filter([](bool b) { return b; }, {"isSignal"})
                    .Sum("weight").GetValue()
            << ", bkg = " 
            << finalTest
                    .Filter([](bool b) { return !b; }, {"isSignal"})
                    .Sum("weight").GetValue()
            << endl;

    tempFile->Close();

    // remove the now useless temp file
    remove("temp.root"); //tempNameFile);

    bigFile->Close();
}

int main(int argc, char **argv) {

    // Initial condition: we must have 1 parameter, path to data work directory
    if (argc == 1) {
        cerr    << "ERROR: Path to directory missing.\n"
                << "SYNTAX: prepareForBDT path/to/DATA_directory"
                << endl;
        return 1;
    } else if (argc > 2) {
        cerr    << "ERROR: Just one argument.\n"
                << "SYNTAX: prepareForBDT path/to/DATA_directory"
                << endl;
        return 1;
    }
    
    // Path to working directory
    const string dataPATH = string(argv[1]);
    
    // ROOT files witch contains all data
    const string bigFileName = dataPATH + "/DATA.root";
    
    const float trainProp = 0.2f;
    createFriendTree(dataPATH, bigFileName, dataPATH + "/split_bb_e-0.8_p+0.3.root", true, trainProp, -0.8, 0.3);
    createFriendTree(dataPATH, bigFileName, dataPATH + "/split_bb_e+0_p+0.root", true, trainProp, 0, 0);

    createFriendTree(dataPATH, bigFileName, dataPATH + "/split_ww_e-0.8_p+0.3.root", false, trainProp, -0.8, 0.3);
    createFriendTree(dataPATH, bigFileName, dataPATH + "/split_ww_e+0_p+0.root", false, trainProp, 0, 0);

    return 0;
}
