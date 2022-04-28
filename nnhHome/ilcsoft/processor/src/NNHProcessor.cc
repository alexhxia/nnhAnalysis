/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "NNHProcessor.hh"

#include "EventShape.hh"
#include "ParticleInfo.hh"
#include "PDGInfo.hh"

#include <LCIOSTLTypes.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

#include <marlin/VerbosityLevels.h>

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>

#include <TFile.h>
#include <TTree.h>

#include <fastjet/ClusterSequence.hh>

#include <CLHEP/Vector/LorentzVector.h>
#include <CLHEP/Vector/ThreeVector.h>

#include <streamlog/loglevels.h>
#include <streamlog/streamlog.h>
#include <string>
#include <vector>

#include <cassert>

using namespace std;

NNHProcessor aNNHProcessor;

/* TOOLS */

/**
 * Built 'PseudoJet' with 'ReconstructedParticle'
 */
fastjet::PseudoJet recoParticleToPseudoJet(
        EVENT::ReconstructedParticle* reconstructedParticle) {
    
    const double* momentum = reconstructedParticle->getMomentum();
    double energy = reconstructedParticle->getEnergy();

    fastjet::PseudoJet particle(momentum[0], momentum[1], momentum[2], energy);
    
    ParticleInfo* particleInfo = new ParticleInfo;
    particleInfo->setRecoParticle(reconstructedParticle);
    
    particle.set_user_info(particleInfo);
    
    return particle;
}

/**
 * Compute a recoil mass with a 4-vector and a energy
 */
double computeRecoilMass(const CLHEP::HepLorentzVector z4Vector, float energy) {
    
    CLHEP::Hep3Vector pTot = CLHEP::Hep3Vector(energy * sin(7e-3), 0, 0);
    pTot = pTot - CLHEP::Hep3Vector(z4Vector.px(), z4Vector.py(), z4Vector.pz());
    
    double rm = (energy - z4Vector.e()) * (energy - z4Vector.e()) - pTot.mag2();

    if (rm < 0.) {
        return 0.;
    } else {
        rm = sqrt(rm);
        return rm;
    }
}

/**
 * Compute a recoil mass with a PseudoJet and a energy
 */
double computeRecoilMass(const fastjet::PseudoJet& particle, float energy) {
    
    const CLHEP::HepLorentzVector vec = CLHEP::HepLorentzVector(
            particle.px(), particle.py(), particle.pz(), particle.e()); 
            
    return computeRecoilMass(vec, energy);
}

/**
 * Search a couple particle in PseudoJet vector's
 * which minimized (invariant mass - targetMass)
 */
array<fastjet::PseudoJet, 2> findParticleByMass(
            const vector<fastjet::PseudoJet> jets,
            const double                     targetMass,
            vector<fastjet::PseudoJet>&      remainingJets) {


    // Search a couple particle which minimized (invariant mass - targetMass)
    array<unsigned int, 2> goodPair = array<unsigned int, 2>{};
    double chi2 = numeric_limits<double>::max();

    for (unsigned int i = 0U; i < jets.size(); ++i) {
        for (unsigned int j = i + 1; j < jets.size(); ++j) {
            
            double m = (jets[i] + jets[j]).m(); // invariant mass
            double val = abs(m - targetMass);

            if (val <= chi2) {
                chi2 = val;
                goodPair = {i, j};
            }
        }
    }

    array<fastjet::PseudoJet, 2> toReturn = {
            jets[goodPair[0]], 
            jets[goodPair[1]]
    };

    // Save a not "good pair" jets
    for (unsigned int i = 0U; i < jets.size(); ++i) {
        if (i != goodPair[0] && i != goodPair[1]) {
            remainingJets.push_back(jets[i]);
        }
    }

    return toReturn;
}

/* public */

// MARKER

NNHProcessor::NNHProcessor() : Processor("NNHProcessor") {
    
    // Test
    registerProcessorParameter(
            "RootFileName", 
            "File name for the root output", 
            rootFileName, 
            string("test.root"));

    // MC particles
    registerProcessorParameter(
            "MCParticlesCollectionName", 
            "Name of the MC particles collection",
            mcParticleCollectionName, 
            string("MCParticlesSkimmed"));

    // Reconstructed Particles 
    registerProcessorParameter(
            "ReconstructedParticlesCollectionName", 
            "Name of the reconstructed particles collection",
            reconstructedParticleCollectionName, 
            string("PandoraPFOs"));

    // Isolated Photons 
    registerProcessorParameter(
            "IsolatedPhotonsCollectionName", 
            "Name of the reconstructed isolated photon collection",
            isolatedPhotonsCollectionName, 
            string("IsolatedPhotons"));

    // Isolated Leptons
    registerProcessorParameter(
            "IsolatedLeptonsCollectionNames",
            "Name of the reconstructed isolated leptons collections", 
            isolatedLeptonsCollectionNames,
            {"IsolatedElectrons", "IsolatedMuons", "IsolatedTaus"});

    // Jets
    registerProcessorParameter(
            "2JetsCollectionName", 
            "2 Jets Collection Name", 
            _2JetsCollectionName, 
            string("Refined2Jets"));
            
    registerProcessorParameter(
            "3JetsCollectionName", 
            "3 Jets Collection Name", 
            _3JetsCollectionName, 
            string("Refined3Jets"));
            
    registerProcessorParameter(
            "4JetsCollectionName", 
            "4 Jets Collection Name", 
            _4JetsCollectionName, 
            string("Refined4Jets"));
}

