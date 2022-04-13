/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "PDGInfo.hh"

#include <EVENT/MCParticle.h>

#include <string>

#include <stdexcept>
#include <cassert>

using namespace std;

bool isNeutrino(int pdg) {
    pdg = abs(pdg);
    return     pdg == ELECTRON_NEUTRINO
            || pdg == MUON_NEUTRINO
            || pdg == TAU_NEUTRINO;
}

bool isChargedLepton(int pdg) {
    pdg = abs(pdg);
    return     pdg == ELECTRON
            || pdg == MUON
            || pdg == TAU;
}

bool isQuark(int pdg) {
    pdg = abs(pdg);
    return 1 <=  pdg && pdg <= 9;
}

bool isAbsPDG(const int pdg, const EVENT::MCParticle* particle) {
    return abs(pdg) == abs(particle->getPDG());
}

bool isSameParticleAbsPDG(
        const EVENT::MCParticle* particle1, 
        const EVENT::MCParticle* particle2) {
    
    return abs(particle1->getPDG()) == abs(particle2->getPDG());
}

bool isTwinParticlePDG(
        const EVENT::MCParticle* particle1, const EVENT::MCParticle* particle2) {
    
    return particle1->getPDG() == -particle2->getPDG();
}

int getFlavorLeptonAbs(int pdg) {
    pdg = abs(pdg);
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
