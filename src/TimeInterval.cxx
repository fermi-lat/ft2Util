#include "ft2Util_2/TimeInterval.h"
#include <stdexcept>

namespace ft2Util_2 
{
  TimeInterval::TimeInterval(double start, double stop, bool good) : m_start(start), m_stop(stop), m_good(good)
  {
    if(start >=stop) 
    {
      throw std::runtime_error("TimeInterval():start is >= stop, invalid time interval!");
    }
  }
  
  //!  Get the start time of the interval
  const double& TimeInterval::start() const 
  {
    return m_start;
  }
  
  //!  Get the stop time of the interval
  const double& TimeInterval::stop() const 
  {
    return m_stop;
  }
  
  //!  Return true if the interval is set as "good" and false if the interval is set to "bad"
  const bool& TimeInterval::isGood() const 
  {
    return m_good;
  }
  
  //!  Set the interval as "good"
  void TimeInterval::setAsGood() 
  {
    m_good = true;
  }
  
  //! Set the interval as "bad"
  void TimeInterval::setAsBad() 
  {
    m_good = false;
  }
  
  void TimeInterval::setStart(double start) 
  {
    m_start = start;
  }
  
  void TimeInterval::setStop(double stop) 
  {
    m_stop = stop;
  }  
  
  bool TimeInterval::contains(double time)
  {
    if(time >= m_start && time <=m_stop) 
    {
      return true;
    } else 
    {
      return false;
    }
  }
  
}