// COMMANDS

/**
 * Create output file and root tree.
 * Init branch root tree.
 */
void NNHProcessor::init() {
    
    streamlog_out(MESSAGE) << "NNHProcessor::init()" << std::endl;
    outputFile = new TFile(rootFileName.c_str(), "RECREATE");
    outputTree = new TTree("tree", "tree");

    // Collision Energy
    outputTree->Branch("processID", &processID);
    outputTree->Branch("event", &event);
    outputTree->Branch("sqrtS", &sqrtS);

    outputTree->Branch("isValid_bb", &isValid_bb);
    outputTree->Branch("isValid_ww", &isValid_ww);

    outputTree->Branch("visible_e", &visible_e);
    outputTree->Branch("nParticles", &nParticles);
    outputTree->Branch("nIsoLep", &nIsoLep);
    outputTree->Branch("eIsoLep", &eIsoLep);

    // Reco variables
    outputTree->Branch("higgs_e", &higgs_e);
    outputTree->Branch("higgs_pt", &higgs_pt);
    outputTree->Branch("higgs_m", &higgs_m);
    outputTree->Branch("higgs_cosTheta", &higgs_cosTheta);
    outputTree->Branch("higgs_recMass", &higgs_recMass);

    outputTree->Branch("higgs_bTag1", &higgs_bTag1);
    outputTree->Branch("higgs_bTag2", &higgs_bTag2);

    outputTree->Branch("b1_m", &b1_m);
    outputTree->Branch("b1_pt", &b1_pt);
    outputTree->Branch("b1_e", &b1_e);
    outputTree->Branch("b2_m", &b2_m);
    outputTree->Branch("b2_pt", &b2_pt);
    outputTree->Branch("b2_e", &b2_e);

    outputTree->Branch("w1_m", &w1_m);
    outputTree->Branch("w1_pt", &w1_pt);
    outputTree->Branch("w1_e", &w1_e);
    outputTree->Branch("w1_cosBetw", &w1_cosBetw);
    outputTree->Branch("w2_m", &w2_m);
    outputTree->Branch("w2_pt", &w2_pt);
    outputTree->Branch("w2_e", &w2_e);
    outputTree->Branch("w2_cosBetw", &w2_cosBetw);

    outputTree->Branch("higgs_bb_cosBetw", &higgs_bb_cosBetw);
    outputTree->Branch("higgs_ww_cosBetw", &higgs_ww_cosBetw);

    outputTree->Branch("y_12", &y_12);
    outputTree->Branch("y_23", &y_23);
    outputTree->Branch("y_34", &y_34);
    outputTree->Branch("y_45", &y_45);
    outputTree->Branch("y_56", &y_56);
    outputTree->Branch("y_67", &y_67);

    outputTree->Branch("zz_z1_m", &zz_z1_m);
    outputTree->Branch("zz_z2_m", &zz_z2_m);
    outputTree->Branch("sl_w_m", &sl_w_m);
    outputTree->Branch("sl_rec_m", &sl_rec_m);

    outputTree->Branch("oblateness", &oblateness);
    outputTree->Branch("sphericity", &sphericity);
    outputTree->Branch("cosThrust", &cosThrust);
    outputTree->Branch("principleThrust", &principleThrust);
    outputTree->Branch("majorThrust", &majorThrust);
    outputTree->Branch("minorThrust", &minorThrust);

    // ISR
    outputTree->Branch("mc_ISR_e", &mc_ISR_e);
    outputTree->Branch("mc_ISR_pt", &mc_ISR_pt);

    // Neutrinos
    outputTree->Branch("mc_nu_flavor", &mc_nu_flavor);
    outputTree->Branch("mc_nu_e", &mc_nu_e);
    outputTree->Branch("mc_nu_pt", &mc_nu_pt);
    outputTree->Branch("mc_nu_m", &mc_nu_m);
    outputTree->Branch("mc_nu_cosBetw", &mc_nu_cosBetw);

    // Higgs
    outputTree->Branch("mc_higgs_e", &mc_higgs_e);
    outputTree->Branch("mc_higgs_pt", &mc_higgs_pt);
    outputTree->Branch("mc_higgs_m", &mc_higgs_m);
    outputTree->Branch("mc_higgs_recMass", &mc_higgs_recMass);
    outputTree->Branch("mc_higgs_decay", &mc_higgs_decay);
    outputTree->Branch("mc_higgs_subDecay", &mc_higgs_subDecay);

    outputTree->Branch("mc_higgs_decay1_e", &mc_higgs_decay1_e);
    outputTree->Branch("mc_higgs_decay1_pt", &mc_higgs_decay1_pt);
    outputTree->Branch("mc_higgs_decay1_m", &mc_higgs_decay1_m);
    outputTree->Branch("mc_higgs_decay2_e", &mc_higgs_decay2_e);
    outputTree->Branch("mc_higgs_decay2_pt", &mc_higgs_decay2_pt);
    outputTree->Branch("mc_higgs_decay2_m", &mc_higgs_decay2_m);
    outputTree->Branch("mc_higgs_decay_cosBetw", &mc_higgs_decay_cosBetw);
}

