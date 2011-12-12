#include <fstream>
#include <stdexcept>
#include <cmath>
#include <sstream>

#include "ft2Util_2/Magic7.h"
#include "ft2Util_2/Extrapolator.h"
#include "ft2Util_2/Interpolator.h"
#include "ft2Util_2/TimeInterval.h"
#include "util.h"
#include "facilities/Util.h"
#include "Configuration.h"

ft2Util_2::Magic7::Magic7(const std::string m7txtFile, const double tstart, const double tstop)
        : m_filename(m7txtFile)
{
    //open the text file
    std::ifstream input_file;
    input_file.open(m7txtFile.c_str());

    if (!input_file.is_open()) {
        std::cerr << "FATAL ERROR:  Unable to open Magic 7 file :  " << m7txtFile << std::endl;
        throw std::runtime_error("ft2Util_2::Magic7(): could not open Magic 7 file");
    } else
    {
        std::cout << "Magic 7 file " << m7txtFile << " opened"<< std::endl << std::endl;
    }
//Read the Magic 7 File and fill the maps containing the messages
    int linePos=0;
//Infinite loop, will exit with break
    while (true) {
        linePos++;
        std::string line;
        std::getline(input_file, line);

        if (input_file.eof())
        {
            //End of file
            break;
        }
        std::vector<std::string> tokens;
        facilities::Util::stringTokenize(line, " ",  tokens);
        //If there are less than 3 tokens we cannot even understand if the message is ORB or ATT, so we have
        //to ignore the current line
        if (tokens.size() < 3)
        {
            //No tokens readed
            std::cerr << "WARNING: Incomplete message at line " << linePos << " in Magic 7 file " << m7txtFile << std::endl;
            std::cerr << "Line reads like this: " << line << std::endl << "Ignoring this line." << std::endl;
            continue;
        }
        double MET = facilities::Util::stringToDouble(tokens[3]) +facilities::Util::stringToDouble(tokens[4])/1E6;
        //Ignore the current message if it refers to a time more than a certain amount outside the requested time interval:
        //This is to avoid to read too much in the M7 file, but I don't cut exactly at tstart or tstop to keep extra messages
        //to make interpolation easier at the extremes of the interval.
        if ( (tstart !=-1 && tstop != -1) &&
          (tstart - MET > ft2Util_2::Configuration::Instance()->magic7ReadPadding ||
          MET - tstop > ft2Util_2::Configuration::Instance()->magic7ReadPadding)) continue;
        //ATT or ORB?
        if ( tokens[2]=="ATT" && tokens.size() == 12) {
            //ATT message
            AttMessage curMessage(tokens);
            m_attMessages.insert(std::pair<double, ft2Util_2::AttMessage>(curMessage.getTime(), curMessage));
        } else if (tokens[2]=="ORB" && tokens.size() == 13) {
            //ORB message
            OrbMessage curMessage(tokens);
            m_orbMessages.insert(std::pair<double, ft2Util_2::OrbMessage>(curMessage.getTime(), curMessage));
        } else
        {
            std::cerr << "WARNING:  Unable to parse the message at line " << linePos << " in Magic 7 file  " << m7txtFile << std::endl;
            std::cerr << "Message reads like this: " << std::endl << line << std::endl;
        }
        tokens.clear();
    }
    input_file.close();

    if (m_attMessages.empty())
    {
        std::cerr.precision(14);
        std::cerr << "FATAL ERROR:  No ATT messages found in the file  " << m7txtFile
                  << " for the required time span (" << tstart << ", " << tstop << ")." << std::endl;
        throw std::runtime_error("ft2Util_2::Magic7(): No ATT messages found in M7 file in the requested time span.");
    }
    if (m_orbMessages.empty())
    {
        std::cerr.precision(14);
        std::cerr << "FATAL ERROR:  No ORB messages found in the file  " << m7txtFile
                  << " for the required time span (" << tstart << ", " << tstop << ")." << std::endl;
        throw std::runtime_error("ft2Util_2::Magic7(): No ORB messages found in M7 file in the requested time span.");
    }
//Summarizing the Magic 7 content
    std::cout.precision(14);
    std::cout << "Magic 7 content:" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "N. of ATT messages                    :" << m_attMessages.size() << std::endl;
    std::cout << "Time of the first ATT message (MET)   :" << m_attMessages.begin()->first << std::endl;
    std::cout << "Time of the last ATT message (MET)    :" << (--m_attMessages.end())->first << std::endl;
    std::cout << "N. of ORB messages                    :" << m_orbMessages.size() << std::endl;
    std::cout << "Time of the first ORB message (MET)   :" << m_orbMessages.begin()->first << std::endl;
    std::cout << "Time of the last ORB message (MET)    :" << (--m_attMessages.end())->first << std::endl << std::endl;
    std::cout.precision(5);
//Setup the time intervals
    this->setupTimeIntervals();
}

