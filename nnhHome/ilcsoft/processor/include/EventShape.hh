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

/**
 * 
 */
class EventShape {
    
    public:
    
        // ATTRIBUTE STATIQUE
        
        /**
         * Max particle number
         */
        static unsigned int m_maxpart; // = 1000;
    
        // ATTRIBUTES
        
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


        std::mt19937_64 generator = std::mt19937_64();
        
        std::uniform_int_distribution<> distribution = std::uniform_int_distribution<>(0, 1);

        std::array<double, 4> m_dThrust = {};
        
        double m_dOblateness = 0.;
        
        // MAKER
        
        EventShape() = default;
        ~EventShape() = default;
        
        // REQUESTS
        
        /**
         * \return return Hep3Vector(m_dAxes(j, i)) , i = {1, 2, 3}
         */
        CLHEP::Hep3Vector getTrustAxis() const;         /// j == 1
        CLHEP::Hep3Vector getMajorTrustAxis() const;    /// j == 2
        CLHEP::Hep3Vector getMinorTrustAxis() const;    /// j == 3

        /**
         * \return m_dThrust[j]
         */
        double getTrustValue() const;       /// j == 1
        double getMajorTrustValue() const;  /// j == 2
        double getMinorTrustValue() const;  /// j == 2

        /**
         * \return m_dOblateness
         */
        double getOblateness() const;
        
        // Setting and getting parameters.
        
        /**
         * \param tp
         * 
         * \post tp > 0. ==> m_dDeltaThPower == tp - 1.
         */
        void setThrustMomentumPower(double tp); 
        
        /**
         * \return 1.0 + m_dDeltaThPower
         */
        double getThrustMomentumPower() const; 
        
        /**
         * \param nf
         * 
         * \post nf > 3 ==> m_iFast == nf
         */
        void setFast(int nf);
        
        /**
         * \return m_iFast
         */
        int getFast() const;
        
        //  COMMANDS
        /**
         * Change particles list and attributes
         * 
         * \param particles : PseudoJet vector
         * \pre particles.size() > (m_maxpart = 1000)
         */
        void setParticleList(const std::vector<fastjet::PseudoJet>& particles);

    private:
    
        // REQUESTS 
        /**
         * Polar angle ???
         * 
         * \param x coord
         * \param y coord
         * 
         * \return
         *      if abs(x) / r < 0.8 then 
         *          sign(acos(x / r), y)            
         *      else 
         *          if x > 0 then 
         *              asin(y / r)
         *          else 
         *              sign(asin(y / r)) * pi - asin(y / r)
         */
        double ulAngle(double x, double y) const;
        
        /**
         * \param a
         * \param b
         * 
         * \return sign(b) * abs(a)
         */
        double sign(double a, double b) const;

        /**
         * \param man
         * \param exp
         * 
         * \return man^{exp}
         */
        int iPow(int man, int exp);
        
        // COMMANDS
                
        /**
         * \param momentum
         * \param theta, phi : angle
         * \param bx, by, bz : \vec{\beta}
         */
        void ludbrb(
                Eigen::MatrixXd& momentum,
                double theta, double phi,
                double bx, double by, double bz);
};

#endif
