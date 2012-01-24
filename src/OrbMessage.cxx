#include "ft2Util/OrbMessage.h"
#include "facilities/Util.h"

ft2Util::OrbMessage::~OrbMessage() {}

ft2Util::OrbMessage::OrbMessage(const std::vector<std::string> &m7OrbMessageTokens)
{
  m_date = m7OrbMessageTokens[0];
  m_timeUTC = m7OrbMessageTokens[1];
  
  m_MET = facilities::Util::stringToDouble(m7OrbMessageTokens[3]) +
            facilities::Util::stringToDouble(m7OrbMessageTokens[4])/1E6;
  
  m_x = facilities::Util::stringToDouble(m7OrbMessageTokens[5]);
  m_y = facilities::Util::stringToDouble(m7OrbMessageTokens[6]);
  m_z = facilities::Util::stringToDouble(m7OrbMessageTokens[7]);
  m_position =CLHEP::Hep3Vector(m_x,m_y,m_z);
  
  m_vx = facilities::Util::stringToDouble(m7OrbMessageTokens[8]);
  m_vy = facilities::Util::stringToDouble(m7OrbMessageTokens[9]);
  m_vz = facilities::Util::stringToDouble(m7OrbMessageTokens[10]);
  m_velocity = CLHEP::Hep3Vector(m_vx,m_vy,m_vz);
  
  m_GNCmode = facilities::Util::atoi(m7OrbMessageTokens[11]);
  m_inSAA = facilities::Util::atoi(m7OrbMessageTokens[12]);

}

const void  ft2Util::OrbMessage::printMessage()
{
  std::cout.precision(20);
  std::cout << "Date: " << m_date << std::endl
                 << "UTC Time: " << m_timeUTC << std::endl
                 << "MET: " << m_MET << std::endl
                 << "Position:  " << m_position << std::endl
                 << "Translational velocity: (" << m_vx << ", " << m_vy << ", " << m_vz << ")"
                 << std::endl;
}

const double  ft2Util::OrbMessage::getTime() const
{
  return m_MET;
}

const std::string  ft2Util::OrbMessage::getDate() const
{
  return m_date;
}
const std::string  ft2Util::OrbMessage::getTimeUTC() const
{
  return m_timeUTC;
}
const CLHEP::Hep3Vector  ft2Util::OrbMessage::getPosition() const 
{
  return m_position;
}
const CLHEP::Hep3Vector  ft2Util::OrbMessage::getVelocity() const 
{
  return m_velocity;
}
const int  ft2Util::OrbMessage::getMode() const 
{
  return m_GNCmode;
}
const int  ft2Util::OrbMessage::getInSAA() const 
{
  return m_inSAA;
}
