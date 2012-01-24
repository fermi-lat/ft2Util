/** @file LatCondition.h
@brief declare class LatCondition
*/

//Avoid to include this header more than one time
#ifndef LATCONDITION_H
#define LATCONDITION_H

namespace ft2Util
    {
      /**
      *
      * @brief Represents the current LAT operational condition (mode, configuration and data quality)
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
    class LatCondition
        {
            public:
              //!  LAT mode (1=capture,2=sunpoint,3=inertial pointing,4=Maneuver,5=zenithpoint/survey, or the reentry modes 6 and 7).
                const int& mode() const {return m_mode;};
                //!  LAT configuration (1 = nominal science configuration, 0 = not recommended for analysis)
                const int& config() const {return m_config;};
                //!  LAT Data Quality (1 = good, 2 = not reviewed, 3 = good with bad parts, 0 = bad)
                const int& dataQuality() const {return m_dataQuality;};

                LatCondition (int inmode, int latconfig=1, int dataquality=2);
                virtual ~LatCondition(){};
          private:
            int m_mode;
            int m_config;
            int m_dataQuality;
        };
}

#endif
