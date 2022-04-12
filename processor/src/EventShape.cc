/******************************************************************************/
/**                                                                          **/
/**               Team : FCC, IP2I, UCBLyon 1, France, 2022                  **/
/**                                                                          **/
/******************************************************************************/

#include "EventShape.hh"

#include <cassert>

using namespace std;

unsigned int EventShape::m_maxpart = 1000;

// COMMANDS

/**
 * Change particles list.
 * AE : particles.size() > (m_maxpart = 1000)
 */
void EventShape::setPartList(const vector<fastjet::PseudoJet>& particles) {
    // To make this look like normal physics notation the
    // zeroth element of each array, mom[i][0], will be ignored
    // and operations will be on elements 1,2,3...

    // Init momentum matrix
    Eigen::MatrixXd momentum(m_maxpart, 6); 

    double          tmax = 0.;
    double          phi = 0.;
    double          the = 0.;
    double          sgn;
    Eigen::MatrixXd fast(m_iFast + 1, 6);
    Eigen::MatrixXd work(11, 6);
    double          tdi[4] = {0., 0., 0., 0.};
    double          tds;
    double          tpr[4] = {0., 0., 0., 0.};
    double          thp;
    double          thps;

    Eigen::MatrixXd temp(3, 5);

    // Exception
    if (particles.size() > m_maxpart) {
        cerr << "ERROR : Too many particles input to EventShape" << endl;
        return;
    }

    // init momentum matrix particles
    int np = 0; // particle nb counter
    for (const auto& particle : particles) { // auto ? fastjet::PseudoJet
        momentum(np, 1) = particle.px();
        momentum(np, 2) = particle.py();
        momentum(np, 3) = particle.pz();
        momentum(np, 4) = particle.modp();

        if (abs(m_dDeltaThPower) <= 0.001) {
            momentum(np, 5) = 1.0;
        } else {
            momentum(np, 5) = pow(momentum(np, 4), m_dDeltaThPower);
        }
        tmax = tmax + momentum(np, 4) * momentum(np, 5);
        np++;
    }

    // Stop if it have less 2 particles
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
            ludbrb(momentum, 0., -phi, 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    temp(i, j) = m_dAxes(i + 1, j);
                }
                temp(i, 4) = 0.;
            }
            ludbrb(temp, 0., -phi, 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    m_dAxes(i + 1, j) = temp(i, j);
                }
            }
            
            the = ulAngle(m_dAxes(1, 3), m_dAxes(1, 1));
            ludbrb(momentum, -the, 0., 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    temp(i, j) = m_dAxes(i + 1, j);
                }
                temp(i, 4) = 0.;
            }
            
            ludbrb(temp, -the, 0., 0., 0., 0.);
            for (int i = 0; i < 3; i++) {
                for (int j = 1; j < 4; j++) {
                    m_dAxes(i + 1, j) = temp(i, j);
                }
            }
        }

        for (int ifas = 0; ifas < m_iFast + 1; ifas++) {
            fast(ifas, 4) = 0.;
        }

        // Find the m_iFast highest momentum particles and
        // put the highest in fast[0], next in fast[1],....fast[m_iFast-1].
        // fast[m_iFast] is just a workspace.
        for (int i = 0; i < np; i++) {
            if (pass == 2) {
                momentum(i, 4) = sqrt(momentum(i, 1) * momentum(i, 1) 
                                    + momentum(i, 2) * momentum(i, 2));
            }
            
            for (int ifas = m_iFast - 1; ifas > -1; ifas--) {
                if (momentum(i, 4) > fast(ifas, 4)) {
                    for (int j = 1; j < 6; j++) {
                        fast(ifas + 1, j) = fast(ifas, j);
                        if (ifas == 0) {
                            fast(ifas, j) = momentum(i, j);
                        }
                    }
                } else {
                    for (int j = 1; j < 6; j++) {
                        fast(ifas + 1, j) = momentum(i, j);
                    }
                    break;
                }
            }
        }
        // Find axis with highest thrust (case 1)/ highest major (case 2).
        for (int ie = 0; ie < work.rows(); ie++) {
            work(ie, 4) = 0.;
        }

        int p = min(m_iFast, np) - 1;
        // Don't trust Math.pow to give right answer always.
        // Want nc = 2**p.
        int nc = iPow(2, p);
        for (int n = 0; n < nc; n++) {
            for (int j = 1; j < 4; j++) {
                tdi[j] = 0.;
            }

            for (int i = 0; i < min(m_iFast, n); i++) {
                sgn = fast(i, 5);
                if (iPow(2, (i + 1)) * ((n + iPow(2, i)) / iPow(2, (i + 1))) >= i + 1) {
                    sgn = -sgn;
                }

                for (unsigned int j = 1; j < 5 - pass; j++) {
                    tdi[j] = tdi[j] + sgn * fast(i, j);
                }
            }
            
            tds = tdi[1] * tdi[1] + tdi[2] * tdi[2] + tdi[3] * tdi[3];
            for (int iw = min(n, 9); iw > -1; iw--) {
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
        m_dThrust[pass] = 0.;
        thp = -99999.;
        int nagree = 0;
        for (int iw = 0; iw < min(nc, 10) && nagree < m_iGood; iw++) {
            thp = 0.;
            thps = -99999.;
            while (thp > thps + m_dConv) {
                thps = thp;
                for (int j = 1; j < 4; j++) {
                    if (thp <= 1E-10) {
                        tdi[j] = work(iw, j);
                    } else {
                        tdi[j] = tpr[j];
                        tpr[j] = 0.;
                    }
                }
                for (int i = 0; i < np; i++) {
                    sgn = sign(momentum(i, 5), tdi[1] * momentum(i, 1) 
                                             + tdi[2] * momentum(i, 2) 
                                             + tdi[3] * momentum(i, 3));
                    for (unsigned int j = 1; j < 5 - pass; j++) {
                        tpr[j] = tpr[j] + sgn * momentum(i, j);
                    }
                }
                thp = qrt(tpr[1] * tpr[1] + tpr[2] * tpr[2] + tpr[3] * tpr[3]) / tmax;
            }
            // Save good axis. Try new initial axis until enough
            // tries agree.
            if (thp < m_dThrust[pass] - m_dConv) {
                break;
            }

            if (thp > m_dThrust[pass] + m_dConv) {
                nagree = 0;
                sgn = iPow(-1, distribution(generator));
                for (int j = 1; j < 4; j++) {
                    m_dAxes(pass, j) = sgn * tpr[j] / (tmax * thp);
                }
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

    for (int i = 0; i < np; i++) {
        thp += momentum(i, 5) * abs(
                  m_dAxes(3, 1) * momentum(i, 1) 
                + m_dAxes(3, 2) * momentum(i, 2));
    }

    m_dThrust[3] = thp / tmax;
    // Rotate back to original coordinate system.
    for (int i = 0; i < 3; i++) {
        for (int j = 1; j < 4; j++) {
            temp(i, j) = m_dAxes(i + 1, j);
        }

        temp(i, 4) = 0;
    }
    ludbrb(temp, the, phi, 0., 0., 0.);
    for (int i = 0; i < 3; i++)     {
        for (int j = 1; j < 4; j++) {
            m_dAxes(i + 1, j) = temp(i, j);
        }
    }
    m_dOblateness = m_dThrust[2] - m_dThrust[3];
}

// Setting and getting parameters.

/**
 * Set Thrust Momentum Power
 * AE: Error if sp < 0
 */
void EventShape::setThMomPower(double tp) {
    
    //assert(sp > 0.); // Error if sp not positive. ???

    if (tp > 0.) {
        m_dDeltaThPower = tp - 1.;
    }
}

double EventShape::getThMomPower() const {
    return 1.0 + m_dDeltaThPower;
}

/**
 * Set Fast
 * AE: Error if sp < 0
 */
void EventShape::setFast(int nf) {
    
    //assert(sp >= 0);// Error if sp not positive. ???
    
    if (nf > 3)
        m_iFast = nf;
}

int EventShape::getFast() const {
    
    return m_iFast;
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

// utilities(from Jetset):
double EventShape::ulAngle(double x, double y) const {
    
    constexpr double pi = 3.141592653589793;
    double ulangl = 0.;
    double r = sqrt(x * x + y * y);
    
    if (r > 1e-20) {
        if (abs(x) / r < 0.8) {
            ulangl = sign(acos(x / r), y);
        } else {
            ulangl = asin(y / r);
            if (x < 0.) {
                double ulangl_sign = (ulangl < 0.) ? -1. : +1.; 
                ulangl = ulangl_sign * pi - ulangl;
            }
        }
    }

    return ulangl;
}

double EventShape::sign(double a, double b) const {
    
    double b_sign = (b < 0.) ? -1. : +1.;
    return b_sign * abs(a);
}

void EventShape::ludbrb(
        Eigen::MatrixXd& momentum, 
        double theta, double phi, 
        double bx, double by, double bz) {
    
    // Ignore "zeroth" elements in rot,pr,dp.
    // Trying to use physics-like notation.
    Eigen::Matrix4d rot;
    double pr[4];
    double dp[5];

    unsigned int np = momentum.rows(); // auto ? int
    if (theta * theta + phi * phi > 1e-20) {
        double ct = cos(theta);
        double st = sin(theta);
        double cp = cos(phi);
        double sp = sin(phi);

        rot(1, 1) = ct * cp;
        rot(1, 2) = -sp;
        rot(1, 3) = st * cp;
        rot(2, 1) = ct * sp;
        rot(2, 2) = cp;
        rot(2, 3) = st * sp;
        rot(3, 1) = -st;
        rot(3, 2) = 0.0;
        rot(3, 3) = ct;

        for (unsigned int i = 0; i < np; i++) {
            for (int j = 1; j < 4; j++) {
                pr[j] = momentum(i, j);
                momentum(i, j) = 0.;
            }
            for (int jb = 1; jb < 4; jb++) {
                for (int k = 1; k < 4; k++)
                    momentum(i, jb) = momentum(i, jb) + rot(jb, k) * pr[k];
            }
        }
        
        double betaMax = 0.99999999;
        double beta2 = bx * bx + by * by + bz * bz;
        double beta = sqrt(beta2);
        if (beta2 > 1e-20) {
            if (beta > betaMax) {
                // send message: boost too large, resetting to <~1.0.
                bx = bx * (betaMax / beta);
                by = by * (betaMax / beta);
                bz = bz * (betaMax / beta);
                beta = betaMax;
            }
            double gamma = 1. / sqrt(1. - beta2);
            for (unsigned int i = 0; i < np; i++) {
                for (int j = 1; j < 5; j++) {
                    dp[j] = momentum(i, j);
                }

                double bp = bx * dp[1] + by * dp[2] + bz * dp[3];
                double gbp = gamma * (gamma * bp / (1.0 + gamma) + dp[4]);
                momentum(i, 1) = dp[1] + gbp * bx;
                momentum(i, 2) = dp[2] + gbp * by;
                momentum(i, 3) = dp[3] + gbp * bz;
                momentum(i, 4) = gamma * (dp[4] + bp);
            }
        }
    }
}

int EventShape::iPow(int man, int exp) {
    
    int ans = 1;
    for (int k = 0; k < exp; k++)
        ans = ans * man;

    return ans;
}
