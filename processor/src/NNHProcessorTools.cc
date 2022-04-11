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
bool isQuark(int pdg) const {
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
bool isSameParticleAbsPDG(const EVENT::MCParticle* p1, const EVENT::MCParticle* p2) {
    return std::abs(p1->getPDG()) == std::abs(p2->getPDG());
}

/* NNHProcessor */

/**
 * Built 'PseudoJet' with 'ReconstructedParticle'
 */
fastjet::PseudoJet recoParticleToPseudoJet(
        EVENT::ReconstructedParticle* reconstructedParticle) {
    
    const double* momentum = recoPart->getMomentum();   // auto ? const double*
    double energy = recoPart->getEnergy();              // auto ? double

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
