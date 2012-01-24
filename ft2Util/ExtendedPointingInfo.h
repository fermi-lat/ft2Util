/** @file ExtendedPointingInfo.h
@brief declare class ExtendedPointingInfo
*/

//Avoid to include this header more than one time
#ifndef EXTENDEDPOINTINGINFO_H
#define EXTENDEDPOINTINGINFO_H

#include "astro/PointingInfo.h"
#include "astro/SkyDir.h"

namespace ft2Util
    {
      /**
      *
      * @brief Represents the pointing information of the spacecraft.
      *
      * This is basically an interface to the astro::PointingInfo class, which allows to insert new features without
      * modifying the astro package. For example, we compute here the rocking angle, which is not provided by the astro
      * package. We also provide a method to get the quaternion representing the spacecraft attitude, which is a private member
      * in the astro::PointingInfo class. More quantities related to the pointing could be easily added here in the future.
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
        class ExtendedPointingInfo
            {
                public:
//                     ExtendedPointingInfo () {};
                    ExtendedPointingInfo ( const CLHEP::Hep3Vector& position,
                                           const astro::Quaternion& orientation,
                                           const astro::EarthCoordinate& earthPos );
                    ~ExtendedPointingInfo();
                    /**
                    *@brief Returns the current rocking angle.
                    */   
                    double rockingAngle() const;
                    /**
                    *@brief Returns the quaternion representing the current spacecraft attitude.
                    */   
                    const astro::Quaternion & quaternion() const;
                   
                     //Wrappers around the methods of the earthCoord onbject
                    //embedded in the parent class,
                    //so that it is possible to call methods with a syntax of the type:
                    //ft2Grow::ExtendedPointingInfo(..).latitude() instead of 
                    //ft2Grow::ExtendedPointingInfo(..).earthCoord.latitude()
                    //Note that the second calling syntax will still work...
                    bool insideSAA() const;
                    //!  test for SAA with explicit coordinates 
                    bool insideSAA (double lat, double lon) const;
                    //!  Ground point latitude
                    double latitude()const;
                    //!  Ground point longitude
                    double longitude()const;
                    //!  Spacecraft altitude (km)
                    double altitude()const;
                    //!  McIlwain L
                    double L()const;
                    //!  McIlwain B (gauss)
                    double B()const;
                    //!  See http://www.spenvis.oma.be/spenvis/help/background/magfield/rlambda.html for more information
                    double lambda()const;
                    //!  See http://www.spenvis.oma.be/spenvis/help/background/magfield/rlambda.html for more information
                    double R()const;
                    //!  Geomagnetic latitude
                    double geolat()const;
                    //!  Geomagnetic longitude
                    double geolon()const;
                    //!  magnetic field in zenith system 
                    const CLHEP::Hep3Vector &  magnetic_field () const;
                    //!  set the boundary from external list 
                    void setSAAboundary(const std::vector<std::pair<double,double> >& boundary);
                    //! Get the X axis of the SC
                    astro::SkyDir xAxis() const;
                    //! Get the Z axis of the SC
                    astro::SkyDir zAxis() const;
                    //! Get the Zenith vector
                    astro::SkyDir zenith() const;
                    //! Get the SC position (in meters) in the (x,y,z) Earth-centered intertial coordinates system (ECI)
                    const CLHEP::Hep3Vector& position() const;
                    //! Get the rotation which transform a vector from a generic cartesian system to the instrument coordinate system
                    CLHEP::HepRotation rotation() const;
                private:
                    astro::PointingInfo m_pointingInfo;
                    astro::Quaternion m_quaternion;
            };
    }
#endif
