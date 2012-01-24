/** @file Magic7.h
@brief declare class Magic7
*/

//Avoid to include this header more than one time
#ifndef MAGIC7_H
#define MAGIC7_H

#include <map>

#include "astro/PointingInfo.h"
#include "ft2Util/AttMessage.h"
#include "ft2Util/OrbMessage.h"
#include "ft2Util/TimeInterval.h"

class TimeInterval;

namespace ft2Util
{
/**
*
* @brief Represents the pointing information of the spacecraft.

You should not need to instantiate this class directly when using the package
(see documentation of StatusFile and Status classes).
* @author Giacomo Vianello <giacomov@slac.stanford.edu>
*$Header$
*/
class Magic7
{
public:
    /**
    *@brief Constructor
    @param m7txtFile The input Magic 7 file
    @param tstart The start of the time interval of interest
    @param tstop The stop time of the time interval of interest
    Entries outside util::magic7ReadPadding from the time interval tstart-tstop will be ignored
    */
    Magic7 ( const std::string m7txtFile, const double tstart=-1, const double tstop=-1 );
    ~Magic7();
    void print();
    /**
    *@brief Setup the time intervals which are the natural choice for the 1s FT2 file
    ATT messages are issued at 5 Hz, while ORB message at 1 Hz. Thus, the complete information about
    the status of the satellite is exactly known once per second. The instants when a ORB and a ATT message
    are issued at the same time are then a natural choice to sample the position/attitude quantities. This method
    sets up a vector of time intervals starting from the first concurrent ATT and ORB messages and proceeds 
    with 1s steps until the end of the Magic 7 file is reached.
    */
    void setupTimeIntervals();
    /**
    *@brief Get the best estimate of the quaternion describing the attitude at a certain time
    @param time The time of interest
    If a ATT message correspond exactly to "time", the quaternion contained there is returned. Otherwise, a interpolation/
    extrapolation is performed.
    */
    const astro::Quaternion getQuaternion(double time) const;
    /**
    *@brief Get the best estimate of the position of the satellite at a certain time
    @param time The time of interest
    If a ORB message correspond exactly to "time", the position contained there is returned. Otherwise, a interpolation/
    extrapolation is performed.
    */
    const CLHEP::Hep3Vector getPosition(double time) const;
    /**
    *@brief Get the mode for the LAT at a certain time
    @param time The time of interest
    If a ORB message correspond exactly to "time", the mode contained there is returned. Otherwise, the last known mode
    (i.e., the mode contained in the closest ORB message before "time") is returned.
    */
    const int getMode(double time) const;
    /**
    *@brief Get the value of the inSAA flag at a certain time
    @param time The time of interest
    If a ORB message correspond exactly to "time", the value for the flag contained there is returned. Otherwise, the last known
    value is returned (i.e., the value contained in the closest ORB message before "time").
    */
    const int getInSAA(double time) const;
    std::vector<TimeInterval> timeIntervals() const 
    {
      return m_timeIntervals;
    };

private:
    std::string m_filename;
    std::map<double, AttMessage> m_attMessages;
    std::map<double, OrbMessage> m_orbMessages;
    astro::Quaternion linearExtrapolation(AttMessage &message1, AttMessage &message2, double &time);
    astro::Quaternion normalize(const astro::Quaternion &q) const;
    std::vector<TimeInterval> m_timeIntervals;
};
}
#endif
