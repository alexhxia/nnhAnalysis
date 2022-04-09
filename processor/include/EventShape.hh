/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#ifndef EVENTSHAPE
#define EVENTSHAPE

#include <CLHEP/Vector/ThreeVector.h>
#include <Eigen/Dense>
#include <fastjet/PseudoJet.hh>

#include <random>

class EventShape {
    
    public:
        EventShape() = default;
        ~EventShape() = default;

        void setPartList(const std::vector<fastjet::PseudoJet>& particles);

        void   setThMomPower(double tp);        // setThrustMomentumPower
        double getThMomPower() const;           // getThrustMomentumPower
        
        void   setFast(int nf);
        int    getFast() const;

        CLHEP::Hep3Vector thrustAxis() const;   // getTrustAxis
        CLHEP::Hep3Vector majorAxis() const;    // getMajorAxis
        CLHEP::Hep3Vector minorAxis() const;    // getMinorAxis 

        double thrust() const;                  // getTrust
        double majorThrust() const;             // getMajorTrust 
        double minorThrust() const;             // getMajorTrust 
        // thrust :: Corresponding thrust, major, and minor value.

        double oblateness() const;              // getOblateness

    private:
        double ulAngle(double x, double y) const;   // ~polar angle
        
        /**
         * Return : sign(b) * abs(a)
         */
        double sign(double a, double b) const;
        
        /**
         * ???
         */
        void   ludbrb(
                Eigen::MatrixXd& mom,               // momentum
                double theta, double phi,           // angle
                double bx, double by, double bz);   // coord ?

        int iPow(int man, int exp);             // man^{exp}

        /**
         * PARU(42): Power of momentum dependence in thrust finder.
         */
        double m_dDeltaThPower = 0.;
        /**
         * MSTU(44): # of initial fastest particles choosen to start search.
         */ 
        int m_iFast = 4;

        /**
         * PARU(48): Convergence criteria for axis maximization.
         */
        double m_dConv = 0.0001;

        /**
         * MSTU(45): # different starting configurations that must converge 
         * before axis is accepted as correct.
         */
        int m_iGood = 2;

        /**
         * m_dAxes[1] is the Thrust axis.
         * m_dAxes[2] is the Major axis.
         * m_dAxes[3] is the Minor axis.
         */
        Eigen::Matrix4d m_dAxes = {};


        std::mt19937_64                 generator = std::mt19937_64();
        std::uniform_int_distribution<> distribution = std::uniform_int_distribution<>(0, 1);

        std::array<double, 4> m_dThrust = {};
        double                m_dOblateness = 0.;

        /**
         * Max nb particle
         */
        static unsigned int m_maxpart = 1000;
};

#endif
