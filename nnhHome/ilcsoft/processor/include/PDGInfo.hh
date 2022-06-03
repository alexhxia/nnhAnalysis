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
* \return pdg == X_NEUTRINO
*/
bool isNeutrino(int pdg);

/**
* Return if particle is a charged lepton (or charged anti-lepton).
* \return |pdg| == (ELECTRON || MUON || TAU)
*/
bool isChargedLepton(int pdg);

/**
* Return if particle is a quark (or anti-quark).
* \return 1 <= |pdg| <= 9
*/
bool isQuark(int pdg);

/**
* Return if particle have this pdg, in absolute.
* \return |pdg| == |particle->getPDG()|
*/
bool isAbsPDG(const int pdg, const EVENT::MCParticle* particle);

/**
* Return if p1 have same pdg p2 in absolute.
* \return particle1->getPDG()| == |particle2->getPDG()|
*/
bool isSameParticleAbsPDG(
    const EVENT::MCParticle* p1, const EVENT::MCParticle* p2);

/**
* Return if p1 is anti-particle to p2.
* \return particle1->getPDG() == -particle2->getPDG()
*/
bool isTwinParticlePDG(
    const EVENT::MCParticle* p1, const EVENT::MCParticle* p2);
    
/**
* Return the absolute flavor of charged lepton.
* \return pdg == (ELECTRON => 1 || MUON => 2 || TAU => 3)
*/
int getFlavorLeptonAbs(int pdg);

/**
 * Return the Neutrino number contains in vecPDG;
 * \return count(isneutrino(vecPDG))
 */
int getNeutrinoNb(const std::vector<int> vecPDG);

#endif // PDG_INFO_H
