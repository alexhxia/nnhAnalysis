/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/


#include "ParticleInfo.hh"

// REQUEST 

EVENT::ReconstructedParticle* ParticleInfo::getReconstructedParticle() const { 
    
    return _reconstructedParticle; 
}

// COMMAND

void ParticleInfo::setReconstructedParticle(
            EVENT::ReconstructedParticle* recoPart) { 
                
    _reconstructedParticle = recoPart; 
}

