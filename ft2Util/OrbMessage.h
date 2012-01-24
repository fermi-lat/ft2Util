/** @file OrbMessage.h
@brief declare class OrbMessage
*/

//Avoid to include this header more than one time
#ifndef ORBMESSAGE_H
#define ORBMESSAGE_H

#include "iostream"
#include "string"
#include "vector"

#include "CLHEP/Vector/ThreeVector.h"

namespace ft2Util
{
/**
*
* @brief Represents the ORB message

The ORB message is issued once per second by the spacecraft. It is downloaded from the SC and
placed in the Magic 7 file. It has this form:

2008-01-01 01:56:40.960000 ORB 220845400 960000 -4315522.500000 -5205704.000000 1508676.875000 0.000000 0.000000 0.000000 5 0

where:\n
1 - Date \n
2 - Time (UTC) \n
3 - Record type (=ORB) \n
4 - Spacecraft message timestamp, in MET \n
5 - Spacecraft message timestamp, microseconds of the current second \n
6-8 The x, y, z, coordinates of the spacecraft position in the ECI J2000 frame (meters) \n
9-11 The x,y,z components of the velocity vector of the spacecraft in the ECIJ2000 frame (m/s) \n
12 - Spacecraft GNC mode (integer) \n
(1=capture, 2=sunpoint, 3=inertial pointing,4=Maneuver,5=zenithpoint/survey; 6-7=reentry modes) \n
13 - Flag indicating whether or not the observatory is within the LAT SAA boundary 1==IN, 0==OUT \n

* @author Giacomo Vianello <giacomov@slac.stanford.edu>
*
*$Header$
*/
class OrbMessage
{
public:
  /**
  *@brief Constructor
  @param m7OrbMessageTokens A vector of string containing the fields of the ORB message
  */ 
    OrbMessage ( const std::vector<std::string> &m7OrbMessageTokens);
    ~OrbMessage();
    //! Get the time (MET) corresponding to this message
    const double getTime() const;
    //! Get the date corresponding to this message
    const std::string getDate() const;
    //! Get the time corresponding to this message in UTC format ( hh::mm::ss)
    const std::string getTimeUTC() const;
    //! Get the position of the spacecraft
    const CLHEP::Hep3Vector getPosition() const;
    //! Get the velocity vector of the spacecraft
    const CLHEP::Hep3Vector getVelocity() const;
    //! Get the spacecraft GNC mode (integer)  (1=capture, 2=sunpoint, 3=inertial pointing,4=Maneuver,5=zenithpoint/survey; 6-7=reentry modes) 
    const int getMode() const;
    //! Get the value for the inSAA flag 
    const int getInSAA() const;
    //! Print the message
    const void printMessage();

private:
    std::string m_date, m_timeUTC;
    double m_MET;
    double m_x,m_y,m_z;
    CLHEP::Hep3Vector m_position;
    double m_vx,m_vy,m_vz;
    CLHEP::Hep3Vector m_velocity;
    int m_GNCmode;
    int m_inSAA;
};
}
#endif
