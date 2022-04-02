/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "EventShape.hh"

unsigned int EventShape::m_maxpart = 1000;

/**
 * AE : particles.size() <= m_maxpart
 */
void EventShape::setPartList(const std::vector<fastjet::PseudoJet>& particles) {
    
    // AE : particles.size() <= m_maxpart
    if (particles.size() > m_maxpart) {
        std::cout 
                << "ERROR : Too many particles input to EventShape" 
                << std::endl;
        return;
    }
    
    /*
     * To make this look like normal physics notation 
     * the zeroth element of each array, mom[i][0], will be ignored
     * and operations will be on elements 1,2,3...
     */
    Eigen::MatrixXd mom(m_maxpart, 6); // init momentum of m_maxpart ?

    double          tmax = 0.;
    double          phi = 0.;
    double          the = 0.;                   // theta ?
    double          sgn;                        // sign ?
    Eigen::MatrixXd fast(m_iFast + 1, 6);
    Eigen::MatrixXd work(11, 6);
    double          tdi[4] = {0., 0., 0., 0.};
    double          tds;
    double          tpr[4] = {0., 0., 0., 0.};
    double          thp;
    double          thps;

    Eigen::MatrixXd temp(3, 5);

    

    int np = 0; // num particles
    for (const auto& particle : particles) {
        mom(np, 1) = particle.px();
        mom(np, 2) = particle.py();
        mom(np, 3) = particle.pz();
        mom(np, 4) = particle.modp();

        if (std::abs(m_dDeltaThPower) <= 0.001) {
            mom(np, 5) = 1.0;
        } else {
            mom(np, 5) = std::pow(mom(np, 4), m_dDeltaThPower);
        }
        tmax = tmax + mom(np, 4) * mom(np, 5);
        np++;
    }

    if (np < 2) {
        m_dThrust[1] = -1.0;
        m_dOblateness = -1.0;
        return;
    }

    // for pass = 1: find thrust axis.
    // for pass = 2: find major axis.
    for (unsigned int pass = 1; pass < 3; pass++) {
        if (pass == 2) {
            phi = ulAngle(m_dAxes(1, 1), m_dAxes(1, 2));
            
            ludbrb(mom, 0, -phi, 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    temp(i, j) = m_dAxes(i + 1, j);
                }
                temp(i, 4) = 0;
            }
            
            ludbrb(temp, 0., -phi, 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    m_dAxes(i + 1, j) = temp(i, j);
                }
            }
            the = ulAngle(m_dAxes(1, 3), m_dAxes(1, 1));
            
            ludbrb(mom, -the, 0., 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    temp(i, j) = m_dAxes(i + 1, j);
                }
                temp(i, 4) = 0;
            }
            
            ludbrb(temp, -the, 0., 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    m_dAxes(i + 1, j) = temp(i, j);
                }
            }
        }

        // init all element = 0. at matrix 'fast'.
        for (int ifas = 0; ifas < m_iFast + 1; ifas++) {
            fast(ifas, 4) = 0.;
        }
        
        // Find the m_iFast highest momentum particles and
        // put the highest in fast[0], next in fast[1],....fast[m_iFast-1].
        // fast[m_iFast] is just a workspace.
        for (int i = 0; i < np; i++) {
            
            if (pass == 2) {
                mom(i, 4) = std::sqrt(
                        mom(i, 1) * mom(i, 1) + mom(i, 2) * mom(i, 2));
            }
            
            for (int ifas = m_iFast - 1; ifas > -1; ifas--) {
                if (mom(i, 4) > fast(ifas, 4)) {
                    for (int j = 1; j < 6; j++) {
                        fast(ifas + 1, j) = fast(ifas, j);
                        if (ifas == 0) {
                            fast(ifas, j) = mom(i, j);
                        }
                    }
                } else {
                    for (int j = 1; j < 6; j++) {
                        fast(ifas + 1, j) = mom(i, j);
                    }
                    break;
                }
            }
        }
        
        // Find axis with highest thrust (case 1)/ highest major (case 2).
        for (int ie = 0; ie < work.rows(); ie++) {
            work(ie, 4) = 0.;
        }

        int p = std::min(m_iFast, np) - 1;
        
        // Don't trust Math.pow to give right answer always.
        // Want nc = 2**p.
        int nc = iPow(2, p);
        for (int n = 0; n < nc; n++) {
            for (int j = 1; j < 4; j++) {
                tdi[j] = 0.;
            }
            for (int i = 0; i < std::min(m_iFast, n); i++) {
                sgn = fast(i, 5);
                if (iPow(2, (i + 1)) * ((n + iPow(2, i)) / iPow(2, (i + 1))) >= i + 1) {
                    sgn = -sgn;
                }
                for (unsigned int j = 1; j < 5 - pass; j++) {
                    tdi[j] = tdi[j] + sgn * fast(i, j);
                }
            }
            tds = tdi[1] * tdi[1] + tdi[2] * tdi[2] + tdi[3] * tdi[3];
            for (int iw = std::min(n, 9); iw > -1; iw--) {
                if (tds > work(iw, 4)) {
                    for (int j = 1; j < 5; j++) {
                        work(iw + 1, j) = work(iw, j);
                        if (iw == 0) {
                            if (j < 4) {
                                work(iw, j) = tdi[j];

                            } else {
                                work(iw, j) = tds;
                            }
                        }
                    }
                } else {
                    for (int j = 1; j < 4; j++) {
                        work(iw + 1, j) = tdi[j];
                    }
                    work(iw + 1, 4) = tds;
                }
            }
        }
        
        // Iterate direction of axis until stable maximum.
        m_dThrust[pass] = 0;
        thp = -99999.;
        int nagree = 0;
        for (int iw = 0; iw < std::min(nc, 10) && nagree < m_iGood; iw++) {
            thp = 0.;
            thps = -99999.;
            while (thp > thps + m_dConv) {
                thps = thp;
                for (int j = 1; j < 4; j++) {
                    if (thp <= 1E-10) {
                        tdi[j] = work(iw, j);
                    } else {
                        tdi[j] = tpr[j];
                        tpr[j] = 0;
                    }
                }
                for (int i = 0; i < np; i++) {
                    sgn = sign(mom(i, 5), tdi[1] * mom(i, 1) + tdi[2] * mom(i, 2) + tdi[3] * mom(i, 3));
                    for (unsigned int j = 1; j < 5 - pass; j++)
                        tpr[j] = tpr[j] + sgn * mom(i, j);
                }
                thp = std::sqrt(tpr[1] * tpr[1] + tpr[2] * tpr[2] + tpr[3] * tpr[3]) / tmax;
            }
            
            // Save good axis. Try new initial axis until enough tries agree.
            if (thp < m_dThrust[pass] - m_dConv) {
                break;
            }
            if (thp > m_dThrust[pass] + m_dConv) {
                nagree = 0;
                sgn = iPow(-1, distribution(generator));
                for (int j = 1; j < 4; j++)
                    m_dAxes(pass, j) = sgn * tpr[j] / (tmax * thp);

                m_dThrust[pass] = thp;
            }
            nagree = nagree + 1;
        }
    }
    // Find minor axis and value by orthogonality.
    sgn = iPow(-1, distribution(generator));
    m_dAxes(3, 1) = -sgn * m_dAxes(2, 2);
    m_dAxes(3, 2) = sgn * m_dAxes(2, 1);
    m_dAxes(3, 3) = 0.;
    thp = 0.;

    for (int i = 0; i < np; i++)
        thp += mom(i, 5) * std::abs(m_dAxes(3, 1) * mom(i, 1) + m_dAxes(3, 2) * mom(i, 2));

    m_dThrust[3] = thp / tmax;
    // Rotate back to original coordinate system.
    for (int i6 = 0; i6 < 3; i6++)
    {
        for (int j = 1; j < 4; j++)
            temp(i6, j) = m_dAxes(i6 + 1, j);

        temp(i6, 4) = 0;
    }
    ludbrb(temp, the, phi, 0., 0., 0.);
    for (int i7 = 0; i7 < 3; i7++)
    {
        for (int j = 1; j < 4; j++)
            m_dAxes(i7 + 1, j) = temp(i7, j);
    }
    m_dOblateness = m_dThrust[2] - m_dThrust[3];
}