/**
 * For each event :
 *      - init event variables
 */
void NNHProcessor::processEvent(LCEvent* evt) {
    
    clear();

    cout << "Event : " << evt->getEventNumber() << endl;

    processID = evt->getParameters().getIntVal(string("ProcessID"));
    event = evt->getParameters().getIntVal(string("Event Number"));
    sqrtS = evt->getParameters().getFloatVal(string("Energy"));

    mcCol = evt->getCollection(mcParticleCollectionName);
    recoCol = evt->getCollection(reconstructedParticleCollectionName);

    // MC stuff
    const EVENT::MCParticle* mc_gamma0 
            = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(6));
            
    const EVENT::MCParticle* mc_gamma1 
            = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(7));
            
    const EVENT::MCParticle* mc_nu0 
            = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(8));
            
    const EVENT::MCParticle* mc_nu1 
            = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(9));
            
    const EVENT::MCParticle* mc_higgs 
            = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(10));

    try {
        processISR(mc_gamma0, mc_gamma1);
    } catch (logic_error& e) {
        streamlog_out(DEBUG) 
                << "Run : " << evt->getRunNumber() << ", "
                << "Event : " << evt->getEventNumber() << " : "
                << e.what() << endl;
    }
    
    try {
        processNeutrinos(mc_nu0, mc_nu1);
    } catch (logic_error& e) {
        streamlog_out(DEBUG) 
                << "Run : " << evt->getRunNumber() << ", "
                << "Event : " << evt->getEventNumber() << " : "
                << e.what() << std::endl;
    }
    
    try {
        processHiggs(mc_higgs);
    } catch (logic_error& e) {
        streamlog_out(DEBUG) 
                << "Run : " << evt->getRunNumber() << ", "
                << "Event : " << evt->getEventNumber() << " : "
                << e.what() << std::endl;
    }
    // end of MC stuff

    principleThrust = recoCol->getParameters().getFloatVal("principleThrustValue");
    majorThrust = recoCol->getParameters().getFloatVal("majorThrustValue");
    minorThrust = recoCol->getParameters().getFloatVal("minorThrustValue");

    FloatVec ta = FloatVec{};
    recoCol->getParameters().getFloatVals("principleThrustAxis", ta);
    const CLHEP::Hep3Vector principleThrustAxis 
            = CLHEP::Hep3Vector(ta[0], ta[1], ta[2]);

    cosThrust = abs(principleThrustAxis.cosTheta());
    oblateness = recoCol->getParameters().getFloatVal("Oblateness");

    if (minorThrust != minorThrust) { // handle NaN case
        minorThrust = 0;
    }

    sphericity = recoCol->getParameters().getFloatVal("sphericity");

    // treat isolated leptons
    isolatedLeptons.clear();
    eIsoLep = 0;
    for (const string colName : isolatedLeptonsCollectionNames) {
        LCCollection* col = evt->getCollection(colName);
        unsigned int n = col->getNumberOfElements();

        for (unsigned int i = 0; i < n; ++i) {
            EVENT::ReconstructedParticle* particle = dynamic_cast
                    <EVENT::ReconstructedParticle*>(col->getElementAt(i));
            isolatedLeptons.insert(particle);
            eIsoLep += particle->getEnergy();
        }
    }
    nIsoLep = isolatedLeptons.size();

    // treat isolated photons
    isolatedPhotons.clear();
    {
        LCCollection* col = evt->getCollection(isolatedPhotonsCollectionName);
        unsigned int n = col->getNumberOfElements();

        for (unsigned int i = 0; i < n; ++i) {
            EVENT::ReconstructedParticle* particle = dynamic_cast
                    <EVENT::ReconstructedParticle*>(col->getElementAt(i));
            isolatedPhotons.insert(particle);
        }
    }

    nParticles = recoCol->getNumberOfElements();
    visible_e = 0;

    particles.reserve(nParticles);

    for (int index = 0; index < nParticles; ++index) {
        EVENT::ReconstructedParticle* recoPart = dynamic_cast
                <EVENT::ReconstructedParticle*>(recoCol->getElementAt(index));

        visible_e += recoPart->getEnergy();

        fastjet::PseudoJet particle = recoParticleToPseudoJet(recoPart);
        particles.push_back(particle);
    }

    // Jets study

    const auto sortJetsByEnergy = [](
            const EVENT::ReconstructedParticle* a,
            const EVENT::ReconstructedParticle* b) -> bool { 
        return a->getEnergy() > b->getEnergy(); 
    };

    const LCCollection* _2JetsCol = evt->getCollection(_2JetsCollectionName);
    const LCCollection* _3JetsCol = evt->getCollection(_3JetsCollectionName);
    const LCCollection* _4JetsCol = evt->getCollection(_4JetsCollectionName);

    vector<EVENT::ReconstructedParticle*> _2Jets 
            = vector<EVENT::ReconstructedParticle*>{};
            
    vector<EVENT::ReconstructedParticle*> _3Jets 
            = vector<EVENT::ReconstructedParticle*>{};
            
    vector<EVENT::ReconstructedParticle*> _4Jets 
            = vector<EVENT::ReconstructedParticle*>{};

    for (int index = 0; index < _2JetsCol->getNumberOfElements(); ++index) {
        _2Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(
                _2JetsCol->getElementAt(index)));
    }
    for (int index = 0; index < _3JetsCol->getNumberOfElements(); ++index) {
        _3Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(
                _3JetsCol->getElementAt(index)));
    }
    for (int index = 0; index < _4JetsCol->getNumberOfElements(); ++index) {
        _4Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(
                _4JetsCol->getElementAt(index)));
    }

    sort(_2Jets.begin(), _2Jets.end(), sortJetsByEnergy);
    sort(_3Jets.begin(), _3Jets.end(), sortJetsByEnergy);
    sort(_4Jets.begin(), _4Jets.end(), sortJetsByEnergy);

    // 2 jets study
    isValid_bb = (_2Jets.size() == 2);
    if (isValid_bb) {
        vector<fastjet::PseudoJet> jets = vector<fastjet::PseudoJet>{};
        for (EVENT::ReconstructedParticle* lcioJet : _2Jets) {
            jets.push_back(recoParticleToPseudoJet(lcioJet));
        }

        const fastjet::PseudoJet higgs = join(jets[0], jets[1]);
        const CLHEP::Hep3Vector higgs_mom = CLHEP::Hep3Vector(
                jets[0].px(), jets[0].py(), jets[0].pz());

        higgs_e = higgs.e();
        higgs_pt = higgs.pt();
        higgs_m = higgs.m();
        higgs_cosTheta = higgs_mom.cosTheta();

        higgs_recMass = computeRecoilMass(higgs, sqrtS);

        b1_m = jets[0].m();
        b1_pt = jets[0].pt();
        b1_e = jets[0].e();

        b2_m = jets[1].m();
        b2_pt = jets[1].pt();
        b2_e = jets[1].e();

        const CLHEP::Hep3Vector b1_mom = CLHEP::Hep3Vector(
                jets[0].px(), jets[0].py(), jets[0].pz());
                
        const CLHEP::Hep3Vector b2_mom = CLHEP::Hep3Vector(
                jets[1].px(), jets[1].py(), jets[1].pz());

        higgs_bb_cosBetw = std::cos(b1_mom.angle(b2_mom));

        higgs_bTag1 = 0;
        higgs_bTag2 = 0;

        y_12 = 0;
        y_23 = 0;
        y_34 = 0;
        y_45 = 0;
        y_56 = 0;
        y_67 = 0;

        IntVec intValues = IntVec{};
        StringVec strValues = StringVec{};
        _2JetsCol->getParameters().getIntVals("PIDAlgorithmTypeID", intValues);
        _2JetsCol->getParameters().getStringVals("PIDAlgorithmTypeName", strValues);

        int algoBtag = -1;
        int algoYth = -1;
        for (unsigned int i = 0U; i < strValues.size(); ++i) {
            if (strValues[i] == "lcfiplus") {
                algoBtag = intValues[i];
            }
            if (strValues[i] == "yth") {
                algoYth = intValues[i];
            }
        }

        const ParticleIDVec particle1IDs = _2Jets[0]->getParticleIDs();
        const ParticleIDVec particle2IDs = _2Jets[1]->getParticleIDs();

        for (EVENT::ParticleID* particleID : particle1IDs) {
            if (particleID->getAlgorithmType() == algoYth) {
                const FloatVec params = particleID->getParameters();

                FloatVec yCutVec = vector<float>{};
                for (float param : params) {
                    yCutVec.push_back(param);
                }

                constexpr float minYCut = numeric_limits<float>::min();
                for (float yCut : yCutVec) {
                    yCut = max(yCut, minYCut);
                }

                y_12 = -log10(yCutVec[1]);
                y_23 = -log10(yCutVec[2]);
                y_34 = -log10(yCutVec[3]);
                y_45 = -log10(yCutVec[4]);
                y_56 = -log10(yCutVec[5]);
                y_67 = -log10(yCutVec[6]);
            }

            if (particleID->getAlgorithmType() == algoBtag) {
                higgs_bTag1 = particleID->getParameters()[0];
            }
        }

        for (EVENT::ParticleID* particleID : particle2IDs) {
            if (particleID->getAlgorithmType() == algoBtag) {
                higgs_bTag2 = particleID->getParameters()[0];
            }
        }
    }

    // 3 jets study
    if (_3Jets.size() == 3) {
        vector<fastjet::PseudoJet> jets = vector<fastjet::PseudoJet>{};
        for (EVENT::ReconstructedParticle* lcioJet : _3Jets) {
            jets.push_back(recoParticleToPseudoJet(lcioJet));
        }

        vector<fastjet::PseudoJet> osef{};

        array<fastjet::PseudoJet, 2> W_jetPair = findParticleByMass(
                jets, W_MASS_REF, osef);
        
        fastjet::PseudoJet W = join(W_jetPair[0], W_jetPair[1]);

        sl_w_m = W.m();
        sl_rec_m = computeRecoilMass(W, sqrtS);
    }
    

    // 4 jets study
    isValid_ww = (_4Jets.size() == 4);
    if (isValid_ww) {
        vector<fastjet::PseudoJet> jets = vector<fastjet::PseudoJet>{};
        for (EVENT::ReconstructedParticle* lcioJet : _4Jets) {
            jets.push_back(recoParticleToPseudoJet(lcioJet));
        }

        vector<fastjet::PseudoJet> smallW_jetPair{};

        array<fastjet::PseudoJet, 2> bigW_jetPair = findParticleByMass(
                jets, W_MASS_REF, smallW_jetPair);

        fastjet::PseudoJet bigW = join(bigW_jetPair[0], bigW_jetPair[1]);
        CLHEP::Hep3Vector bigW_mom = CLHEP::Hep3Vector(
                bigW.px(), 
                bigW.py(), 
                bigW.pz());
        CLHEP::Hep3Vector bigW_jet1Mom = CLHEP::Hep3Vector(
                bigW_jetPair[0].px(), 
                bigW_jetPair[0].py(), 
                bigW_jetPair[0].pz());
        CLHEP::Hep3Vector bigW_jet2Mom = CLHEP::Hep3Vector(
                bigW_jetPair[1].px(), 
                bigW_jetPair[1].py(), 
                bigW_jetPair[1].pz());

        fastjet::PseudoJet smallW = join(smallW_jetPair[0], smallW_jetPair[1]);
        CLHEP::Hep3Vector smallW_mom = CLHEP::Hep3Vector(
                smallW.px(), 
                smallW.py(), 
                smallW.pz());
        CLHEP::Hep3Vector smallW_jet1Mom = CLHEP::Hep3Vector(
                smallW_jetPair[0].px(), 
                smallW_jetPair[0].py(), 
                smallW_jetPair[0].pz());
        CLHEP::Hep3Vector smallW_jet2Mom = CLHEP::Hep3Vector(
                smallW_jetPair[1].px(), 
                smallW_jetPair[1].py(), 
                smallW_jetPair[1].pz());

        w1_m = bigW.m();
        w1_pt = bigW.pt();
        w1_e = bigW.e();
        w1_cosBetw = cos(bigW_jet1Mom.angle(bigW_jet2Mom));

        w2_m = smallW.m();
        w2_pt = smallW.pt();
        w2_e = smallW.e();
        w2_cosBetw = cos(smallW_jet1Mom.angle(smallW_jet2Mom));

        higgs_ww_cosBetw = cos(bigW_mom.angle(smallW_mom));

        // background study
        vector<fastjet::PseudoJet> smallZ_jetPair{};

        array<fastjet::PseudoJet, 2> bigZ_jetPair = findParticleByMass(
                jets, Z_MASS_REF, smallZ_jetPair);

        fastjet::PseudoJet bigZ = join(bigZ_jetPair[0], bigZ_jetPair[1]);
        fastjet::PseudoJet smallZ = join(smallZ_jetPair[0], smallZ_jetPair[1]);

        zz_z1_m = bigZ.m();
        zz_z2_m = smallZ.m();
    }

    outputTree->Fill();

    nEventsProcessed++;
    
    // Print 
    if (nEventsProcessed % 10000 == 0) {
        streamlog_out(MESSAGE) 
                << nEventsProcessed << " events processed" << endl;
    }
}

