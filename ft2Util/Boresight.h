/** @file Boresight.h
@brief declare class Boresight
*/

//Avoid to include this header more than one time
#ifndef BORESIGHT_H
#define BORESIGHT_H

#include <typeinfo>
#include <iostream>

//#include "fitsGen/Ft2File.h"
#include "astro/PointingHistory.h"

namespace ft2Util
    {
      /**
      *
      * @brief Represents the boresight correction.
      
      The boresight alignment represents the alignment between the spacecraft and 
      LAT coordinate systems. In the simplest terms, this can be regarded as a 
      fixed rotation in the LAT reference frame, and because the alignment angles 
      are small, the rotations about each axis commute. This rotation is expressed
      in terms of Tait-Bryan angles (Rx, Ry, Rz), with all angles expressed in arcsec.
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
    class Boresight
        {
            public:
              /**
              *@brief Default constructor, using default angles.
              
              If you use this constructor the boresight angles will be initialized to
              the default value in the configuration file
              */             
                Boresight();
                /**
                @brief Constructor with explicit angles.
                
                The boresight angles will be initialized to the given values.
                */       
                Boresight(double Rx, double Ry, double Rz);
                /**
                @brief Constructor with boresight XML file.
                
                You can use this constructor passing as input the name of a
                XML file containing the boresight angles in a tag like this:
                @verbatim
                <LATAlignment>
                <angle Rx="-170" Ry="-173" Rz="-491" />
                </LATAlignment>
                @endverbatim
                Such XML files are used inside the Data Processing Pipeline, and
                are contained in the calibration database.
                */    
                Boresight(const std::string borefile);
                virtual ~Boresight();
                /**
                *@brief Get the rotation matrix corresponding to the boresight correction.
                
                Please mind: if dir1 is a direction in LAT coordinate, to obtain the coordinates in the SC reference frame you have to
                do getRotation().inverse()*dir1.
                */  
                const CLHEP::HepRotation &getRotation() const;
                /**
                *@brief Get the quaternion corresponding to the boresight correction.
                */  
                const astro::Quaternion getQuaternion();
          private:
            CLHEP::HepRotation m_rotation;
            double m_Rx, m_Ry, m_Rz;
            int xmlParser(std::string infile, double* Rx, double* Ry, double* Rz);
            void update_rotation();

        };
}
#endif