ft2Util_2::Magic7::~Magic7() {}

void ft2Util_2::Magic7::print()
{
    //Print out the file
    std::cout << std::endl <<  "ATT Messages: " << std::endl << std::endl;
    int counter=1;
    for (std::map<double, AttMessage>::iterator itor=m_attMessages.begin();itor != m_attMessages.end(); ++itor, ++counter)
    {
        std::cout << "ATT Message # " << counter << std::endl;
        itor->second.printMessage();
    }

    std::cout << std::endl << "ORB Messages: " << std::endl << std::endl;
    counter=1;
    for (std::map<double, OrbMessage>::iterator itor=m_orbMessages.begin();itor != m_orbMessages.end(); ++itor, ++counter)
    {
        std::cout << "ORB Message # " << counter << std::endl;
        itor->second.printMessage();
    }
}

void ft2Util_2::Magic7::setupTimeIntervals()
{
    std::cerr.precision(14);
    std::cout.precision(14);
    //Find the first time where the ATT and ORB message arrived together. Most of the time it should be the time
    //of the first ORB message, but who knows...
    //Since the frequency of ORB message is 1 per second, while the frequency of ATT messages is 5 per second,
    //I loop first over ORB messages, then over ATT messages
    std::map<double, OrbMessage>::iterator firstORBitor;
    bool found=false;
    std::map<double, AttMessage>::iterator ATTitor;
    for (std::map<double, OrbMessage>::iterator ORBitor=m_orbMessages.begin();ORBitor != m_orbMessages.end(); ++ORBitor)
    {
        //Since the following loop is very fast, we don't need to assume time-ordered ATT messages
        for (ATTitor=m_attMessages.begin();ATTitor != m_attMessages.end(); ++ATTitor)
        {
          if (std::abs(ATTitor->first - ORBitor->first) < ft2Util_2::Configuration::Instance()->NULL_TIME_DIFFERENCE)
            {
                //The current ATT and ORB messages have the same time stamp
                found=true;
                firstORBitor=ORBitor;
                break;
            }
            if (ATTitor->first > ORBitor->first)
            {
                //std::map is time ordered, no need to proceed further
                break;
            }
        }
        if (found)
        {
            //if I reach here it means that there was a break before the end of the ATT loop,
            //so we have found the contemporary ATT and ORB messages
            break;
        } else {
            //No ATT message has the same time stamp of the current ORB message
            continue;
        }
    }

    if (! found)
    {
        //HEY! No contemporary ATT and ORB messages found!
        std::cerr << "FATAL ERROR: no contemporary ATT and ORB messages found in the magic 7 file " << m_filename << std::endl;
        throw std::runtime_error("ft2Util_2::Magic7(): error in Magic 7 file (no contemporary ATT and ORB messages)");
    }
    std::cout << "First concurrent ORB and ATT messages at time (MET):   " << firstORBitor->first << std::endl << std::endl;

    //Now we have the iterator to the first contemporaneous ATT and ORB messages
    //Let's assign the time intervals
    std::vector<double> intervalsStartTimes;
    
    for (std::map<double, OrbMessage>::iterator itor=firstORBitor; itor != m_orbMessages.end(); ++itor)
    {
        if (itor == firstORBitor)
        {
            intervalsStartTimes.push_back(firstORBitor->first);
            continue;
        }
        //we compute the time difference between the current ORB message
        //and the previous one
        std::map<double, OrbMessage>::iterator prevItor=itor;
        prevItor--;

        double deltaT=itor->first-prevItor->first;
        if (deltaT -1 > ft2Util_2::Configuration::Instance()->NULL_TIME_DIFFERENCE)
        {
            //There are one or more lacking ORB messages
            //How many lacking messages?
            int nLackingMessages = (int) deltaT-1;
            std::cerr << "WARNING: " << nLackingMessages << " lacking ORB message(s) between "
                      << prevItor->first << " and " << itor->first << " in magic 7 file " << m_filename << std::endl;

            //Fill up the gap generating nevertheless appropriate time intervals
            double startingTime=prevItor->first;
            for (int i=0;i<=nLackingMessages;++i)
            {
                intervalsStartTimes.push_back(startingTime+i+1);
            }
        } else if (deltaT-1 < 0)
        {
            //Two ORB messages at less than 1 seconds, check if it is a change in mode
            if (itor->second.getMode() != prevItor->second.getMode())
            {
                //Change of mode
                std::cout << "Detected a change of mode (from " << prevItor->second.getMode() << " to " << itor->second.getMode()
                          << " between " << prevItor->first << " and " << itor->first << " in magic 7 file " << m_filename << std::endl;
                intervalsStartTimes.push_back(itor->first);
            } else
            {
                //Two ORB messages at less than 1 seconds, without a change in mode: don't know what is happening here...
                std::cerr << "WARNING: " << " two ORB messages in less than one second, without a change of mode... "
                          << "very strange! It happened between "
                          << prevItor->first << " and " << itor->first << " in magic 7 file " << m_filename << std::endl;
                /*          throw std::runtime_error("ft2Util_2::setupTimeIntervals():two ORB messages in less than one second without a change of mode. ");*/
            }
        } else
        {
            //a normal situation
            intervalsStartTimes.push_back(itor->first);
        }
    }
    
    //Now fill the timeIntervals vector
    m_timeIntervals.reserve(intervalsStartTimes.size());
    for(long int intId=0;intId < (int) intervalsStartTimes.size()-1;++intId)
    {
      m_timeIntervals.push_back(TimeInterval(intervalsStartTimes[intId],intervalsStartTimes[intId+1],true));
    }
    
}