// Returning results

CLHEP::Hep3Vector EventShape::thrustAxis() const {
    return CLHEP::Hep3Vector(m_dAxes(1, 1), m_dAxes(1, 2), m_dAxes(1, 3));
}

CLHEP::Hep3Vector EventShape::majorAxis() const {
    return CLHEP::Hep3Vector(m_dAxes(2, 1), m_dAxes(2, 2), m_dAxes(2, 3));
}

CLHEP::Hep3Vector EventShape::minorAxis() const {
    return CLHEP::Hep3Vector(m_dAxes(3, 1), m_dAxes(3, 2), m_dAxes(3, 3));
}

double EventShape::thrust() const {
    return m_dThrust[1];
}

double EventShape::majorThrust() const {
    return m_dThrust[2];
}

double EventShape::minorThrust() const {
    return m_dThrust[3];
}

double EventShape::oblateness() const {
    return m_dOblateness;
}

// Getting parameters.

double EventShape::getThMomPower() const {
    return 1.0 + m_dDeltaThPower;
}

int EventShape::getFast() const {
    return m_iFast;
}

// COMMANDS 

// Setting parameters.

/**
 * Do nothing if (nf <= 3)
 */
// AE : nf > 3 => WHY ?
void EventShape::setFast(int nf) {
    
    // Error if sp not positive.
    if (nf > 3)
        m_iFast = nf;
}

/**
 * Do nothing if (tp <= 0)
 * tp : thrust_power ?
 */
// AE : tp > 0. => WHY ?
void EventShape::setThMomPower(double tp) {
    
    // Error if sp not positive.
    if (tp > 0.)
        m_dDeltaThPower = tp - 1.0;
}

/* TOOLS */

