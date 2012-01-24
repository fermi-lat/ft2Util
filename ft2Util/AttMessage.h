//Avoid to include this header more than one time
#ifndef ATTMESSAGE_H
#define ATTMESSAGE_H

#include "astro/Quaternion.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "iostream"
#include "vector"

namespace ft2Util
    {
      /** @file AttMessage.h
      * @class AttMessage
      * @brief This class represents one ATT message. 
      *
      * The ATT message is issued by the spacecraft 5 times per second, and contains 
         the information about the attitude of the satellite. It is stored in a line of the Magic 7 file, which looks like:\n
      *
      * 2008-01-01 01:56:40.160000 ATT 220845400 160000 -0.093747 -0.618179 0.668382 0.402905 0.000000 0.000000 0.000000
      *
      * where the fields are respectively: \n
      * 1- Date \n
      * 2 - Time (UTC) \n 
      * 3 - Record type (=ATT) \n 
      * 4 - Spacecraft message timestamp, in MET \n
      * 5 - Spacecraft message timestamp, microseconds of the current second \n
      * 6-9 The x, y, z, and w components of the attitude quaternion in the ECI J2000 frame \n
      * 10-12 The "rotational state" of the spacecraft, that is, the x,y,z components of the angular 
      * velocity vector of the spacecraft (rad/s), in the spacecraft reference frame \n
      *
      * @author Giacomo Vianello (giacomov@slac.stanford.edu)
      *
      * $Header$
      */
        class AttMessage
            {
                public:
//                   AttMessage();
                  /**
                  Constructor.
                  @param m7AttMessageTokens A std::vector of string containing the fields of the ATT message
                  */
                    AttMessage ( const std::vector<std::string> & m7AttMessageTokens);
                    ~AttMessage();
                    /**
                    Get the time (MET) of the message
                    */
                    const double getTime() const;
                    /**
                    Get the quaternion contained in the message
                    */
                    const astro::Quaternion& getQuaternion() const;
                    /**
                    Get the rotational velocity of the spacecraft contained in the message
                    */
                    const CLHEP::Hep3Vector& getRotVelocity() const;
                    /**
                    Print the content of the message for debugging purpose.
                    */
                    const void printMessage();

                private:
                    std::string m_date, m_timeUTC;
                    double m_MET;
                    double m_qsj_1,m_qsj_2,m_qsj_3,m_qsj_4;
                    astro::Quaternion m_quaternion;
                    double m_rotx, m_roty, m_rotz;
                    CLHEP::Hep3Vector m_rotVelocity;
            };
    }
#endif
