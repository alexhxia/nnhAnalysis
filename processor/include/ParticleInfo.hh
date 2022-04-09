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
        ParticleInfo();
        ~ParticleInfo() = default;

        ParticleInfo(const ParticleInfo& toCopy) = delete;
        void operator=(const ParticleInfo& toCopy) = delete;

        void setRecoParticle(EVENT::ReconstructedParticle* recoPart);

        EVENT::ReconstructedParticle* recoParticle() const;

    protected:
        EVENT::ReconstructedParticle* _recoParticle; // = nullptr;
};