/**
 * Terminate processus : close tree ROOT and file.
 */
void NNHProcessor::end() {
    
    outputTree->Write();
    outputFile->Close();
}

/* protected */

/**
 * Remove all particules.
 */
void NNHProcessor::clear() {
    particles.clear();
}

/**
 * Init root tree variables in ISR process : "mc_ISR_*"
 * AE : isPhoton(gamma0) && isPhoton(gamma1)
 */
void NNHProcessor::processISR(
            const EVENT::MCParticle* gamma0, const EVENT::MCParticle* gamma1) {
    
    // Reinit mc_ISR_*
    mc_ISR_e = -1;
    mc_ISR_pt = -1;

    // Exception
    if (!isAbsPDG(PHOTON, gamma0) || !isAbsPDG(PHOTON, gamma1)) {
        throw std::logic_error("not gammas");
    }

    // 4-Vector gammas sum
    CLHEP::HepLorentzVector gamma0_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            gamma0->getMomentum()[0], gamma0->getMomentum()[1],
            gamma0->getMomentum()[2], gamma0->getEnergy());
            
    CLHEP::HepLorentzVector gamma1_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            gamma1->getMomentum()[0], gamma1->getMomentum()[1],
            gamma1->getMomentum()[2], gamma1->getEnergy());
            
    CLHEP::HepLorentzVector ISR_4Vec = gamma0_4Vec + gamma1_4Vec; // auto ? CLHEP::HepLorentzVector

    // Init mc_ISR_*
    mc_ISR_e = ISR_4Vec.e();
    mc_ISR_pt = ISR_4Vec.perp();
}

