# nnhAnalysis/processor/include

# ``ParticlesInfo.hh``

## Include 

- ``fastjet/PseudoJet.hh`` appartient au projet ``FastJet`` (la documentation http://fastjet.fr/)
Ce paquet permet de retrouver les jets dans les collision pp et ee-.

Dans ``ParticlesInfo``, on étend la classe ``fastjet::PseudoJet::UserInfoBase`` 
(la documentation http://fastjet.fr/repo/fastjet-doc-3.4.0.pdf#subsection.3.1)

- ``EVENT/ReconstructedParticle.h`` lui appartient au projet ``LCIO``
(http://lcio.desy.de/v01-10/doc/doxygen_api/html/classEVENT_1_1ReconstructedParticle.html)

## Fonctionnnemnt
Ajout un attribut ``EVENT::ReconstructedParticle`` au sein de l'objet ``fastjet::PseudoJet::UserInfoBase``.

## Modification par rapport à ``ggarillot``

- setRecoParticle <- setReconstructedParticle
- recoParticle <- getReconstructedParticle

# ``EventShape.hh``

## Include 

- ``CLHEP/Vector/ThreeVector.h`` : https://www-zeuthen.desy.de/ILC/geant4/clhep-2.0.4.3/ThreeVector_8h.html
- ``Eigen/Dense`` : Matrice 

// Attention au test ligne 470, Eigen::Matrix4d initRotMatrix4d()
