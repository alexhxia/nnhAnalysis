/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#pragma once

#include <fastjet/PseudoJet.hh>

#include <EVENT/ReconstructedParticle.h>

/**
 * ParticleInfo class, add to 'fastjet::PseudoJet::UserInfoBase' interface 
 * once object of 'EVENT::ReconstructedParticle'.
 */
class ParticleInfo : public fastjet::PseudoJet::UserInfoBase {
    
    public:
    
        // MARKER
        
        ParticleInfo() = default;
        ~ParticleInfo() = default;

        // LEGASIES
        
        ParticleInfo(const ParticleInfo& toCopy) = delete;
        void operator=(const ParticleInfo& toCopy) = delete;

        // REQUEST
        
        /**
         * \return _reconstructedParticle
         */
        EVENT::ReconstructedParticle* getReconstructedParticle() const;
        
        // COMMAND
        
        /**
         * \post _reconstructedParticle == recoPart
         */
        void setReconstructedParticle(EVENT::ReconstructedParticle* recoPart);

    protected:
        
        // ATTRIBUTES
        
        EVENT::ReconstructedParticle* _reconstructedParticle = nullptr;
};