/**
 * Init root tree variables for neutrinos process : "mc_nu_*"
 * AE : isNeutrino(nu0) && isNeutrino(nu1) && isTwin(nu0, nu1)
 */
void NNHProcessor::processNeutrinos(
        const EVENT::MCParticle* nu0, const EVENT::MCParticle* nu1) {
    
    // Reinit mv_nu_*
    mc_nu_flavor = -1;
    mc_nu_e = -1;
    mc_nu_pt = -1;
    mc_nu_m = -1;
    mc_nu_cosBetw = -2;

    // Exception
    if (!isNeutrino(nu0->getPDG()) || !isNeutrino(nu1->getPDG())
            || !isSameParticleAbsPDG(nu0, nu1)) {
        throw std::logic_error("not neutrinos");
    }

    //int nu0_flavor = nu0->getPDG();
    //int nu1_flavor = nu1->getPDG();

    int flavor = std::abs(nu0->getPDG());

    //bool isNeutrinos = (nu0_flavor == -nu1_flavor) && (flavor == 12 || flavor == 14 || flavor == 16);

    CLHEP::HepLorentzVector nu0_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            nu0->getMomentum()[0], nu0->getMomentum()[1], 
            nu0->getMomentum()[2], nu0->getEnergy());
            
    CLHEP::HepLorentzVector nu1_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            nu1->getMomentum()[0], nu1->getMomentum()[1], 
            nu1->getMomentum()[2], nu1->getEnergy());
    
    // 4-Vector neutrinos sum
    CLHEP::HepLorentzVector nu_4Vec = nu0_4Vec + nu1_4Vec;
    
    // angle between 2 neutrinos 4-vector 
    double angleBetw = nu0_4Vec.v().angle(nu1_4Vec.v()); // auto ? double

    // Init mc_nu_*
    mc_nu_flavor = flavor;
    mc_nu_e = nu_4Vec.e();
    mc_nu_pt = nu_4Vec.perp();
    mc_nu_m = nu_4Vec.m();
    mc_nu_cosBetw = std::cos(angleBetw);
}

