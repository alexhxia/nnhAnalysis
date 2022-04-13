/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#ifndef PDG_INFO_H
#define PDG_INFO_H

#include <string>

#include "EVENT/MCParticle.h"

/**
 * PDGInfo class is a util class for help to readability.
 */
 
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

// REQUESTS

/**
* Return if particle is a neutrinoo (or anti-neutrino).
*/
bool isNeutrino(int pdg);

/**
* Return if particle is a charged lepton (or charged anti-lepton).
*/
bool isChargedLepton(int pdg);

/**
* Return if particle is a quark (or anti-quark).
*/
bool isQuark(int pdg);

/**
* Return if particle have this pdg, in absolute.
*/
bool isAbsPDG(const int pdg, const EVENT::MCParticle* particle);

/**
* Return if p1 have same pdg p2 in absolute.
*/
bool isSameParticleAbsPDG(
    const EVENT::MCParticle* p1, const EVENT::MCParticle* p2);

/**
* Return if p1 is anti-particle to p2.
*/
bool isTwinParticlePDG(
    const EVENT::MCParticle* p1, const EVENT::MCParticle* p2);
    
/**
* Return the absolute flavor of charged lepton.
*/
int getFlavorLeptonAbs(int pdg);

/**
 * Return the Neutrino number contains in vecPDG;
 */
int getNeutrinoNb(const std::vector<int> vecPDG);

#endif // PDG_INFO_H
