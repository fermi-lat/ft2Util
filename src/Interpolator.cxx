#include "ft2Util_2/Interpolator.h"
//This class implements the interpolation of points

ft2Util_2::Interpolator::Interpolator(CLHEP::Hep3Vector position1, double t1,
                           CLHEP::Hep3Vector position2, double t2) :
        m_pos1(position1), m_pos2(position2), m_t1(t1), m_t2(t2)
{
    CLHEP::Hep3Vector vectprod_out;
   //Save the length of the two vectors
    m_length1 = m_pos1.mag();
    m_length2 = m_pos2.mag();

    /* compute a base vector on the orbital plane (vector12) */
    vectprod_out = m_pos1.cross(m_pos2);
    m_vector12 = vectprod_out.cross(m_pos1);
    m_length12 = m_vector12.mag();
}

ft2Util_2::Interpolator::~Interpolator()
{
}

const CLHEP::Hep3Vector ft2Util_2::Interpolator::evaluateIn(double t)
{
  double fract ;
  CLHEP::Hep3Vector interpPos;
  double intlength;
  
  /* Interpolate. */
  fract = (t - m_t1) / (m_t2 - m_t1);
  intlength = m_length1 + fract * (m_length2 - m_length1);
  /* check vectors  */
  if ((m_length1 == 0.0) && (m_length2 == 0.0)) {
    /* both vectors are null */
    for (int ii = 0; ii < 3; ++ii) interpPos[ii] = 0.0;
    
  } else if (m_length1 == 0.0) {
    /* scposn1 is null, but scposn2 is not */
    for (int ii = 0; ii < 3; ++ii) {
      interpPos[ii] = m_pos2[ii] / m_length2 * intlength;
    }
    
  } else if ((m_length2 == 0.0) || (m_length12 == 0.0)) {
    /* left:  scposn2 is null, but scposn1 is not */
    /* right: either vector is not null, but they are parallel */
    for (int ii = 0; ii < 3; ++ii) {
      interpPos[ii] = m_pos1[ii] / m_length1 * intlength;
    }
    
  } else { /* both has a non-zero length, and they are not parallel */
    double inttheta, factor_cos, factor_sin;
    /* linear interpolation for orbital phase */
    inttheta = fract * acos( m_pos1.dot(m_pos2) / m_length1 / m_length2);
    factor_cos = cos(inttheta);
    factor_sin = sin(inttheta);
    for (int ii = 0; ii < 3; ++ii) {
      interpPos[ii] = intlength * (m_pos1[ii] / m_length1 * factor_cos
      + m_vector12[ii] / m_length12 * factor_sin);
    }
  }
//   std::cout.precision(14);
//   std::cout << "Interpolation summary:" << std::endl;
//   std::cout << "t = " << m_t1 << ", v = ("  << m_pos1[0] << "," << m_pos1[1] << "," << m_pos1[2] << ")" << std::endl;
//   std::cout << "t = " << t << ", v = ("  << interpPos[0] << "," << interpPos[1] << "," << interpPos[2] << ")   (<- Interpolation)" << std::endl;
//   std::cout << "t = " << m_t2 << ", v = ("  << m_pos2[0] << "," << m_pos2[1] << "," << m_pos2[2] << ")" << std::endl;
//   std::cout << std::endl;
  return interpPos;
}
