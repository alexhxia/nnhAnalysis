/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "Channels.hh"

#include <cstdio>
#include <cstdlib>
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

// dirty global variables

// these following channels won't change depending if it's h->bb or h->WW study
const std::set<int> CHANNELS_FERMIONS_2_L = {
    500006, 500008
};

const std::set<int> CHANNELS_FERMIONS_2_H = {
    500010, 500012
};

const std::set<int> CHANNELS_FERMIONS_4_H = {
    500062, 500064, 500066, 500068, 500070, 500072
};

const std::set<int> CHANNELS_FERMIONS_4_SL = {
    500074, 500076, 500078, 500080, 500082, 500084, 500101, 500102,
    500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112
};

const std::set<int> CHANNELS_FERMIONS_4_L = {
    500086, 500088, 500090, 500092, 500094, 500096, 500098, 500100,
    500113, 500114, 500115, 500116, 500117, 500118, 500119, 500120,
    500122, 500124, 500125, 500126, 500127, 500128
};

// these two will change
std::set<int> CHANNELS_SIGNAL = {402007};

std::set<int> CHANNELS_OTHERHIGGS = {
    402001, 402002, 402003, 402004, 402005, 402006, 402008, 402009,
    402010, 402011, 402012, 402013, 402014, 402182, 402185
};

