//StatusFile.cxx

#include "ft2Util/StatusFile.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "astro/SolarSystem.h"
#include <algorithm>
#include "util.h"

ft2Util::StatusFile::StatusFile ( const std::string & filename, const std::string extname)
        : m_pointingHistory ( filename ), m_extname(extname)
{
    //constructor

    //Read from the file the columns not already readed by astro::PointingHistory
    getOtherData(filename);
}

ft2Util::StatusFile::~StatusFile()
{
    //Empty destructor;
}

void ft2Util::StatusFile::getOtherData(const std::string & filename)
{
    //Open the FT2 file data extension
    const tip::Table* table;
    table = tip::IFileSvc::instance().readTable(filename, m_extname);
    //Fill the m_latConditions map, the m_startTimes vector and the m_Quaternions map
    //We already know how much times we will store
    m_startTimes.resize(table->getNumRecords());
    //Init some variables
    double cur_start,q1,q2,q3,q4;
    tip::Table::ConstIterator itor;
    int i=0;
    for (i=0, itor = table->begin(); itor != table->end(); ++itor, i++)
    {
        cur_start = (*itor)["START"].get();
        m_startTimes[i]=cur_start;
        short int thisMode, thisConfig, thisDataQuality;
        ((*itor)["LAT_MODE"].get(thisMode));
        ((*itor)["LAT_CONFIG"].get(thisConfig));
        ((*itor)["DATA_QUAL"].get(thisDataQuality));
        ft2Util::LatCondition thisCondition(thisMode, thisConfig, thisDataQuality);
        m_latConditions.insert(std::pair<double, ft2Util::LatCondition>(cur_start,thisCondition));

        //This should be removed when the method to get the quaternion
        //will be implemented in astro::PointingInfo
        q1=(*itor)["QSJ_1"].get();
        q2=(*itor)["QSJ_2"].get();
        q3=(*itor)["QSJ_3"].get();
        q4=(*itor)["QSJ_4"].get();
        m_Quaternions[cur_start]=astro::Quaternion(CLHEP::Hep3Vector(q1,q2,q3),q4);
    }
    delete table;
}

const ft2Util::Status ft2Util::StatusFile::operator() ( double time )
{
    //Control that the requested time is inside the allowed range
    double max_time=*(std::max_element(m_startTimes.begin(),m_startTimes.end()));
    if (time > max_time )
    {
        std::ostringstream message;
        message.precision(20);
        message << "StatusFile: Time out of Range!:\n"
        << "Requested unreliable extrapolation for the time " << time
        << ", that is after the last time contained in the pointing database: ("
        << max_time  << ")"
        << std::endl;
        throw astro::PointingHistory::TimeRangeError ( message.str() );
    }
    //Get the current pointing information
    //Note that exceptions caused by a time not valid are treated
    //by the astro package. I already checked that the requested time is inside the
    //allowed range, so there is nothing I can do if the user
    //request a time not valid. Thus I don't catch the exception
    const astro::PointingInfo & current_pointing( m_pointingHistory ( time ));

    //Get the right LatCondition (the ones corresponding to the first entry
    //before the requested time)
    std::map<double,LatCondition>::const_iterator
                   cur_condition=m_latConditions.lower_bound(time);
    //Get the right Quaternion
    //This should be removed when the method to get the quaternion
    //will be implemented in the astro::PointingInfo class
    //Find the first quaternion before and after the requested time
    std::map<double, astro::Quaternion>::iterator itor=m_Quaternions.upper_bound(time);
    astro::Quaternion & highBound_quat(itor->second);
    double time2( itor->first);
    --itor;
    astro::Quaternion & lowBound_quat(itor->second);
    double time1(itor->first);
    //Interpolate between the two quaternions
    double prop=(time-time1)/(time2-time1);
    astro::Quaternion cur_quaternion=lowBound_quat.interpolate(highBound_quat, prop);

    return ft2Util::Status::Status( current_pointing.position(),
                                      cur_quaternion,
                                      current_pointing.earthCoord(),
                                      cur_condition->second,
                                      util::getSunPosition(time));
}
