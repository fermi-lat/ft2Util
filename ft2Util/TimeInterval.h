/** @file TimeInterval.h
@brief declare class TimeInterval
*/

//Avoid to include this header more than one time
#ifndef TIMEINTERVAL_H
#define TIMEINTERVAL_H

namespace ft2Util_2
    {
      /**
      *
      * @brief Represents a time interval.
      * @author Giacomo Vianello <giacomov@slac.stanford.edu>
      *
      * $Header$
      */
    class TimeInterval
        {
            public:
              //!  Get the start time of the interval
                const double& start() const;
                //!  Get the stop time of the interval
                const double& stop() const;
                //!  Return true if the interval is set as "good" and false if the interval is set to "bad"
                const bool& isGood() const;
                //!  Set the interval as "good"
                void setAsGood();
                //! Set the interval as "bad"
                void setAsBad();
                void setStart(double start);
                void setStop(double stop);
                
                //! Return true if the given time is contained in the time interval, false otherwise (extremes are considered part of the interval)
                bool contains(double time);
                
                TimeInterval (double start, double stop, bool good=true);
                ~TimeInterval(){};
                
          private:
            double m_start;
            double m_stop;
            bool m_good;
        };
}

#endif