const astro::Quaternion ft2Util_2::Magic7::getQuaternion(double time) const
{
    //Lower_bound returns the element whose key is equal to "time" if exists,
    //otherwise returns the first element whose key is greater than "time" or the end
    //iterator if time is out of the time span covered by the messages
    std::map<double, AttMessage>::const_iterator attItor = m_attMessages.lower_bound(time);

    const AttMessage *message1, *message2;
    astro::Quaternion cur_quaternion;
    if (attItor->first == time)
    {
        //Found a message corresponding to the requested time
        astro::Quaternion thisQuaternion = m_attMessages.find(time)->second.getQuaternion();
        return thisQuaternion;
    } else if (attItor==m_attMessages.begin())
    {
        //The requested time is before the first message: backward extrapolation required
        //Accumulate the first two ATT messages and extrapolated backward assuming
        //a constant rotational velocity
        std::cout << "WARNING: Magic7::getQuaternion(): backward extrapolation needed" << std::endl;
        message1=&(attItor->second); //This is the first available message
        message2=&((++attItor)->second); //This is the second available message
    } else if (attItor==m_attMessages.end())
    {
        //The requested time is after the last message: a forward extrapolation is required
        //Accumulate the last two ATT messages and extrapolated forward assuming
        //a constant rotational velocity
        std::cerr << "WARNING: Magic7::getQuaternion(): forward extrapolation needed" << std::endl;
        message2=&((--attItor)->second); //This is the last available message
        message1=&((--attItor)->second); //This is the next-to-last message
    } else
    {
        //No messages corresponding to the requested time found. Interpolation required
        std::cout.precision(14);
        std::cout  << "Magic7::getQuaternion(): interpolation needed to get quaternion at time " << time << std::endl;
        message2 = &(attItor->second); //This is the first available message AFTER the requested time
        message1 = &((--attItor)->second); //This is the first available message BEFORE the requested time
    }
    //interpolate/Extrapolate to the requested time
    double time1(message1->getTime());
    double time2(message2->getTime());
    double prop=(time-time1)/(time2-time1);
    std::cerr  << "Performing SLERP between " << time1 << " and " << time2 << " to get attitude at time " << time << " (fraction: " << prop << " s )" << std::endl;
    cur_quaternion=this->normalize(message1->getQuaternion().interpolate(message2->getQuaternion(), prop));
    return cur_quaternion;
}

//Normalize a quaternion to 1: rotations must have norm = 1
astro::Quaternion ft2Util_2::Magic7::normalize(const astro::Quaternion &q) const
{
    double w;
    double norm = sqrt(q.norm());
    CLHEP::Hep3Vector v(q.vector());
    v = v/norm;
    w=q.scalar()/norm;
    return astro::Quaternion(v,w);
}

