/** @file Status.h
@brief declare class Status
*/

//Avoid to include this header more than one time
#ifndef STATUS_H
#define STATUS_H

#include "ft2Util_2/ExtendedPointingInfo.h"
#include "ft2Util_2/LatInfo.h"
//#include "ft2Grow/LatCondition.h" Already included by LatInfo.h

namespace ft2Util_2
    {
      /**
      *
      * @brief This class contains the status of both the spacecraft (in the "spacecraft" member)
      * and the LAT (the "lat" member) at a given time.
      
      You should not need to instantiate this class directly. 
      Use the () operator of the StatusFile class instead.
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
    class Status
        {
            public:
//                 Status() {};
                Status(const CLHEP::Hep3Vector& position, 
                            const astro::Quaternion& orientation, 
                            const astro::EarthCoordinate& earthPos,
                            const ft2Util_2::LatCondition &condition, 
                            const astro::SkyDir &sunDir);
                virtual ~Status();
                //! Provide access to a member containing all the pointing information of the spacecraft (see ExtendedPointingInfo )               
                const ExtendedPointingInfo & spacecraft() const {
                  return m_spacecraft;
                }
                //! Provide access to a member containing the pointing information of the LAT (see LatInfo )                
                const LatInfo & lat() const {
                  return m_lat;
                }
                //! Provide access to a member containing the direction of the sun                
                const astro::SkyDir & sun() const {
                  return m_sun;
                }
                
          private:
            ExtendedPointingInfo m_spacecraft;
            LatInfo m_lat;
            astro::SkyDir m_sun;
        };
}
#endif