/**
 * Init root tree variables for Higgs process : "mc_higgs_*"
 * AE : isHiggs(higgs)
 */
void NNHProcessor::processHiggs(const EVENT::MCParticle* higgs) {
    
    // Reinit mc_higgs_*
    mc_higgs_e = -1;
    mc_higgs_pt = -1;
    mc_higgs_m = -1;
    mc_higgs_recMass = -1;
    mc_higgs_decay = -1;
    mc_higgs_subDecay = -1;

    mc_higgs_decay1_e = -1;
    mc_higgs_decay1_pt = -1;
    mc_higgs_decay1_m = -1;
    mc_higgs_decay2_e = -1;
    mc_higgs_decay2_pt = -1;
    mc_higgs_decay2_m = -1;
    mc_higgs_decay_cosBetw = -1;

    // Exception
    if (!isAbsPDG(HIGGS, higgs)) {
        throw logic_error("not a higgs");
    }

    CLHEP::HepLorentzVector higgs_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            higgs->getMomentum()[0], higgs->getMomentum()[1], 
            higgs->getMomentum()[2], higgs->getEnergy());

    const MCParticleVec vec = higgs->getDaughters(); // auto ? CLHEP::HepLorentzVector*

    // We need exactly 2 daughters to continue without error
    if (vec.size() != 2) {
        throw logic_error("weird higgs decay : not 2 particles");
    }

    std::array<int, 2> decay; 
    try {
        decay = findDecayMode(vec[0], vec[1]);
    } catch (exception const& e) {
        stringstream str ;
        str << e.what(); 
        throw logic_error("Higgz decay: " + str.str());
    }
                
    CLHEP::HepLorentzVector decay1_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            vec[0]->getMomentum()[0], vec[0]->getMomentum()[1],
            vec[0]->getMomentum()[2], vec[0]->getEnergy());
    
    CLHEP::HepLorentzVector decay2_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            vec[1]->getMomentum()[0], vec[1]->getMomentum()[1],
             vec[1]->getMomentum()[2], vec[1]->getEnergy());

    // we choose 1 have more energy 
    if (decay2_4Vec.e() > decay1_4Vec.e()) {
        std::swap(decay1_4Vec, decay2_4Vec);
    }

    double angleBetw = decay1_4Vec.v().angle(decay2_4Vec.v()); // auto ? double

    // Init mc_higgs_*
    mc_higgs_e = higgs_4Vec.e();
    mc_higgs_pt = higgs_4Vec.perp();
    mc_higgs_m = higgs_4Vec.m();

    mc_higgs_recMass = computeRecoilMass(higgs_4Vec, sqrtS);

    mc_higgs_decay = decay[0];
    mc_higgs_subDecay = decay[1];

    mc_higgs_decay1_e = decay1_4Vec.e();
    mc_higgs_decay1_pt = decay1_4Vec.perp();
    mc_higgs_decay1_m = decay1_4Vec.m();
    mc_higgs_decay2_e = decay2_4Vec.e();
    mc_higgs_decay2_pt = decay2_4Vec.perp();
    mc_higgs_decay2_m = decay2_4Vec.m();
    mc_higgs_decay_cosBetw = std::cos(angleBetw);
}