const CLHEP::Hep3Vector ft2Util_2::Magic7::getPosition(double time) const
{
    //Lower_bound returns the element whose key is equal to "time" if exists,
    //otherwise returns the first element whose key is greater than "time"
    std::map<double, OrbMessage>::const_iterator orbItor = m_orbMessages.lower_bound(time);
    if (orbItor->first == time)
    {
        //Found a message corresponding to the requested time     
        return m_orbMessages.find(time)->second.getPosition();
    } else
    {
        //No messages corresponding to the requested time found. Interpolation or extrapolation required:
        //-If the time requested is somewhere inside the interval covered by the magic 7 file.
        // interpolation is required and we will use the method already implemented in gtBary
        //- If the time requested is outside the time covered by the M7 file, we need extrapolation,
        //  and we'll use a parabolic extrapolation

        if (orbItor != m_orbMessages.begin() && orbItor != m_orbMessages.end())
        {
            //GtBary method, see https://confluence.slac.stanford.edu/display/ISOC/Position+interpolation+using+FT2+files
            //Find the point before this
            std::cerr << "WARNING: Magic7::getPosition(): interpolation needed to get position at time " << time << std::endl;
            std::map<double, OrbMessage>::const_iterator prevPosItor = orbItor;
            --prevPosItor;
            std::cerr << std::endl << std::endl << "----------------------" << std::endl;
            std::cerr << "INTERPOLATIONS NEEDED" << std::endl << std::endl;
            Interpolator posInterpolator(prevPosItor->second.getPosition(),prevPosItor->first,
                                         orbItor->second.getPosition(), orbItor->first);
            return posInterpolator.evaluateIn(time);
        }
        else
        {
            std::cerr << std::endl << std::endl << "----------------------" << std::endl;
            std::cerr <<  "EXTRAPOLATIONS NEEDED" << std::endl << std::endl;
            //  Quadratic extrapolation of the position
            //Take at least 4 points and at most 20 points for the fit
            std::map<double, CLHEP::Hep3Vector> points;
            //Collect points before the requested one until a maximum of 10 points
            for (int i=0;i < 10;++i)
            {
                if (orbItor == m_orbMessages.begin())
                {
                    //If this happens the first loop, it means that there are no messages before the time requested
                    //so we are dealing with a backward extrapolation
                    std::cerr.precision(14);
                    std::cerr << std::endl << "WARNING: Magic7::getPosition(): backward extrapolation needed to get position at time " << time << std::endl;
                    break;
                } else
                {
                    --orbItor;
                    points[orbItor->first]=(orbItor->second.getPosition());
                }
            }
//             int npointsBefore=points.size();
            //std::cout << "Collected "<< npointsBefore << " points before";
            //Reset orbItor to the initial value
            orbItor = m_orbMessages.lower_bound(time);

            //Collect points after the requested one until a maximum of 10 points
            for (int i=0;i < 10;++i)
            {
                if (orbItor == m_orbMessages.end())
                {
                    //If this happens the first loop, it means that there are no messages after the time requested
                    //so we are dealing with a forward extrapolation
                    std::cerr.precision(14);
                    std::cerr << std::endl << "WARNING: Magic7::getPosition(): forward extrapolation needed to get position at time " << time << std::endl;
                    break;
                } else
                {
                    points[orbItor->first]=(orbItor->second.getPosition());
                    ++orbItor;
                }
            }
            //Check how many points I have collected
            //std::cout << " and " << points.size()-npointsBefore << " after the requested time "
            //<< "to use for the interpolation" << std::endl;

            if (points.size() < 4)
            {
                std::cerr << "FATAL ERROR: Only " << points.size() << " points available for extrapolation (minimum = 4)"
                          << std::endl;
                std::stringstream str;
                str.precision(15);
                str << "Magic7::getPosition():: too few points available for extrapolating the position at the time "
                << time << std::endl;
                throw std::runtime_error(str.str());
            }


            //Now that we have enough points, extrapolate
            std::vector<double> times,x,y,z;
            for (std::map<double, CLHEP::Hep3Vector>::iterator itor=points.begin();itor!=points.end();++itor)
            {
                times.push_back(itor->first);
                CLHEP::Hep3Vector v=itor->second;
                x.push_back(v.x());
                y.push_back(v.y());
                z.push_back(v.z());
            }
            std::vector<double> zero(x.size(),1e3);
            Extrapolator extrapX(times,x,2);
            Extrapolator extrapY(times,y,2);
            Extrapolator extrapZ(times,z,2);

            double nx=extrapX.evaluateIn(time);
            double ny=extrapY.evaluateIn(time);
            double nz=extrapZ.evaluateIn(time);
            CLHEP::Hep3Vector cur_position(nx,ny,nz);
            return cur_position;
        }
    }
}