Channel getOtherChannel(int p) {
    
    auto isInSet = [](std::set<int> set, int element) -> bool { return set.find(element) != set.end(); };

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
    
    auto isInSet = [](std::set<int> set, int element) -> bool { 
        return set.find(element) != set.end(); 
    };

    if (isInSet(CHANNELS_SIGNAL, p)) {
        if (hd == 5) {
            return SIGNAL;
        }else {
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
    
    auto isInSet = [](std::set<int> set, int element) -> bool { 
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

void createFriendTree(
        std::string bigFileName, std::string friendFileName, 
        bool isBB, float trainProportion, float ePol, float pPol) {
        
    std::cout 
            << "Create Friend Tree with ePol = " << ePol << ", " 
            << "pPol = " << pPol << ", "
            << "trainProportion = " << trainProportion 
            << std::endl;

    // depending on bb or ww case, add the corresponding signal and background channelIDs
    if (isBB) {
        CHANNELS_SIGNAL.insert(402173);
        CHANNELS_OTHERHIGGS.insert(402176);
    } else {
        CHANNELS_SIGNAL.insert(402176);
        CHANNELS_OTHERHIGGS.insert(402173);
    }

    auto bigFile = TFile::Open(bigFileName.c_str(), "READ");
    auto bigTree = bigFile->Get<TTree>("tree");

    auto dataFrame = ROOT::RDataFrame(*bigTree);

    std::function<Channel(int, int, int)> getChannel;

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
    std::random_device                    rd;
    std::mt19937_64                       gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    auto lambda_isTrain = [&]() -> bool { 
        return dis(gen) < trainProportion; 
    };

    // Pre selection
    std::vector<std::string> cols;
    if (isBB) {
        cols = {"eIsoLep", "higgs_m", "isValid_bb"};
    } else {
        cols = {"eIsoLep", "higgs_m", "isValid_ww"};
    }

    auto preCut = [](
            const float& eIsoLep, 
            const float& higgs_m, 
            const bool& isValid) -> bool {
        if (!isValid) {
            return false;
        }
        if (eIsoLep > 0) {
            return false;
        }
        if (higgs_m < 70 || higgs_m > 220) {
            return false;
        }
        return true;
    };

    auto df = dataFrame
            .Define("isSignal", lambda_isSignal, {"processID", "mc_higgs_decay", "mc_higgs_subDecay"})
            .Define("channelType", lambda_getChannel, {"processID", "mc_higgs_decay", "mc_higgs_subDecay"})
            .Define("isTrain", lambda_isTrain)
            .Define("preSelected", preCut, cols);

    // Do a temporary save of the file to fix the RNG generation otherwise it will change on each operation on the
    // dataframe
    df.Snapshot("tempTree", "temp.root", {"isSignal", "channelType", "isTrain", "preSelected"});

    auto tempFile = TFile::Open("temp.root");
    auto tempTree = tempFile->Get<TTree>("tempTree");

    bigTree->AddFriend(tempTree, "temp");

    df = ROOT::RDataFrame(*bigTree);

    std::stringstream toto;
    toto << std::getenv("NNH_HOME") << "/analysis/channels.json";

    const std::string JSON_FILE = toto.str();

    std::ifstream ifs(JSON_FILE);
    auto          json = nlohmann::json::parse(ifs);

    const auto eR = 0.5 * (ePol + 1.);
    const auto eL = 0.5 * (1. - ePol);
    const auto pR = 0.5 * (pPol + 1.);
    const auto pL = 0.5 * (1. - pPol);

    const std::map<std::string, float> weightsPol = {
        {"eL.pR", eL * pR}, {"eR.pL", eR * pL}, 
        {"eL.pL", eL * pL}, {"eR.pR", eR * pR}
    };

    std::map<int, float> xSectMap = {};

    const auto allProcesses = {
        CHANNELS_SIGNAL,       CHANNELS_OTHERHIGGS,   CHANNELS_FERMIONS_2_L,
        CHANNELS_FERMIONS_2_H, CHANNELS_FERMIONS_4_H, CHANNELS_FERMIONS_4_SL,
        CHANNELS_FERMIONS_4_L
    };

    std::set<int> PROCESSES;

    for (const auto& set : allProcesses) {
        PROCESSES.insert(set.begin(), set.end());
    }

    // compute event weights
    for (auto processID : PROCESSES) {
        // exclude those two for now because they have overlap with exclusive bb and WW decays
        if (processID == 402007 || processID == 402008) {
            continue;
        }

        auto channelInfo = json[std::to_string(processID)];
        auto xSect = channelInfo["xsect"].get<float>();
        auto polarization = channelInfo["Polarization"].get<std::string>();

        xSectMap[processID] = xSect * weightsPol.at(polarization);
    }

    // handle overlap of higgs inclusive and exclusive bb and WW decays
    const std::tuple<int, int, int, std::string> polL = {402007, 402173, 402176, "eL.pR"};
    const std::tuple<int, int, int, std::string> polR = {402008, 402182, 402185, "eR.pL"};

    for (const auto& [processAll, processBB, processWW, polarization] : {polL, polR}) {
        const auto channelInfoBB = json[std::to_string(processBB)];
        const auto channelInfoWW = json[std::to_string(processWW)];
        const auto channelInfoAll = json[std::to_string(processAll)];

        const auto xSectBB = channelInfoBB["xsect"].get<float>();
        const auto xSectWW = channelInfoWW["xsect"].get<float>();
        const auto xSectIncl = channelInfoAll["xsect"].get<float>() - xSectBB - xSectWW;

        xSectMap[processAll] = xSectIncl * weightsPol.at(polarization);
    }

    std::cout << "Cross sections : " << std::endl;
    for (const auto& [processID, xSect] : xSectMap) {
        std::cout << "Process : " << processID << ", xSect = " << xSect << std::endl;
    }

    // Now we count the number of events for each process for the training and the testing sets

    // <<processID, is train or test>, number of events>
    std::map<std::pair<int, bool>, unsigned int> nEventsMap = {};

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
    std::map<std::pair<int, bool>, float> weightsMap = {};

    for (const auto& [pair, nEvents] : nEventsMap) {
        const auto processID = pair.first;
        weightsMap[pair] = xSectMap.at(processID) / nEvents;
    }

    // for (const auto& [pair, nEvents] : nEventsMap) {
    //     if (pair.second) {
    //         std::cout << "ProcessID : " << pair.first << ", nEventsTrain : " << nEvents << std::endl;
    //     } else {
    //         std::cout << "ProcessID : " << pair.first << ", nEventsTest : " << nEvents << std::endl;
    //     }
    // }

    // std::cout << "weightsTrainMap : " << std::endl;
    // for (const auto& [pair, weight] : weightsMap) {
    //     if (pair.second) {
    //         std::cout << "Process : " << pair.first << ", weight : " << weight << std::endl;
    //      }
    //  }

    // std::cout << "weightsTestMap : " << std::endl;
    // for (const auto& [pair, weight] : weightsMap) {
    //     if (!pair.second) {
    //         std::cout << "Process : " << pair.first << ", weight : " << weight << std::endl;
    //      }
    // }
    
    auto lambda_weights = [&](bool isTrain, int p, int hd) -> float {
        const auto realProcess = getRealProcess(p, hd);
        return weightsMap.at({realProcess, isTrain});
    };

    // write the weights for all the events (training and testing sets)
    auto df_firstPassForWeights = df.Define(
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

    const auto sumSignal = df_firstPassForWeights.Filter(
            lambda_isSignalAndTrain,
            {"isSignal", "isTrain"}
    ).Sum("weightFirstPass").GetValue();
    const auto sumBkg = df_firstPassForWeights.Filter(
            lambda_isNotSignalAndTrain, 
            {"isSignal", "isTrain"}
    ).Sum("weightFirstPass").GetValue();

    // The correction factor to apply to signal events to equilibrate with the background
    const auto corr = sumBkg / sumSignal;

    std::cout << "sumSignal = " << sumSignal << std::endl;
    std::cout << "sumBkg = " << sumBkg << std::endl;
    std::cout << "corr = " << corr << std::endl;

    auto lambda_weightCorr = [&](bool isSignal, bool isTrain, int p, int hd) -> float {
        const auto realProcess = getRealProcess(p, hd);

        auto weight = weightsMap.at({realProcess, isTrain});

        if (isSignal && isTrain) {
            weight *= corr;
        }

        return weight;
    };

    // write the weights a second time for all the events (with the corrected training weights for signal events)
    auto finalDF = df_firstPassForWeights.Define(
            "weight", 
            lambda_weightCorr,
            {"isSignal", "isTrain", "processID", "mc_higgs_decay"}
    );

    // write the output friend tree
    finalDF.Snapshot(
            "tree", 
            friendFileName, 
            {"isSignal", "channelType", "isTrain", "preSelected", "weight"}
    );

    auto finalTrain = finalDF.Filter([](bool b) { return b; }, {"isTrain"});

    auto finalTest = finalDF.Filter([](bool b) { return !b; }, {"isTrain"});

    std::cout << "Train : signal = "
              << finalTrain.Filter([](bool b) { return b; }, {"isSignal"}).Sum("weight").GetValue()
              << ", bkg = " << finalTrain.Filter([](bool b) { return !b; }, {"isSignal"}).Sum("weight").GetValue()
              << std::endl;

    std::cout << "Test : signal = " << finalTest.Filter([](bool b) { return b; }, {"isSignal"}).Sum("weight").GetValue()
              << ", bkg = " << finalTest.Filter([](bool b) { return !b; }, {"isSignal"}).Sum("weight").GetValue()
              << std::endl;

    tempFile->Close();

    // remove the now useless temp file
    std::remove("temp.root");

    bigFile->Close();
}

int main() {
    
    const auto trainProp = 0.2f;

    auto nnhHome = std::getenv("NNH_HOME");

    if (!nnhHome) {
        std::cerr << "ERROR : NNH_HOME env variable is not set" << std::endl;
        return 1;
    }

    const auto dataPATH = std::string(nnhHome) + "/analysis/DATA";
    const auto bigFileName = dataPATH + "/DATA.root";

    createFriendTree(bigFileName, dataPATH + "/split_bb_e-0.8_p+0.3.root", true, trainProp, -0.8, 0.3);
    createFriendTree(bigFileName, dataPATH + "/split_bb_e+0_p+0.root", true, trainProp, 0., 0.);

    createFriendTree(bigFileName, dataPATH + "/split_ww_e-0.8_p+0.3.root", false, trainProp, -0.8, 0.3);
    createFriendTree(bigFileName, dataPATH + "/split_ww_e+0_p+0.root", false, trainProp, 0., 0.);

    return 0;
}
