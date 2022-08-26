#pragma once

#include <fastjet/PseudoJet.hh>

#include <EVENT/ReconstructedParticle.h>

class ParticleInfo : public fastjet::PseudoJet::UserInfoBase
{
  public:
    ParticleInfo() = default;
    ~ParticleInfo() = default;

    ParticleInfo(const ParticleInfo& toCopy) = delete;
    void operator=(const ParticleInfo& toCopy) = delete;

    void setRecoParticle(EVENT::ReconstructedParticle* recoPart) { _recoParticle = recoPart; }

    auto recoParticle() const { return _recoParticle; }

  protected:
    EVENT::ReconstructedParticle* _recoParticle = nullptr;
};