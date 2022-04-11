/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "NNHProcessor.hh"

#include "EventShape.hh"
#include "ParticleInfo.hh"

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

enum PGDCode {
    // Leptons
    ELECTRON = 11,
    ELECTRON_NEUTRINO = 12,
    MUON = 13,
    MUON_NEUTRINO = 14,
    TAU = 15,
    TAU_NEUTRINO = 16,

    // Bosons
    GLUON = 21,
    PHOTON = 22,
    Z0 = 23,
    W = 24,
    HIGGS = 25
};

/**
 * Return if particle is a neutrino.
 */
bool isNeutrino(int pdg) {
    return     pdg == ELECTRON_NEUTRINO
            || pdg == MUON_NEUTRINO
            || pdg == TAU_NEUTRINO;
}

/**
 * Return if particle is a charged lepton.
 */
bool isChargedLepton(int pdg) {
    return     pdg == ELECTRON
            || pdg == MUON
            || pdg == TAU;
}

/**
 * Return if particle is a quark.
 */
bool isQuark(int pdg) {
    return 1 <=  pdg && pdg <= 9;
}

/**
 * Return if particle have this pdg in absolute.
 */
bool isAbsPDG(const int pdg, const EVENT::MCParticle* particle) {
    return pdg == std::abs(particle->getPDG());
}

/**
 * Return if p1 have same pdg p2 in absolute.
 */
bool isSameParticleAbsPDG(
        const EVENT::MCParticle* p1, const EVENT::MCParticle* p2) {
    
    return std::abs(p1->getPDG()) == std::abs(p2->getPDG());
}

/* NNHProcessor */

NNHProcessor aNNHProcessor;

/**
 * Built 'PseudoJet' with 'ReconstructedParticle'
 */
fastjet::PseudoJet recoParticleToPseudoJet(
        EVENT::ReconstructedParticle* reconstructedParticle) {
    
    const double* momentum = reconstructedParticle->getMomentum();   // auto ? const double*
    double energy = reconstructedParticle->getEnergy();              // auto ? double

    fastjet::PseudoJet particle(momentum[0], momentum[1], momentum[2], energy);
    
    ParticleInfo* particleInfo = new ParticleInfo;      // auto ? ParticleInfo
    particleInfo->setRecoParticle(reconstructedParticle);
    
    particle.set_user_info(particleInfo);
    
    return particle;
}

/**
 * Compute a recoil mass with a 4-vector and a energy
 */
double computeRecoilMass(const CLHEP::HepLorentzVector z4Vector, float energy) {
    
    CLHEP::Hep3Vector pTot = CLHEP::Hep3Vector(energy * std::sin(7e-3), 0, 0); // auto ? CLHEP::Hep3Vector
    pTot = pTot - CLHEP::Hep3Vector(z4Vector.px(), z4Vector.py(), z4Vector.pz());
    
    double rm = (energy - z4Vector.e()) * (energy - z4Vector.e()) - pTot.mag2();

    if (rm < 0.) {
        return 0.;
    } else {
        rm = std::sqrt(rm);
        return rm;
    }
}

/**
 * Compute a recoil mass with a PseudoJet and a energy
 */
double computeRecoilMass(const fastjet::PseudoJet& particle, float energy) {
    
    const CLHEP::HepLorentzVector vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            particle.px(), particle.py(), particle.pz(), particle.e()); 
            
    return computeRecoilMass(vec, energy);
}

/**
 * Search a couple particle in PseudoJet vector's
 * which minimized (invariant mass - targetMass)
 */
std::array<fastjet::PseudoJet, 2> findParticleByMass(
            const std::vector<fastjet::PseudoJet> jets,
            const double                          targetMass,
            std::vector<fastjet::PseudoJet>&      remainingJets) {


    // Search a couple particle which minimized (invariant mass - targetMass)
    std::array<unsigned int, 2> goodPair = std::array<unsigned int, 2>{}; // auto ? std::array<unsigned int, 2>
    double chi2 = std::numeric_limits<double>::max(); // auto ? double

    for (unsigned int i = 0U; i < jets.size(); ++i) { // auto ? size_t
        for (unsigned int j = i + 1; j < jets.size(); ++j) { // auto ? size_t
            
            double m = (jets[i] + jets[j]).m(); // invariant mass, auto ? double
            double val = std::abs(m - targetMass); // auto ? double

            if (val <= chi2) {
                chi2 = val;
                goodPair = {i, j};
            }
        }
    }

    std::array<fastjet::PseudoJet, 2> toReturn = {
                jets[goodPair[0]], jets[goodPair[1]]};

    // Save a not "good pair" jets
    for (unsigned int i = 0U; i < jets.size(); ++i) { // auto ? size_t
        if (i != goodPair[0] && i != goodPair[1]) {
            remainingJets.push_back(jets[i]);
        }
    }

    return toReturn;
}

/**
 * Return : 1 if pdg is pdg electron's
 *          2 if muon
 *          3 if tau
 *          throw error else.
 */
int getChargedLeptonCode(const int pdg) {
    
    if (pdg == ELECTRON) {
        return 1;
    } else if (pdg == MUON) {
        return 2;
    } else if (pdg == TAU) {
        return 3;
    } else {
        throw std::runtime_error("is not a charged lepton");
    }
}


/**
 * AE: (isPhoton(particle1) || isZ0Boson(particle1)) 
 *      && isSameAbsPDG(particle1, particle2)
 * 
 * AS:  1       if qqqq
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
 *     (l : lepton)
 */
