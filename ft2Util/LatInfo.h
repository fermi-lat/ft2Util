/** @file LatInfo.h
@brief declare class LatInfo
*/

//Avoid to include this header more than one time
#ifndef LATINFO_H
#define LATINFO_H


#include <iostream>

#include "astro/PointingHistory.h"
#include "ft2Util/ExtendedPointingInfo.h"
#include "ft2Util/Boresight.h"
#include "ft2Util/LatCondition.h"

namespace ft2Util
    {
      /**
      *
      * @brief Represents the LAT status.
      *
      * This class represents the status of the LAT in a particular time. Notably, it contains methods to obtain the 
      attitude for the LAT, which is the spacecraft pointing corrected for the mis-alignment between the axes
      of the SC and those of the LAT.
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
    class LatInfo
        {
            public:
              /**
               Constructor
               @param spacecraft This is an ExtendedPointingInfo class repesenting the status of the spacecraft at a particular time
               @param condition This is a LatCondition class representing the condition (mode, config and data quality) of the LAT at a
               particular time
              */  
                LatInfo ( const ft2Util::ExtendedPointingInfo& spacecraft,
                                const ft2Util::LatCondition& condition);
                virtual ~LatInfo();
                /**
                *@brief Update the values of the boresight angles used for the boresight correction
                This rotation is expressed
                in terms of Tait-Bryan angles (Rx, Ry, Rz), with all angles expressed in arcsec. See also the Boresight class.
                */   
                void updateBoresightAngles(double Rx, double Ry, double Rz);
                /**
                *@brief Update the values of the boresight angles used for the boresight correction
                reading them from an XML Boresight file.
                
                You can use this method passing as input the name of a
                XML file containing the boresight angles in a tag like this:
                @verbatim
                <LATAlignment>
                <angle Rx="-170" Ry="-173" Rz="-491" />
                </LATAlignment>
                @endverbatim
                Such XML files are used inside the Data Processing Pipeline, and
                are contained in the calibration database.
                */   
                void updateBoresightAngles(const std::string& borefile);
                /**
                *@brief Returns the rotation matrix representing the boresight correction.
                */   
                CLHEP::HepRotation getBoresightMatrix();
                //return the two axis corrected for the boresight
                //! returns the LAT x Axis
                /**
                
                The LAT x Axis is the spacecraft x Axis readed from the FT2 file, corrected for the boresight
                */   
                astro::SkyDir xAxis() const;
                //! returns the LAT z Axis
                /**
                
                The LAT z Axis is the spacecraft z Axis readed from the FT2 file, corrected for the boresight
                */                   
                astro::SkyDir zAxis() const;
                //Return the LAT quaternion (different from the spacecraft one)
                //!  returns the quaternion corresponding to the LAT pointing
                
                //!The LAT quaternion corresponds to the spacecraft quaternion corrected for the boresight
                astro::Quaternion quaternion() const;
                //Accessors for the LatCondition class
                //!  LAT mode (1=capture,2=sunpoint,3=inertial pointing,4=Maneuver,5=zenithpoint/survey, or the reentry modes 6 and 7).
                short int mode() const;
                //!  LAT configuration (1 = nominal science configuration, 0 = not recommended for analysis)
                short int config() const;
                //!  LAT Data Quality (1 = good, 2 = not reviewed, 3 = good with bad parts, 0 = bad)
                short int dataQuality() const;
                
                //! Return True if the angles have been updated from the default values, or False otherwise
                static bool angles_have_been_updated() {
                  return s_angles_have_been_updated;
                }
                //! Return the boresight correction currently stored in this class
                static const Boresight & boresight() {
                  return s_boresight;
                }
                
            private:
                astro::SkyDir boresightCorrection ( CLHEP::HepRotation matrix, astro::SkyDir vector ) const;
                void setBoresight(Boresight new_boresight);
                ExtendedPointingInfo m_spacecraft;
                LatCondition m_condition;
                //The boresight rotation matrix is made static, in order
                //to be conserved between
                //different instantation of the class, to make it easy to do loops
                static bool s_angles_have_been_updated;
                //Instance a default boresight correction. This represents
                //the rotation with the default angles (see boresight.h), 
                //and could be overwritten by updateBoresightAngles.
                static Boresight s_boresight;
        };
}
#endif

