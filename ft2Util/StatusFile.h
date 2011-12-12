/** @file StatusFile.h
@brief declare class StatusFile
*/

//Avoid to include this header more than one time
#ifndef STATUSFILE_H
#define STATUSFILE_H

#include "ft2Util_2/Status.h"

namespace ft2Util_2
{
/**
*
* @brief Use this class to access all the information about the spacecraft pointing and position.

This class represents the core of the package. Provided an FT2 file, it gives easy
access to all the information about the position of the spacecraft, and about the pointing of
both the spacecraft and the LAT, at a given time.

Basically you have to instantiate the class givin the FT2 file name as input. Then, using the
operator (), you can obtain all the information given a time (in MET).

See the main page of the Doxygen documentation of the package for an example.
* @author Giacomo Vianello <giacomov@slac.stanford.edu>
*
* $Header$
*/
class StatusFile
{
public:

    /**
    *@brief Main constructor

    @param filename : the FT2 file name
    */
    StatusFile ( const std::string& filename, const std::string extname="SC_DATA" );
    virtual ~StatusFile();
    /**
    @brief Use this to access all the information about the position and the pointing of both the
    spacecraft and the LAT at a given time.

    @param time : the time (in MET). It should be between the first START time and the
    last STOP time contained in the FT2 file, otherwise extrapolation will be performed.
    */
    const Status operator() ( double time ) ;
    //! Return the reference to a vector containing the starting times (in MET) of the time intervals contained in the FT2 file
    const std::vector<double> &startTimes() const {
      return m_startTimes;
    }
    
private:
    void getOtherData(const std::string & filename);
    astro::PointingHistory m_pointingHistory;
    std::map<double, LatCondition> m_latConditions;
    std::string m_extname;
    std::map<double, astro::Quaternion> m_Quaternions;
    //! A vector containing the starting times (in MET) of the time intervals contained in the FT2 file
    std::vector<double> m_startTimes;
};
}
#endif