int getDecayCode(
        const EVENT::MCParticle* particle1, 
        const EVENT::MCParticle* particle2) {
    
    assert((isAbsPDG(PHOTON, particle1) || isAbsPDG(Z0, particle1)) 
            && isSameParticleAbsPDG(particle1, particle2));
    
    int decay2 = 0;
            
    //const MCParticleVec 
    auto daughter1 = particle1->getDaughters(); // auto ? const MCParticleVec*
    //const MCParticleVec 
    auto daughter2 = particle2->getDaughters(); // auto ? const MCParticleVec*

    if (daughter1.size() != 2 || daughter2.size() != 2) {
        throw std::logic_error(
                "weird higgs subdecay for WW or ZZ : " 
                + std::to_string(daughter1.size() + daughter2.size()) 
                + "particles");
    }

    // daughter PDG array
    std::array<int, 4> subDecay = {{
            std::abs(daughter1[0]->getPDG()), 
            std::abs(daughter1[1]->getPDG()),
            std::abs(daughter2[0]->getPDG()), 
            std::abs(daughter2[1]->getPDG())
    }};

    // sorting in ascending order by daughter PDG
    std::sort(subDecay.begin(), subDecay.end());

    // if the first 2 are quark
    if (isQuark(subDecay[1])) { // qq--
        if (isQuark(subDecay[3])) { // qqqq
            decay2 = 1;
        } else if (isNeutrino(subDecay[2]) && isNeutrino(subDecay[3])) { // qqvv
            decay2 = 4;
        } else { // qql-
            try {
                decay2 = getChargedLeptonCode(subDecay[2]);
            } catch(std::exception const& e) {
                std::cerr << "ERREUR : " << e.what() << std::endl;
            }
            if (isChargedLepton(subDecay[3])) { // qqlv
                decay2 += 20;
            } 
            if (isNeutrino(subDecay[3])) { // qqlv
                decay2 += 30;
            } // else qqll
        }
    } else {
        int nbNu = 0;
        for (const int& i : subDecay) { // auto ? int
            if (isNeutrino(i)) {
                nbNu++;
            }
        }
        if (nbNu == 4) { // vvvv
            decay2 = 7;
        } else if (nbNu == 0) { // llll
            decay2 = 500;
            try {
                decay2 = 10 * getChargedLeptonCode(subDecay[0]);
            } catch(std::exception const& e) {
                std::cerr << "ERREUR : " << e.what() << std::endl;
            }
            try {
                decay2 = getChargedLeptonCode(subDecay[2]);
            } catch(std::exception const& e) {
                std::cerr << "ERREUR : " << e.what() << std::endl;
            }
        } else if (nbNu == 2) { // llvv ou lvlv ou vvll
            decay2 = 600;
            std::vector<int> temp = {};
            for (const int& i : subDecay) { // auto ? int
                if (isChargedLepton(i)) {
                    try {
                        temp.push_back(getChargedLeptonCode(i));
                    } catch(std::exception const& e) {
                        std::cerr << "ERREUR : " << e.what() << std::endl;
                    }
                }
            }

            if (temp.size() != 2) {
                throw std::logic_error("weird llvv decay");
            }
            std::sort(temp.begin(), temp.end());

            decay2 = decay2 + 10 * temp[0] + temp[1];
        } 
    }
}

/**
 * 
 */
NNHProcessor::NNHProcessor() : Processor("NNHProcessor") {
    
    // Test
    registerProcessorParameter(
            "RootFileName", "File name for the root output", 
            rootFileName, std::string("test.root"));

    // MC particles
    registerProcessorParameter(
            "MCParticlesCollectionName", "Name of the MC particles collection",
            mcParticleCollectionName, std::string("MCParticlesSkimmed"));

    // Reconstructed Particles 
    registerProcessorParameter(
            "ReconstructedParticlesCollectionName", 
            "Name of the reconstructed particles collection",
            reconstructedParticleCollectionName, 
            std::string("PandoraPFOs"));

    // Isolated Photons 
    registerProcessorParameter(
            "IsolatedPhotonsCollectionName", 
            "Name of the reconstructed isolated photon collection",
            isolatedPhotonsCollectionName, 
            std::string("IsolatedPhotons"));

    // Isolated Leptons
    registerProcessorParameter(
            "IsolatedLeptonsCollectionNames",
            "Name of the reconstructed isolated leptons collections", 
            isolatedLeptonsCollectionNames,
            {"IsolatedElectrons", "IsolatedMuons", "IsolatedTaus"});

    // Jets
    registerProcessorParameter(
            "2JetsCollectionName", "2 Jets Collection Name", 
            _2JetsCollectionName, std::string("Refined2Jets"));
    registerProcessorParameter(
            "3JetsCollectionName", "3 Jets Collection Name", 
            _3JetsCollectionName, std::string("Refined3Jets"));
    registerProcessorParameter(
            "4JetsCollectionName", "4 Jets Collection Name", 
            _4JetsCollectionName, std::string("Refined4Jets"));
}

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
        throw std::logic_error("not a higgs");
    }

    CLHEP::HepLorentzVector higgs_4Vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            higgs->getMomentum()[0], higgs->getMomentum()[1], 
            higgs->getMomentum()[2], higgs->getEnergy());

    const MCParticleVec vec = higgs->getDaughters(); // auto ? CLHEP::HepLorentzVector*

    // We need exactly 2 daughters to continue without error
    if (vec.size() != 2) {
        throw std::logic_error("weird higgs decay : not 2 particles");
    }

    std::array<int, 2> decay = findDecayMode(vec[0], vec[1]); // auto ? std::array<int, 2>

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
 * AE : (isPhoton(part1) || isZ0Boson(part1)) => (PDG(part1) == PDG(part2))
 * AS : (decay1, decay2)
 *          with decay1 = {PDG_PHOTON, PDG_Z0, PDG_HIGGS}
 *          and  decay2 = getDecayCode(part1, part2)       
 */
