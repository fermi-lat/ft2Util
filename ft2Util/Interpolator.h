/** @file Interpolator.h
@brief declare class Interpolator
*/

//Avoid to include this header more than one time
#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "CLHEP/Vector/ThreeVector.h"

namespace ft2Util_2
    {
      /**
      *
      * @brief Represents the method to interpolate the position of the spacecraft
      * It currently implements the same algorithm used in GtBary. Some tests and accuracy estimates are given here:
      * https://confluence.slac.stanford.edu/display/ISOC/Position+interpolation+using+FT2+files
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
        class Interpolator
            {
                public:
                  /**
                  Constructor.
                  @param position1 The position at time t1
                  @param t1 The time when the position was position1 (in MET)
                  @param position2 The position at time t2
                  @param t2 The time when the position was position2 (in MET)
                  */
                  Interpolator(CLHEP::Hep3Vector position1, double t1, 
                                         CLHEP::Hep3Vector position2, double t2);
                  ~Interpolator();
                  /**
                  Get the position at time t, where t1 < t < t2
                  */
                  const CLHEP::Hep3Vector evaluateIn(double t);
                  
                private:
                  CLHEP::Hep3Vector m_pos1;
                  CLHEP::Hep3Vector m_pos2;
                  CLHEP::Hep3Vector m_vector12;
                  double m_t1;
                  double m_t2;
                  double m_length1, m_length2, m_length12;
            };
    }
#endif
