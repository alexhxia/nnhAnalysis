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
#include <iostream>

/* PDG Tools */

// Leptons
int const PDG_ELECTRON = 11;
int const PDG_ELECTRON_NEUTRINO = 12;
int const PDG_MUON = 13;
int const PDG_MUON_NEUTRINO = 14;
int const PDG_TAU = 15;
int const PDG_TAU_NEUTRINO = 16;

// Bosons

int const PDG_GLUON = 21;
int const PDG_PHOTON = 22;
int const PDG_Z0 = 23;
int const PDG_W = 24;
int const PDG_HIGGS = 25;

/* TOOLS REQUEST */

/**
 * Return : 1 <= PDG(p) <= 8
 */
bool isQuarkPDG(const int pdg) {
    return 1 <= pdg && pdg <= 8;
}

bool isQuark(const EVENT::MCParticle* p) {
    return isQuarkPDG(std::abs(p->getPDG()));
}

bool isElectron(const EVENT::MCParticle* p) {
    return isElectronPDG(std::abs(p->getPDG()));
}

bool isElectronPDG(const int pdg) {
    return pdg == PDG_ELECTRON;
}

bool isMuonPDG(const int pdg) {
    return pdg == PDG_MUON;
}

bool isMuon(const EVENT::MCParticle* p) {
    return isMuonPDG(std::abs(p->getPDG()));
}

bool isTauPDG(const int pdg) {
    return pdg == PDG_TAU;
}

bool isTau(const EVENT::MCParticle* p) {
    return isTauPDG(std::abs(p->getPDG()));
}

bool isChargedLeptonPDG(const int pdg) {
    return pdg == PDG_ELECTRON || pdg == PDG_MUON || pdg == PDG_TAU;
}

bool isChargedLepton(const EVENT::MCParticle* p) {
    return isChargedLeptonPDG(std::abs(p->getPDG()));
}

bool isNeutrinoPDG(const int p) {
    return     pdg == PDG_ELECTRON_NEUTRINO
            || pdg == PDG_MUON_NEUTRINO
            || pdg == PDG_TAU_NEUTRINO;
}

bool isNeutrino(const EVENT::MCParticle* p) {
    return isNeutrinoPDG(p->getPDG());
}

bool isGluonPDG(const int pdg) {
    return pdg == PDG_GLUON;
}

bool isGluon(const EVENT::MCParticle* p) {
    return isGluonPDG(std::abs(p->getPDG()));
}

bool isPhotonPDG(const int pdg) {
    return pdg == PDG_PHOTON;
}

bool isPhoton(const EVENT::MCParticle* p) {
    return isPhotonPDG(std::abs(p->getPDG()));
}

bool isZ0BosonPDG(const int pdg) {
    return pdg == PDG_Z0;
}

bool isZ0Boson(const EVENT::MCParticle* p) {
    return isPhotonPDG(std::abs(p->getPDG()));
}

bool isWBosonPDG(const int pdg) {
    return pdg == PDG_W;
}

bool isWBoson(const EVENT::MCParticle* p) {
    return isWBosonPDG(std::abs(p->getPDG()));
}

bool isHiggsPDG(const int pdg) {
    return pdg == PDG_HIGGS;
}

bool isHiggs(const EVENT::MCParticle* p) {
    return isHiggsPDG(std::abs(p->getPDG()));
}

/**
 * Test if p1 is antiparticle to p2.
 */
bool isTwin(const EVENT::MCParticle* p1, const EVENT::MCParticle* p2) {
    return p1->getPDG() == -p2->getPDG();
}

/**
 * Test if p1 and p2 are same PDG, without sign.
 */
bool isSameAbsPDG(const EVENT::MCParticle* p1, const EVENT::MCParticle* p2) {
    return std::abs(p1->getPDG()) == std::abs(p2->getPDG());
}

/* NNHProcessor */

fastjet::PseudoJet recoParticleToPseudoJet(EVENT::ReconstructedParticle* recoPart) {
    
    const double* mom = recoPart->getMomentum();          // auto ? const double*
    double energy = recoPart->getEnergy();          // auto ? double

    fastjet::PseudoJet particle(mom[0], mom[1], mom[2], energy);
    
    ParticleInfo* partInfo = new ParticleInfo; // auto ? ParticleInfo
    partInfo->setRecoParticle(recoPart);
    
    particle.set_user_info(partInfo);
    
    return particle;
}

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

double computeRecoilMass(const fastjet::PseudoJet& particle, float energy) {
    
    const CLHEP::HepLorentzVector vec = CLHEP::HepLorentzVector( // auto ? CLHEP::HepLorentzVector
            particle.px(), particle.py(), particle.pz(), particle.e()); 
            
    return computeRecoilMass(vec, energy);
}