/**
 * AE : the particles decay in 2 particles daughter.
 * Return a array of PDG particle daughters'.
 */
vector<int> getSubDecayPDGSort(
        const EVENT::MCParticle* particle1, 
        const EVENT::MCParticle* particle2) {
            
    
    MCParticleVec vec1 = particle1->getDaughters();
    MCParticleVec vec2 = particle2->getDaughters();

    if (vec1.size() != 2 || vec2.size() != 2) {
        throw logic_error(
                "weird higgs subdecay for WW or ZZ : " 
                + to_string(vec1.size() + vec2.size()) 
                + "particles");
    }

    // all/4 pdg particle daughters
    vector<int> subDecay = {
        abs(vec1[0]->getPDG()), 
        abs(vec1[1]->getPDG()),
        abs(vec2[0]->getPDG()), 
        abs(vec2[1]->getPDG())
    };

    sort(subDecay.begin(), subDecay.end());
    
    return subDecay;
}

/**
 * Search decay2 code.
 * Return:
 *      1       if qqqq
 *      4       if qqvv
 *      7       if vvvv
 *     21       if qqee
 *     22       if qqmm
 *     23       if qqtt
 *     31       if qqev
 *     32       if qqmv
 *     33       if qqtv
 *    5XX       if llll
 *    6XX       if llvv
 * 
 * with q : quark, 
 *      v : neutrino,
 *      e : electron,
 *      m : muon,
 *      t : tau
 *     (l : charged lepton)
 */
