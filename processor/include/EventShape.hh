/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#ifndef EVENTSHAPE
#define EVENTSHAPE

#include <CLHEP/Vector/ThreeVector.h> // ILC
#include <Eigen/Dense> // Eigen -> matrix
#include <fastjet/PseudoJet.hh> // FastJet : jet

#include <random>

/**
 * 
 */
class EventShape {
    
    public:
        // MAKERS
        EventShape() = default;
        
        // WRECKER
        ~EventShape() = default;

        // REQUESTS
        double getThMomPower() const;
        int    getFast() const;

        CLHEP::Hep3Vector thrustAxis() const;
        CLHEP::Hep3Vector majorAxis() const;
        CLHEP::Hep3Vector minorAxis() const;

        double thrust() const;
        double majorThrust() const;
        double minorThrust() const;
        // thrust :: Corresponding thrust, major, and minor value.

        double oblateness() const;
        
        // COMMANDS
        void setPartList(const std::vector<fastjet::PseudoJet>& particles);
        void setThMomPower(double tp);
        void setFast(int nf);

    private:
    
        // ATTRIBUTES
        
        double m_dDeltaThPower = 0;
        // PARU(42): Power of momentum dependence in thrust finder.

        int m_iFast = 4;
        // MSTU(44): # of initial fastest particles choosen to start search.

        double m_dConv = 0.0001;
        // PARU(48): Convergence criteria for axis maximization.

        int m_iGood = 2;
        // MSTU(45): # different starting configurations that must
        // converge before axis is accepted as correct.

        Eigen::Matrix4d m_dAxes = {};
        // m_dAxes[1] is the Thrust axis.
        // m_dAxes[2] is the Major axis.
        // m_dAxes[3] is the Minor axis.

        std::mt19937_64                 generator = std::mt19937_64();
        std::uniform_int_distribution<> distribution = std::uniform_int_distribution<>(0, 1);

        std::array<double, 4> m_dThrust = {};
        double                m_dOblateness = 0;

        static unsigned int m_maxpart;
    
        // REQUESTS
        
        double ulAngle(double x, double y) const; // polar angle
        double sign(double a, double b) const; // sign(b) * abs(a)
        int iPow(int man, int exp); // man^{exp}
        
        // COMMAND
        
        void ludbrb(
                Eigen::MatrixXd& mom, 
                double theta, double phi, 
                double bx, double by, double bz);
};

#endif