/**
 * 
 */
std::array<fastjet::PseudoJet, 2> findParticleByMass(
            const std::vector<fastjet::PseudoJet> jets,
            const double                          targetMass,
            std::vector<fastjet::PseudoJet>&      remainingJets) {


    std::array<unsigned int, 2> goodPair = std::array<unsigned int, 2>{}; // auto ? std::array<unsigned int, 2>
    double chi2 = std::numeric_limits<double>::max(); // auto ? double

    for (unsigned int i = 0U; i < jets.size(); ++i) { // auto ? size_t
        for (unsigned int j = i + 1; j < jets.size(); ++j) { // auto ? size_t
            
            double m = (jets[i] + jets[j]).m(); // auto ? double
            double val = std::abs(m - targetMass); // auto ? double

            if (val <= chi2) {
                chi2 = val;
                goodPair = {i, j};
            }
        }
    }

    std::array<fastjet::PseudoJet, 2> toReturn = {
                jets[goodPair[0]], jets[goodPair[1]]};

    for (unsigned int i = 0U; i < jets.size(); ++i) { // auto ? size_t
        if (i != goodPair[0] && i != goodPair[1]) {
            remainingJets.push_back(jets[i]);
        }
    }

    return toReturn;
}

int getChargedLeptonCode(const int pdg) {
    
    if (pdg == PDG_ELECTRON) {
        return 1;
    } else if (pdg == PDG_MUON) {
        return 2;
    } else if (pdg == PDG_TAU) {
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
 *      4       if qqnn
 *      7       if nnnn
 *     21       if qqee
 *     22       if qqmm
 *     23       if qqtt
 *     31       if qqen
 *     32       if qqmn
 *     33       if qqtn
 *    500 
 *    600
 * 
 * with q : quark, 
 *      n : neutrino,
 *      e : electron,
 *      m : muon,
 *      t : tau
 *     (l : lepton)
 */
int getDecayCode(
        const EVENT::MCParticle* particle1, 
        const EVENT::MCParticle* particle2) {
    
    assert((isPhoton(particle1) || isZ0Boson(particle1)) 
            && isSameAbsPDG(particle1, particle2));
    
    int decay2 = 0;
            
    const MCParticleVec daughter1 = particle1->getDaughters(); // auto ? const MCParticleVec*
    const MCParticleVec daughter2 = particle2->getDaughters(); // auto ? const MCParticleVec*

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
    if (isQuarkPDG(subDecay[1])) { // qq--
        if (isQuarkPDG(subDecay[3])) { // qqqq
            decay2 = 1;
        } else if (isNeutrinoPDG(subDecay[2]) && isNeutrinoPDG(subDecay[3])) { // qqvv
            decay2 = 4;
        } else { // qql-
            try {
                decay2 = getChargedLeptonCode(subDecay[2]);
            } catch(exception const& e) {
                std::cerr << "ERREUR : " << e.what() << endl;
            }
            if (isChargedLeptonPDG(subDecay[3])) { // qqlv
                decay2 += 20;
            } 
            if (isNeutrinoPDG(subDecay[3])) { // qqlv
                decay2 += 30;
            } // else qqll
        }
    } else {
        int nbNu = 0;
        for (const int& i : subDecay) { // auto ? int
            if (isNeutrinoPDG(i)) {
                nbNu++;
            }
        }

        if (nbNu == 0) { // llll
            decay2 = 500;
            try {
                decay2 = 10 * getChargedLeptonCode(subDecay[0]);
            } catch(exception const& e) {
                std::cerr << "ERREUR : " << e.what() << endl;
            }
            try {
                decay2 = getChargedLeptonCode(subDecay[2]);
            } catch(exception const& e) {
                std::cerr << "ERREUR : " << e.what() << endl;
            }
        } else if (nbNu == 2) { // llvv
            decay2 = 600;
            std::vector<int> temp = {};
            for (const int& i : subDecay) { // auto ? int
                if (isChargedLeptonPDG(i)) {
                    try {
                        temp.push_back(getChargedLeptonCode(i));
                    } catch(exception const& e) {
                        std::cerr << "ERREUR : " << e.what() << endl;
                    }
                }
            }

            if (temp.size() != 2) {
                throw std::logic_error("weird llvv decay");
            }
            std::sort(temp.begin(), temp.end());

            decay2 = decay2 + 10 * temp[0] + temp[1];
        } else { // vvvv
            decay2 = 7;
        }
    }
}
