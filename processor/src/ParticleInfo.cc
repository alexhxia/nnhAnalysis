/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/


#include "ParticleInfo.hh"

// MAKER
ParticleInfo::ParticleInfo() {
    _recoParticle = nullptr;
}

// REQUEST 

EVENT::ReconstructedParticle* ParticleInfo::recoParticle() const { 
    return _recoParticle; 
}

// COMMAND

void ParticleInfo::setRecoParticle(EVENT::ReconstructedParticle* recoPart) { 
    _recoParticle = recoPart; 
}