int getDecayCode(int decay1,
        const EVENT::MCParticle* particle1, 
        const EVENT::MCParticle* particle2) {
    
    int decay2 = 0;
    if (decay1 == W || decay1 == Z0) {
        
        // get daughter particles
        vector<int> subDecay;
        try {
            subDecay = getSubDecayPDGSort(particle1, particle2);
        } catch (exception const& e) {
            cerr << "ERREUR : " << e.what() << endl;
        }

        // search decay2 code
        if (isQuark(subDecay[1])) { // qq--
            if (isQuark(subDecay[2])) { // qqq-
                if (isQuark(subDecay[3])) { // qqqq
                    decay2 = 1;
                } else {
                    throw logic_error("weird qqq- decay");
                }
            } else if (isChargedLepton(subDecay[2])) { // qql-
                try {
                    decay2 = getFlavorLeptonAbs(subDecay[2]);
                    if (isChargedLepton(subDecay[3])) { // qqll
                        decay2 += 20;
                    } else if (isNeutrino(subDecay[3])) { // qqlv
                        decay2 += 30;
                    } else {
                        throw logic_error("weird qql- decay");
                    }
                } catch (exception const& e) {
                    stringstream str ;
                    str << e.what(); 
                    throw logic_error("weird qql- decay: " + str.str());
                }
            } else if (isNeutrino(subDecay[2])) { // qqv-
                if (isNeutrino(subDecay[3])) { // qqvv
                    decay2 = 4;
                } else {
                    throw logic_error("weird qqv- decay");
                }
            } else { // 
                throw logic_error("weird qq-- decay");
            }
        } else { 
            int nbNu = getNeutrinoNb(subDecay);
            if (nbNu == 0) { // llll
                decay2 = 500;
                try {
                    decay2 = getFlavorLeptonAbs(subDecay[0]) * 10;
                } catch (exception const& e) {
                    stringstream str ;
                    str << e.what(); 
                    throw logic_error("weird llll decay: " + str.str());
                }
                
                try {
                    decay2 = getFlavorLeptonAbs(subDecay[2]);
                } catch (exception const& e) {
                    stringstream str ;
                    str << e.what(); 
                    throw logic_error("weird llll decay" + str.str());
                }
            } else if (nbNu == 2) { // llvv
                decay2 = 600;
                vector<int> temp = {};
                for (int i : subDecay) {
                    if (isChargedLepton(i)) {
                        try {
                            temp.push_back(i);
                        } catch (exception const& e) {
                            stringstream str ;
                            str << e.what(); 
                            throw logic_error("weird llvv decay: " + str.str());
                        }
                    }
                }

                if (temp.size() != 2) {
                    throw logic_error("weird llvv decay");
                }

                sort(temp.begin(), temp.end());

                decay2 = decay2 + 10 * temp[0] + temp[1];
            } else { // vvvv
                decay2 = 7;
            }
        }
    }
    
    return decay2;
}

/**
 * AE : (isPhoton(part1) || isZ0Boson(part1)) => (PDG(part1) == PDG(part2))
 * AS : {decay1, decay2}
 *          with decay1 = {PHOTON, Z0, HIGGS}
 *          and  decay2 = getDecayCode(part1, part2)       
 */
array<int, 2> NNHProcessor::findDecayMode(
        const EVENT::MCParticle* particle1, 
        const EVENT::MCParticle* particle2) const {
            
    
    // Init if error
    array<int, 2> toReturn{-1, -1};

    // Init conditions
    if (!isAbsPDG(PHOTON, particle1) && !isAbsPDG(Z0, particle1)) {
        if (!isSameParticleAbsPDG(particle1, particle2)) {
            throw logic_error(
                    "weird higgs decay : " 
                    + to_string(particle1->getPDG()) + ", " 
                    + to_string(particle2->getPDG()));
        }
    }

    // init without error
    int decay1, decay2;

    if (!isSameParticleAbsPDG(particle1, particle2)) {
        decay1 = HIGGS;
    } else {
        decay1 = abs(particle1->getPDG());
    }

    try {
        decay2 = getDecayCode(decay1, particle1, particle2);
    } catch (exception const& e) {
        stringstream str ;
        str << e.what(); 
        throw logic_error("inconnnu decay: " + str.str());
    }

    toReturn = {decay1, decay2};
    return toReturn;
}

/**
 * Compute sphericity tensor.
 */
Eigen::Matrix3d NNHProcessor::computeSphericityTensor(
            const std::vector<fastjet::PseudoJet>& particleVec) const {
                
    Eigen::Matrix3d tensor;

    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            double num = 0.;
            double denom = 0.;

            for (const fastjet::PseudoJet& particle : particleVec) {
                num += particle.four_mom()[i] * particle.four_mom()[j];
                denom += particle.modp2();
            }
            tensor(i, j) = num / denom;
        }
    }

    return tensor;
}

/**
 * Compute sphericity
 */
double NNHProcessor::computeSphericity(
            const vector<fastjet::PseudoJet>& particleVec) const {
    
    Eigen::Matrix3d tensor = computeSphericityTensor(particleVec);

    Eigen::Vector3cd eigenVal = tensor.eigenvalues();

    array<double, 3> val = {{
            norm(eigenVal(0)), 
            norm(eigenVal(1)), 
            norm(eigenVal(2))
    }};
    
    sort(val.begin(), val.end());

    streamlog_out(DEBUG) 
                << "Sphericity eigenvalues : (" 
                << val[0] << " " << val[1] << " " << val[2] << ")"
                << std::endl;

    return 1.5 * (val[0] + val[1]);
}