const int ft2Util_2::Magic7::getMode(double time) const
{
    //Lower_bound returns the element whose key is equal to "time" if exists,
    //otherwise returns the first element whose key is greater than "time"
    std::map<double, OrbMessage>::const_iterator orbItor = m_orbMessages.lower_bound(time);
    if (orbItor->first == time)
    {
        //Found a message corresponding to the requested time        
        return m_orbMessages.find(time)->second.getMode();
    } else if (orbItor == m_orbMessages.end())
    {
        //No messages with time equal or greater than the requested one. Return the last known mode
        OrbMessage messageBefore = (--orbItor)->second;

        std::cerr << std::endl << "WARNING: Magic7::getMode(): no ORB message at time " << time
                  << " . Returning the last known mode ("<< messageBefore.getMode() <<"), reported in the ORB message at time "
                  << messageBefore.getTime() << std::endl << std::endl;

        return messageBefore.getMode();
    } else if (orbItor == m_orbMessages.begin())
    {
        //No messages before the requested one. Return the first known mode
        OrbMessage firstMessage = (orbItor)->second;
        std::cerr << std::endl << "WARNING: Magic7::getMode(): no ORB message at time " << time
                  << " . Returning the first known mode ("<< firstMessage.getMode() <<"), reported in the ORB message at time "
                  << firstMessage.getTime() << std::endl << std::endl;

        return firstMessage.getMode();
    } else
    {
        //No messages corresponding to the requested time found. Returning the last known mode, that is
        //that contained in the last ORB message before the requested time
        std::cerr.precision(14);
        //Get the first existing ORB message before the requested time
        OrbMessage messageAfter = orbItor->second;
        //Get the first existing ORB message after the requested time
        OrbMessage messageBefore = (--orbItor)->second;

        std::cerr << std::endl << "WARNING: Magic7::getMode(): no ORB message at time " << time
                  << " . Returning the last known mode ("<< messageBefore.getMode() <<"), reported in the ORB message at time "
                  << messageBefore.getTime() << std::endl << std::endl;
        if (messageAfter.getMode() != messageBefore.getMode())
        {
            std::cerr << "Beware that the mode reported in the next available ORB message at time " << messageAfter.getTime()
                      << " is different, so a change in mode could be occurred anywhere in between." << std::endl << std::endl;
        }

        return messageBefore.getMode();
    }
}

const int ft2Util_2::Magic7::getInSAA(double time) const
{
    //Lower_bound returns the element whose key is equal to "time" if exists,
    //otherwise returns the first element whose key is greater than "time"
    std::map<double, OrbMessage>::const_iterator orbItor = m_orbMessages.lower_bound(time);
    if (orbItor->first == time)
    {
        //Found a message corresponding to the requested time
        return m_orbMessages.find(time)->second.getInSAA();
    } else if (orbItor == m_orbMessages.end())
    {
        //No messages with time equal or greater than the requested one. Return the last known in_SAA value
        OrbMessage messageBefore = (--orbItor)->second;

        std::cerr << "WARNING: Magic7::getInSAA(): no ORB message at time " << time
                  << " . Returning the last known value for the in_SAA flag ("<< messageBefore.getInSAA() <<"), reported in the ORB message at time "
                  << messageBefore.getTime() << std::endl << std::endl;

        return messageBefore.getInSAA();
    } else if (orbItor == m_orbMessages.begin())
    {
        //No messages before the requested one. Return the first known in_SAA value
        OrbMessage firstMessage = (orbItor)->second;
        std::cerr << "WARNING: Magic7::getInSAA():  no ORB message at time " << time
                  << " . Returning the first known value for the in_SAA flag ("<< firstMessage.getInSAA() <<"), reported in the ORB message at time "
                  << firstMessage.getTime() << std::endl << std::endl;

        return firstMessage.getInSAA();
    } else
    {
        //No messages corresponding to the requested time found. Returning the last known in_SAA value, that is
        //that contained in the last ORB message before the requested time
        std::cerr.precision(14);
        //Get the first existing ORB message before the requested time
        OrbMessage messageAfter = orbItor->second;
        //Get the first existing ORB message after the requested time
        OrbMessage messageBefore = (--orbItor)->second;

        std::cerr << std::endl << "WARNING: Magic7::getInSAA():  no ORB message at time " << time
                  << " . Returning the last known value for the in_SAA flag ("<< messageBefore.getInSAA() <<"), reported in the ORB message at time "
                  << messageBefore.getTime() << std::endl << std::endl;
        if (messageAfter.getInSAA() != messageBefore.getInSAA())
        {
            std::cerr << "Beware that the the next available ORB message (at time " << messageAfter.getTime()
                      << ") has a different value for the inSAA flag ,  ("<< messageAfter.getInSAA() <<"), so the spacecraft could be entered/exited the SAA anytime "
                      << "between the two messages." << std::endl << std::endl;
        }

        return messageBefore.getInSAA();
    }
}