// REQUESTS

/**
 * Return polar angle. 
 * r = std::sqrt(x * x + y * y)
 * Return :
 *      IF (r < 1e-20) THEN 
 *          0                                          
 *      ELSE 
 *          IF (|x| / r < 0.8) THEN 
 *              EventShape::sign(acos(x / r), y)
 *          ELSE 
 *              IF (x >= 0) THEN
 *                  asin(y / r)
 *              ELSE 
 *                  sign(asin(y / r)) * pi - asin(y / r) 
 *      
 */
// utilities(from Jetset):
double EventShape::ulAngle(double x, double y) const {
    
    constexpr double pi = 3.141592653589793;
    
    double ulangl = 0.;
    double r = std::sqrt(x * x + y * y);
    
    if (r > 1e-20) {
        if (std::abs(x) / r < 0.8) {
            ulangl = sign(std::acos(x / r), y);
        } else {
            ulangl = std::asin(y / r);
            
            if (x < 0.) {                
                if (ulangl >= 0.) {
                    ulangl = pi - ulangl;
                } else {
                    ulangl = -1 * pi - ulangl;
                }
            }
        } 
    }
    
    return ulangl;
}

/**
 * Return : sign(b) * abs(a)
 */
double EventShape::sign(double a, double b) const {
    
    int b_sign = (b < 0.) ? -1. : +1;
    
    return b_sign * std::abs(a);
}

/**
 * Calculate pow function for integer.
 */
int EventShape::iPow(int man, int exp) {
    
    int ans = 1;
    for (int k = 0; k < exp; k++) {
        ans = ans * man;
    }
    
    return ans;
}

// COMMANDE

/**
 * 
 */
void EventShape::ludbrb(
        Eigen::MatrixXd& mom, // momentum ?
        double theta, double phi, 
        double bx, double by, double bz) {


    /* Ignore "zeroth" elements in rot, pr, dp. *
     * Trying to use physics-like notation.     */
    
    // rotational ?
    Eigen::Matrix4d rot; 
    int np = mom.rows(); // nb of lign
    
    // ?
    int const size_pr(4);
    double pr[size_pr];
    
    // ?
    int const size_dp(5);
    double dp[s_dp];

    if (theta * theta + phi * phi > 1e-20) {
        
        rot = initRotMatrix4d();
        
        //
        for (unsigned int i = 0; i < np; i++) {
            for (int j = 1; j < size_pr; j++) {
                pr[j] = mom(i, j);
                mom(i, j) = 0.;

                for (int k = 1; k < size_pr; k++) {
                    mom(i, j) = mom(i, j) + rot(j, k) * pr[k];
                }
            }
        }
        
        // 
        double beta2 = bx * bx + by * by + bz * bz;
        double beta = std::sqrt(beta2);
        double beta_max = 0.99999999;
        
        if (beta2 > 1e-20) {
            if (beta > beta_max) {
                // send message: boost too large, resetting to <~1.0.
                bx = bx * (beta_max / beta);
                by = by * (beta_max / beta);
                bz = bz * (beta_max / beta);
                beta = beta_max;
            }
            double gamma = 1.0 / std::sqrt(1.0 - beta2);
            for (unsigned int i = 0; i < np; i++) {
                for (int j = 1; j < size_dp; j++) {
                    dp[j] = mom(i, j);
                }

                double bp = bx * dp[1] + by * dp[2] + bz * dp[3];
                double gbp = gamma * (gamma * bp / (1.0 + gamma) + dp[4]);
                mom(i, 1) = dp[1] + gbp * bx;
                mom(i, 2) = dp[2] + gbp * by;
                mom(i, 3) = dp[3] + gbp * bz;
                mom(i, 4) = gamma * (dp[4] + bp);
            }
        }
    }
}

/*
 * Ignore "zeroth" elements in rot, pr, dp.
 * Trying to use physics-like notation.
 * 
 * Return matrix (1:3)
 * [    cos(theta)cos(phi)      -sin(phi)       sin(theta)cos(phi)      ]
 * [    cos(theta)sin(phi)       cos(phi)       sin(theta)sin(phi)      ]
 * [   -sin(theta)                  0           cos(theta)              ]
 */
Eigen::Matrix4d initRotMatrix4d() {
    
    Eigen::Matrix4d rot;
    
    // init 0. no using case => use physics-like notation.
    rot(0, 0) = 0.;
    for (int i = 1; i < 4; i++) {
        rot(i, 0) = 0.;
        rot(0, i) = 0.;
    }
    
    // init other
    double ct = std::cos(theta);
    double st = std::sin(theta);
    double cp = std::cos(phi);
    double sp = std::sin(phi);

    rot(1, 1) = ct * cp;
    rot(1, 2) = -sp;
    rot(1, 3) = st * cp;
    rot(2, 1) = ct * sp;
    rot(2, 2) = cp;
    rot(2, 3) = st * sp;
    rot(3, 1) = -st;
    rot(3, 2) = 0.;
    rot(3, 3) = ct;
    
    return &rot;
}