std::array<int, 2> NNHProcessor::findDecayMode(
        const EVENT::MCParticle* part1, const EVENT::MCParticle* part2) const {
            
    
    // init 
    std::array<int, 2> toReturn{-1, -1}; // return variable

    // Exception
    int p1_PDG = std::abs(part1->getPDG());
    int p2_PDG = std::abs(part2->getPDG());
    
    if (!isAbsPDG(PHOTON, part1) && !isAbsPDG(Z0, part1)) {
        if (p1_PDG != p2_PDG) { // ???
            throw std::logic_error(
                    "weird higgs decay : " 
                    + std::to_string(p1_PDG) + ", " 
                    + std::to_string(p2_PDG));
        }
    }

    // init variable couple to return ??? à vérifier avec Gérald
    int decay1;
    int decay2;
    if (p1_PDG != p2_PDG) {
        decay1 = HIGGS;
        decay2 = 0;
    } else {
        decay1 = p1_PDG;
        decay2 = getDecayCode(part1, part2);
    }

    toReturn = {decay1, decay2};
    return toReturn;
}

std::array<int, 2> NNHProcessor::findDecayMode(const EVENT::MCParticle* part1, const EVENT::MCParticle* part2) const
{
    std::array<int, 2> toReturn{-1, -1};

    auto decay1 = std::abs(part1->getPDG());
    auto decay2 = std::abs(part2->getPDG());

    if (decay1 != 22 && decay1 != 23)
    {
        if (decay1 != decay2)
            throw std::logic_error("weird higgs decay : " + std::to_string(decay1) + ", " + std::to_string(decay2));
    }

    if (decay1 != decay2)
        decay1 = 25;

    decay2 = 0;
    if (decay1 == 24 || decay1 == 23)
    {
        auto vec0 = part1->getDaughters();
        auto vec1 = part2->getDaughters();

        if (vec0.size() != 2 || vec1.size() != 2)
            throw std::logic_error("weird higgs subdecay for WW or ZZ : " + std::to_string(vec0.size() + vec1.size()) +
                                   "particles");

        std::array<int, 4> subDecay = {{std::abs(vec0[0]->getPDG()), std::abs(vec0[1]->getPDG()),
                                        std::abs(vec1[0]->getPDG()), std::abs(vec1[1]->getPDG())}};

        std::sort(subDecay.begin(), subDecay.end());

        if (subDecay[1] < 10) // qq--
        {
            if (subDecay[3] < 10) // qqqq
                decay2 = 1;
            else if (subDecay[2] % 2 != 0 && subDecay[3] % 2 != 0) // qqll
            {
                if (subDecay[2] == 11)
                    decay2 = 21;
                else if (subDecay[2] == 13)
                    decay2 = 22;
                else if (subDecay[2] == 15)
                    decay2 = 23;
                else
                    throw std::logic_error("weird qqll decay");
            }
            else if (subDecay[2] % 2 == 0 && subDecay[3] % 2 == 0) // qqvv
            {
                decay2 = 4;
            }
            else // qqlv
            {
                if (subDecay[2] == 11)
                    decay2 = 31;
                else if (subDecay[2] == 13)
                    decay2 = 32;
                else if (subDecay[2] == 15)
                    decay2 = 33;
                else
                    throw std::logic_error("weird qqlv decay");
            }
        }
        else
        {
            int nbNu = 0;
            for (const auto& i : subDecay)
            {
                if (i % 2 == 0)
                    nbNu++;
            }

            if (nbNu == 0) // llll
            {
                decay2 = 500;
                if (subDecay[0] == 11)
                    decay2 += 10;
                else if (subDecay[0] == 13)
                    decay2 += 20;
                else if (subDecay[0] == 15)
                    decay2 += 30;
                else
                    throw std::logic_error("weird llll decay");

                if (subDecay[2] == 11)
                    decay2 += 1;
                else if (subDecay[2] == 13)
                    decay2 += 2;
                else if (subDecay[2] == 15)
                    decay2 += 3;
                else
                    throw std::logic_error("weird llll decay");
            }
            else if (nbNu == 2) // llvv
            {
                decay2 = 600;
                std::vector<int> temp = {};
                for (const auto& i : subDecay)
                {
                    if (i % 2 != 0)
                    {
                        if (i == 11)
                            temp.push_back(1);
                        else if (i == 13)
                            temp.push_back(2);
                        else if (i == 15)
                            temp.push_back(3);
                        else
                            throw std::logic_error("weird llvv decay");
                    }
                }

                if (temp.size() != 2)
                    throw std::logic_error("weird llvv decay");

                std::sort(temp.begin(), temp.end());

                decay2 = decay2 + 10 * temp[0] + temp[1];
            }
            else // vvvv
            {
                decay2 = 7;
            }
        }
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

            for (const fastjet::PseudoJet& particle : particleVec) { // auto ? fastjet::PseudoJet
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
            const std::vector<fastjet::PseudoJet>& particleVec) const {
    
    Eigen::Matrix3d tensor = computeSphericityTensor(particleVec); // auto ? Eigen::Matrix3d

    Eigen::Vector3cd eigenVal = tensor.eigenvalues(); // auto ? Eigen::Vector3cd

    std::array<double, 3> val = {{
            std::norm(eigenVal(0)), 
            std::norm(eigenVal(1)), 
            std::norm(eigenVal(2))
    }};
    
    std::sort(val.begin(), val.end());

    streamlog_out(DEBUG) 
                << "Sphericity eigenvalues : (" 
                << val[0] << " " << val[1] << " " << val[2] << ")"
                << std::endl;

    return 1.5 * (val[0] + val[1]);
}

/**
 * 
 */
/*void NNHProcessor::processEvent(LCEvent* evt) {
    
    clear();

    std::cout << "Event : " << evt->getEventNumber() << std::endl;
*/
    /* event variables */
    /*processID = evt->getParameters().getIntVal(std::string("ProcessID"));
    event = evt->getParameters().getIntVal(std::string("Event Number"));
    sqrtS = evt->getParameters().getFloatVal(std::string("Energy"));

    mcCol = evt->getCollection(mcParticleCollectionName); //  LCCollection*
    recoCol = evt->getCollection(reconstructedParticleCollectionName); //  LCCollection*
    */
    /* MC stuff */
    
    // process ISR
  /*  const EVENT::MCParticle* mc_gamma0 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(6)); // auto ? EVENT::MCParticle*
    const EVENT::MCParticle* mc_gamma1 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(7)); // auto ? EVENT::MCParticle*
    try {
        processISR(mc_gamma0, mc_gamma1);
    } catch (std::logic_error& e) {
        std::cerr //streamlog_out(DEBUG) 
                << "Run : " << evt->getRunNumber() << ", "
                << "Event : " << evt->getEventNumber() << " : "
                << e.what() << std::endl;
    }
    
    // process Neutrinos
    const EVENT::MCParticle* mc_nu0 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(8)); // auto ? EVENT::MCParticle*
    const EVENT::MCParticle* mc_nu1 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(9)); // auto ? EVENT::MCParticle*
    try {
        processNeutrinos(mc_nu0, mc_nu1);
    } catch (std::logic_error& e) {
        streamlog_out(DEBUG) 
                << "Run : " << evt->getRunNumber() << ", "
                << "Event : " << evt->getEventNumber() << " : "
                << e.what() << std::endl;
    }
    
    // process Higgs
    const EVENT::MCParticle* mc_higgs = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(10)); // auto ? EVENT::MCParticle*
    try {
        processHiggs(mc_higgs);
    } catch (std::logic_error& e) {
        streamlog_out(DEBUG) 
                << "Run : " << evt->getRunNumber() << ", "
                << "Event : " << evt->getEventNumber() << " : "
                << e.what() << std::endl;
    }*/
    /* end of MC stuff */
    /*
    principleThrust = recoCol->getParameters().getFloatVal("principleThrustValue");
    majorThrust = recoCol->getParameters().getFloatVal("majorThrustValue");
    minorThrust = recoCol->getParameters().getFloatVal("minorThrustValue");

    std::vector<float> ta = FloatVec{}; // auto  ? std::vector<float>
    recoCol->getParameters().getFloatVals("principleThrustAxis", ta);
    const CLHEP::Hep3Vector principleThrustAxis = CLHEP::Hep3Vector(ta[0], ta[1], ta[2]); // auto ? CLHEP::Hep3Vector

    cosThrust = std::abs(principleThrustAxis.cosTheta());
    oblateness = recoCol->getParameters().getFloatVal("Oblateness");

    if (minorThrust != minorThrust) { // handle NaN case
        minorThrust = 0.;
    }
    
    sphericity = recoCol->getParameters().getFloatVal("sphericity");

    // treat isolated leptons
    isolatedLeptons.clear();
    eIsoLep = 0;
    for (const std::string colName : isolatedLeptonsCollectionNames) { // auto ? std::string
         LCCollection* col = evt->getCollection(colName); // auto ? LCCollection*
        int n = col->getNumberOfElements(); // auto ? int

        for (int i = 0; i < n; ++i) { // auto ? int
            EVENT::ReconstructedParticle* particle = dynamic_cast<EVENT::ReconstructedParticle*>(col->getElementAt(i)); // auto ? EVENT::ReconstructedParticle*
            isolatedLeptons.insert(particle);
            eIsoLep += particle->getEnergy();
        }
    }
    nIsoLep = isolatedLeptons.size();

    // treat isolated photons
    isolatedPhotons.clear();
    {
         LCCollection* col = evt->getCollection(isolatedPhotonsCollectionName); // auto ?  LCCollection*
        int n = col->getNumberOfElements(); // auto ? int

        for (int i = 0; i < n; ++i) {
            EVENT::ReconstructedParticle* particle = dynamic_cast<EVENT::ReconstructedParticle*>(col->getElementAt(i)); // auto ? EVENT::ReconstructedParticle*
            isolatedPhotons.insert(particle);
        }
    }

    nParticles = recoCol->getNumberOfElements();
    visible_e = 0;

    particles.reserve(nParticles);

    for (int index = 0; index < nParticles; ++index) {
        EVENT::ReconstructedParticle* recoPart 
                = dynamic_cast<EVENT::ReconstructedParticle*>
                        (recoCol->getElementAt(index)); // auto ? EVENT::ReconstructedParticle*

        visible_e += recoPart->getEnergy();

        fastjet::PseudoJet particle = recoParticleToPseudoJet(recoPart); // auto ? fastjet::PseudoJet
        particles.push_back(particle);
    }

    // Jets study

    const auto sortJetsByEnergy = []( // auto ???
            const EVENT::ReconstructedParticle* a,
            const EVENT::ReconstructedParticle* b) -> bool { 
        return a->getEnergy() > b->getEnergy(); 
    };

    const LCCollection* _2JetsCol = evt->getCollection(_2JetsCollectionName); // auto ? LCCollection*
    const LCCollection* _3JetsCol = evt->getCollection(_3JetsCollectionName); // auto ? LCCollection*
    const LCCollection* _4JetsCol = evt->getCollection(_4JetsCollectionName); // auto ? LCCollection*

    std::vector<EVENT::ReconstructedParticle*> _2Jets = std::vector<EVENT::ReconstructedParticle*>{}; // auto ? std::vector<EVENT::ReconstructedParticle*>
    std::vector<EVENT::ReconstructedParticle*> _3Jets = std::vector<EVENT::ReconstructedParticle*>{}; // auto ? std::vector<EVENT::ReconstructedParticle*>
    std::vector<EVENT::ReconstructedParticle*> _4Jets = std::vector<EVENT::ReconstructedParticle*>{}; // auto ? std::vector<EVENT::ReconstructedParticle*>

    int n = _2JetsCol->getNumberOfElements();
    for (int index = 0; index < n; ++index) { // auto ? int
        _2Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(
                _2JetsCol->getElementAt(index)));
    }
    
    n = _3JetsCol->getNumberOfElements();
    for (int index = 0; index < n; ++index) { // auto ? int
        _3Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(
                _3JetsCol->getElementAt(index)));
    }
    
    n = _4JetsCol->getNumberOfElements();
    for (int index = 0; index < n; ++index) { // auto ? int
        _4Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(
                _4JetsCol->getElementAt(index)));
    }

    std::sort(_2Jets.begin(), _2Jets.end(), sortJetsByEnergy);
    std::sort(_3Jets.begin(), _3Jets.end(), sortJetsByEnergy);
    std::sort(_4Jets.begin(), _4Jets.end(), sortJetsByEnergy);

    isValid_bb = (_2Jets.size() == 2);
    if (isValid_bb) {
        std::vector<fastjet::PseudoJet> jets = std::vector<fastjet::PseudoJet>{}; // auto ? std::vector<fastjet::PseudoJet
        for (EVENT::ReconstructedParticle* lcioJet : _2Jets) { // auto ? fastjet::PseudoJet
            jets.push_back(recoParticleToPseudoJet(lcioJet));
        }

        const fastjet::PseudoJet higgs = join(jets[0], jets[1]); // auto ? fastjet::PseudoJet
        const CLHEP::Hep3Vector higgs_mom = CLHEP::Hep3Vector( // auto ? CLHEP::Hep3Vector
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

        const CLHEP::Hep3Vector b1_mom = CLHEP::Hep3Vector(// auto ? CLHEP::Hep3Vector
                jets[0].px(), jets[0].py(), jets[0].pz());
        const CLHEP::Hep3Vector b2_mom = CLHEP::Hep3Vector(// auto ? CLHEP::Hep3Vector
                jets[1].px(), jets[1].py(), jets[1].pz());

        higgs_bb_cosBetw = std::cos(b1_mom.angle(b2_mom));

        higgs_bTag1 = 0.;
        higgs_bTag2 = 0.;

        y_12 = 0.;
        y_23 = 0.;
        y_34 = 0.;
        y_45 = 0.;
        y_56 = 0.;
        y_67 = 0.;

        IntVec intValues = IntVec{}; // auto ? vector<int*>
        StringVec strValues = StringVec{}; // auto ? vector<string*>
        _2JetsCol->getParameters().getIntVals("PIDAlgorithmTypeID", intValues);
        _2JetsCol->getParameters().getStringVals("PIDAlgorithmTypeName", strValues);

        int algoBtag = -1; // auto ? int
        int algoYth = -1; // auto ? int
        for (size_t i = 0U; i < strValues.size(); ++i) {
            if (strValues[i] == "lcfiplus") {
                algoBtag = intValues[i];
            }
            if (strValues[i] == "yth") {
                algoYth = intValues[i];
            }
        }

        const EVENT::ParticleIDVec particle1IDs = _2Jets[0]->getParticleIDs(); // auto ? int*
        const EVENT::ParticleIDVec particle2IDs = _2Jets[1]->getParticleIDs(); // auto ? int*

        for (EVENT::ParticleID* particleID : particle1IDs) {// auto ? int
            if (particleID->getAlgorithmType() == algoYth) {
                const FloatVec params = particleID->getParameters(); // auto ? int*

                std::vector<float> yCutVec = std::vector<float>{}; // auto ? std::vector<float>
                for (const float& param : params) {   // auto ? float
                    yCutVec.push_back(param);
                }

                constexpr float minYCut = std::numeric_limits<float>::min();
                for (float& yCut : yCutVec) { // auto ? float
                    yCut = std::max(yCut, minYCut);
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

        for (const EVENT::ParticleID* particleID : particle2IDs) { // auto int
            if (particleID->getAlgorithmType() == algoBtag) {
                higgs_bTag2 = particleID->getParameters()[0];
            }
        }
    }

    // 3 jets study
    if (_3Jets.size() == 3) {
        std::vector<fastjet::PseudoJet> jets = std::vector<fastjet::PseudoJet>{}; // auto ? std::vector<fastjet::PseudoJet>
        for (EVENT::ReconstructedParticle* lcioJet : _3Jets) // auto ? fastjet::PseudoJet
            jets.push_back(recoParticleToPseudoJet(lcioJet));

        std::vector<fastjet::PseudoJet> osef{};

        std::array<fastjet::PseudoJet, 2> W_jetPair = findParticleByMass(jets, W_MASS_REF, osef); // auto ? std::array<fastjet::PseudoJet, 2>
        fastjet::PseudoJet W = join(W_jetPair[0], W_jetPair[1]); // auto ? fastjet::PseudoJet

        sl_w_m = W.m();
        sl_rec_m = computeRecoilMass(W, sqrtS);
    }

    // 4 jets study
    isValid_ww = (_4Jets.size() == 4);
    if (isValid_ww) {
        std::vector<fastjet::PseudoJet> jets = std::vector<fastjet::PseudoJet>{}; // auto ? std::vector<fastjet::PseudoJet>
        for (EVENT::ReconstructedParticle* lcioJet : _4Jets) { // auto ? fastjet::PseudoJet
            jets.push_back(recoParticleToPseudoJet(lcioJet));
        }

        std::vector<fastjet::PseudoJet> smallW_jetPair{};

        std::array<fastjet::PseudoJet, 2> bigW_jetPair = findParticleByMass(jets, W_MASS_REF, smallW_jetPair); // auto ? std::array<fastjet::PseudoJet, 2>

        fastjet::PseudoJet bigW = join(bigW_jetPair[0], bigW_jetPair[1]); // auto ? fastjet::PseudoJet
        CLHEP::Hep3Vector bigW_mom = CLHEP::Hep3Vector(
                bigW.px(), bigW.py(), bigW.pz()); // auto ? CLHEP::Hep3Vector
        CLHEP::Hep3Vector bigW_jet1Mom = CLHEP::Hep3Vector(
                bigW_jetPair[0].px(), bigW_jetPair[0].py(), bigW_jetPair[0].pz()); // auto ? CLHEP::Hep3Vector
        CLHEP::Hep3Vector bigW_jet2Mom = CLHEP::Hep3Vector(
                bigW_jetPair[1].px(), bigW_jetPair[1].py(), bigW_jetPair[1].pz()); // auto ? CLHEP::Hep3Vector

        fastjet::PseudoJet smallW = join(smallW_jetPair[0], smallW_jetPair[1]); // auto ? fastjet::PseudoJet
        CLHEP::Hep3Vector smallW_mom = CLHEP::Hep3Vector(
                smallW.px(), smallW.py(), smallW.pz()); // auto ? CLHEP::Hep3Vector
        CLHEP::Hep3Vector smallW_jet1Mom = CLHEP::Hep3Vector(
                smallW_jetPair[0].px(), smallW_jetPair[0].py(), smallW_jetPair[0].pz()); // auto ? CLHEP::Hep3Vector
        CLHEP::Hep3Vector smallW_jet2Mom = CLHEP::Hep3Vector(
                smallW_jetPair[1].px(), smallW_jetPair[1].py(), smallW_jetPair[1].pz()); // auto ? CLHEP::Hep3Vector

        w1_m = bigW.m();
        w1_pt = bigW.pt();
        w1_e = bigW.e();
        w1_cosBetw = std::cos(bigW_jet1Mom.angle(bigW_jet2Mom));

        w2_m = smallW.m();
        w2_pt = smallW.pt();
        w2_e = smallW.e();
        w2_cosBetw = std::cos(smallW_jet1Mom.angle(smallW_jet2Mom));

        higgs_ww_cosBetw = std::cos(bigW_mom.angle(smallW_mom));

        // background study
        std::vector<fastjet::PseudoJet> smallZ_jetPair{};

        std::array<fastjet::PseudoJet, 2> bigZ_jetPair = findParticleByMass(
                jets, Z_MASS_REF, smallZ_jetPair); // auto ? std::array<fastjet::PseudoJet, 2>

        fastjet::PseudoJet bigZ = join(bigZ_jetPair[0], bigZ_jetPair[1]); // auto ? fastjet::PseudoJet
        fastjet::PseudoJet smallZ = join(smallZ_jetPair[0], smallZ_jetPair[1]); // auto ? fastjet::PseudoJet

        zz_z1_m = bigZ.m();     // invariant mass
        zz_z2_m = smallZ.m();   // invariant mass
    }

    outputTree->Fill();

    nEventsProcessed++;
    
    // Print all 10.000 processes
    if (nEventsProcessed % 10000 == 0) {
        streamlog_out(MESSAGE) 
                << nEventsProcessed << " events processed" 
                << std::endl;
    }
}*/

void NNHProcessor::processEvent(LCEvent* evt)
{
    clear();

    std::cout << "Event : " << evt->getEventNumber() << std::endl;

    processID = evt->getParameters().getIntVal(std::string("ProcessID"));
    event = evt->getParameters().getIntVal(std::string("Event Number"));
    sqrtS = evt->getParameters().getFloatVal(std::string("Energy"));

    mcCol = evt->getCollection(mcParticleCollectionName);
    recoCol = evt->getCollection(reconstructedParticleCollectionName);

    // MC stuff
    const auto mc_gamma0 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(6));
    const auto mc_gamma1 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(7));
    const auto mc_nu0 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(8));
    const auto mc_nu1 = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(9));
    const auto mc_higgs = dynamic_cast<EVENT::MCParticle*>(mcCol->getElementAt(10));

    try
    {
        processISR(mc_gamma0, mc_gamma1);
    }
    catch (std::logic_error& e)
    {
        streamlog_out(DEBUG) << "Run : " << evt->getRunNumber() << ", Event : " << evt->getEventNumber() << " : "
                             << e.what() << std::endl;
    }
    try
    {
        processNeutrinos(mc_nu0, mc_nu1);
    }
    catch (std::logic_error& e)
    {
        streamlog_out(DEBUG) << "Run : " << evt->getRunNumber() << ", Event : " << evt->getEventNumber() << " : "
                             << e.what() << std::endl;
    }
    try
    {
        processHiggs(mc_higgs);
    }
    catch (std::logic_error& e)
    {
        streamlog_out(DEBUG) << "Run : " << evt->getRunNumber() << ", Event : " << evt->getEventNumber() << " : "
                             << e.what() << std::endl;
    }
    // end of MC stuff

    principleThrust = recoCol->getParameters().getFloatVal("principleThrustValue");
    majorThrust = recoCol->getParameters().getFloatVal("majorThrustValue");
    minorThrust = recoCol->getParameters().getFloatVal("minorThrustValue");

    auto ta = FloatVec{};
    recoCol->getParameters().getFloatVals("principleThrustAxis", ta);
    const auto principleThrustAxis = CLHEP::Hep3Vector(ta[0], ta[1], ta[2]);

    cosThrust = std::abs(principleThrustAxis.cosTheta());
    oblateness = recoCol->getParameters().getFloatVal("Oblateness");

    if (minorThrust != minorThrust) // handle NaN case
        minorThrust = 0;

    sphericity = recoCol->getParameters().getFloatVal("sphericity");

    // treat isolated leptons
    isolatedLeptons.clear();
    eIsoLep = 0;
    for (const auto& colName : isolatedLeptonsCollectionNames)
    {
        auto col = evt->getCollection(colName);
        auto n = col->getNumberOfElements();

        for (auto i = 0; i < n; ++i)
        {
            auto particle = dynamic_cast<EVENT::ReconstructedParticle*>(col->getElementAt(i));
            isolatedLeptons.insert(particle);
            eIsoLep += particle->getEnergy();
        }
    }
    nIsoLep = isolatedLeptons.size();

    isolatedPhotons.clear();
    {
        auto col = evt->getCollection(isolatedPhotonsCollectionName);
        auto n = col->getNumberOfElements();

        for (auto i = 0; i < n; ++i)
        {
            auto particle = dynamic_cast<EVENT::ReconstructedParticle*>(col->getElementAt(i));
            isolatedPhotons.insert(particle);
        }
    }

    nParticles = recoCol->getNumberOfElements();
    visible_e = 0;

    particles.reserve(nParticles);

    for (int index = 0; index < nParticles; ++index)
    {
        auto recoPart = dynamic_cast<EVENT::ReconstructedParticle*>(recoCol->getElementAt(index));

        visible_e += recoPart->getEnergy();

        auto particle = recoParticleToPseudoJet(recoPart);
        particles.push_back(particle);
    }

    // Jets study

    const auto sortJetsByEnergy = [](const EVENT::ReconstructedParticle* a,
                                     const EVENT::ReconstructedParticle* b) -> bool
    { return a->getEnergy() > b->getEnergy(); };

    const auto _2JetsCol = evt->getCollection(_2JetsCollectionName);
    const auto _3JetsCol = evt->getCollection(_3JetsCollectionName);
    const auto _4JetsCol = evt->getCollection(_4JetsCollectionName);

    auto _2Jets = std::vector<EVENT::ReconstructedParticle*>{};
    auto _3Jets = std::vector<EVENT::ReconstructedParticle*>{};
    auto _4Jets = std::vector<EVENT::ReconstructedParticle*>{};

    for (auto index = 0; index < _2JetsCol->getNumberOfElements(); ++index)
        _2Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(_2JetsCol->getElementAt(index)));
    for (auto index = 0; index < _3JetsCol->getNumberOfElements(); ++index)
        _3Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(_3JetsCol->getElementAt(index)));
    for (auto index = 0; index < _4JetsCol->getNumberOfElements(); ++index)
        _4Jets.push_back(dynamic_cast<EVENT::ReconstructedParticle*>(_4JetsCol->getElementAt(index)));

    std::sort(_2Jets.begin(), _2Jets.end(), sortJetsByEnergy);
    std::sort(_3Jets.begin(), _3Jets.end(), sortJetsByEnergy);
    std::sort(_4Jets.begin(), _4Jets.end(), sortJetsByEnergy);

    if (_2Jets.size() != 2)
        isValid_bb = false;
    else
    {
        isValid_bb = true;
        auto jets = std::vector<fastjet::PseudoJet>{};
        for (const auto& lcioJet : _2Jets)
            jets.push_back(recoParticleToPseudoJet(lcioJet));

        const auto higgs = join(jets[0], jets[1]);
        const auto higgs_mom = CLHEP::Hep3Vector(jets[0].px(), jets[0].py(), jets[0].pz());

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

        const auto b1_mom = CLHEP::Hep3Vector(jets[0].px(), jets[0].py(), jets[0].pz());
        const auto b2_mom = CLHEP::Hep3Vector(jets[1].px(), jets[1].py(), jets[1].pz());

        higgs_bb_cosBetw = std::cos(b1_mom.angle(b2_mom));

        higgs_bTag1 = 0;
        higgs_bTag2 = 0;

        y_12 = 0;
        y_23 = 0;
        y_34 = 0;
        y_45 = 0;
        y_56 = 0;
        y_67 = 0;

        auto intValues = IntVec{};
        auto strValues = StringVec{};
        _2JetsCol->getParameters().getIntVals("PIDAlgorithmTypeID", intValues);
        _2JetsCol->getParameters().getStringVals("PIDAlgorithmTypeName", strValues);

        auto algoBtag = -1;
        auto algoYth = -1;
        for (auto i = 0U; i < strValues.size(); ++i)
        {
            if (strValues[i] == "lcfiplus")
                algoBtag = intValues[i];
            if (strValues[i] == "yth")
                algoYth = intValues[i];
        }

        const auto particle1IDs = _2Jets[0]->getParticleIDs();
        const auto particle2IDs = _2Jets[1]->getParticleIDs();

        for (const auto& particleID : particle1IDs)
        {
            if (particleID->getAlgorithmType() == algoYth)
            {
                const auto params = particleID->getParameters();

                auto yCutVec = std::vector<float>{};
                for (const auto& param : params)
                    yCutVec.push_back(param);

                constexpr float minYCut = std::numeric_limits<float>::min();
                for (auto& yCut : yCutVec)
                    yCut = std::max(yCut, minYCut);

                y_12 = -log10(yCutVec[1]);
                y_23 = -log10(yCutVec[2]);
                y_34 = -log10(yCutVec[3]);
                y_45 = -log10(yCutVec[4]);
                y_56 = -log10(yCutVec[5]);
                y_67 = -log10(yCutVec[6]);
            }

            if (particleID->getAlgorithmType() == algoBtag)
                higgs_bTag1 = particleID->getParameters()[0];
        }

        for (const auto& particleID : particle2IDs)
        {
            if (particleID->getAlgorithmType() == algoBtag)
                higgs_bTag2 = particleID->getParameters()[0];
        }
    }

    // 3 jets study
    if (_3Jets.size() == 3)
    {
        auto jets = std::vector<fastjet::PseudoJet>{};
        for (const auto& lcioJet : _3Jets)
            jets.push_back(recoParticleToPseudoJet(lcioJet));

        std::vector<fastjet::PseudoJet> osef{};

        auto W_jetPair = findParticleByMass(jets, W_MASS_REF, osef);
        auto W = join(W_jetPair[0], W_jetPair[1]);

        sl_w_m = W.m();
        sl_rec_m = computeRecoilMass(W, sqrtS);
    }

    // 4 jets study
    if (_4Jets.size() != 4)
        isValid_ww = false;
    else
    {
        isValid_ww = true;

        auto jets = std::vector<fastjet::PseudoJet>{};
        for (const auto& lcioJet : _4Jets)
            jets.push_back(recoParticleToPseudoJet(lcioJet));

        std::vector<fastjet::PseudoJet> smallW_jetPair{};

        auto bigW_jetPair = findParticleByMass(jets, W_MASS_REF, smallW_jetPair);

        auto bigW = join(bigW_jetPair[0], bigW_jetPair[1]);
        auto bigW_mom = CLHEP::Hep3Vector(bigW.px(), bigW.py(), bigW.pz());
        auto bigW_jet1Mom = CLHEP::Hep3Vector(bigW_jetPair[0].px(), bigW_jetPair[0].py(), bigW_jetPair[0].pz());
        auto bigW_jet2Mom = CLHEP::Hep3Vector(bigW_jetPair[1].px(), bigW_jetPair[1].py(), bigW_jetPair[1].pz());

        auto smallW = join(smallW_jetPair[0], smallW_jetPair[1]);
        auto smallW_mom = CLHEP::Hep3Vector(smallW.px(), smallW.py(), smallW.pz());
        auto smallW_jet1Mom = CLHEP::Hep3Vector(smallW_jetPair[0].px(), smallW_jetPair[0].py(), smallW_jetPair[0].pz());
        auto smallW_jet2Mom = CLHEP::Hep3Vector(smallW_jetPair[1].px(), smallW_jetPair[1].py(), smallW_jetPair[1].pz());

        w1_m = bigW.m();
        w1_pt = bigW.pt();
        w1_e = bigW.e();
        w1_cosBetw = std::cos(bigW_jet1Mom.angle(bigW_jet2Mom));

        w2_m = smallW.m();
        w2_pt = smallW.pt();
        w2_e = smallW.e();
        w2_cosBetw = std::cos(smallW_jet1Mom.angle(smallW_jet2Mom));

        higgs_ww_cosBetw = std::cos(bigW_mom.angle(smallW_mom));

        // background study
        std::vector<fastjet::PseudoJet> smallZ_jetPair{};

        auto bigZ_jetPair = findParticleByMass(jets, Z_MASS_REF, smallZ_jetPair);

        auto bigZ = join(bigZ_jetPair[0], bigZ_jetPair[1]);
        auto smallZ = join(smallZ_jetPair[0], smallZ_jetPair[1]);

        zz_z1_m = bigZ.m();
        zz_z2_m = smallZ.m();
    }

    outputTree->Fill();

    nEventsProcessed++;
    if (nEventsProcessed % 10000 == 0)
        streamlog_out(MESSAGE) << nEventsProcessed << " events processed" << std::endl;
}


/**
 * Terminate processus : close tree ROOT and file.
 */
void NNHProcessor::end() {
    
    outputTree->Write();
    outputFile->Close();
}
