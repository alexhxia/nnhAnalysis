/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#pragma once


#include <fastjet/PseudoJet.hh> // FastJet : softwore package for jet
#include <EVENT/ReconstructedParticle.h> // LCIO

/**
 * ParticleInfo class, add to 'fastjet::PseudoJet::UserInfoBase' interface 
 * once object of 'EVENT::ReconstructedParticle'.
 */
class ParticleInfo : public fastjet::PseudoJet::UserInfoBase {
  
    public:
        // MAKERS
        ParticleInfo() = default;
        ParticleInfo(const ParticleInfo& toCopy) = delete; 
        
        // WRECKER
        ~ParticleInfo() = default;

        // REQUESTS
        EVENT::ReconstructedParticle* getReconstructedParticle() const { 
            return _recoParticle; 
        }
        
        // COMMANDS
        void operator=(const ParticleInfo& toCopy) = delete;

        void setReconstructedParticle(EVENT::ReconstructedParticle* rPart) { _
            recoParticle = rPart; 
        }
        
        // ------ BEGIN à supprimer ------ /*
        EVENT::ReconstructedParticle* recoParticle() const { 
            return _recoParticle; 
        }
        void setRecoParticle(EVENT::ReconstructedParticle* rPart) { _
            recoParticle = rPart; 
        }
        // ------ END à supprimer ------*/

    protected:
        // ATTRIBUTES
        EVENT::ReconstructedParticle* recoParticle = nullptr;
};
