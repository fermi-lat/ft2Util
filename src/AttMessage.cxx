#include "ft2Util_2/AttMessage.h"
#include "facilities/Util.h"


ft2Util_2::AttMessage::~AttMessage() {}

ft2Util_2::AttMessage::AttMessage(const std::vector<std::string>& m7AttMessageTokens)
{
  m_date = m7AttMessageTokens[0];
  m_timeUTC = m7AttMessageTokens[1];
  
  m_MET = facilities::Util::stringToDouble(m7AttMessageTokens[3]) +
            facilities::Util::stringToDouble(m7AttMessageTokens[4])/1E6;
  
  m_qsj_1 = facilities::Util::stringToDouble(m7AttMessageTokens[5]);
  m_qsj_2 = facilities::Util::stringToDouble(m7AttMessageTokens[6]);
  m_qsj_3 = facilities::Util::stringToDouble(m7AttMessageTokens[7]);
  m_qsj_4 = facilities::Util::stringToDouble(m7AttMessageTokens[8]);
  m_quaternion = astro::Quaternion(CLHEP::Hep3Vector(m_qsj_1,m_qsj_2,m_qsj_3),m_qsj_4);
  
  m_rotx = facilities::Util::stringToDouble(m7AttMessageTokens[9]);
  m_roty = facilities::Util::stringToDouble(m7AttMessageTokens[10]);
  m_rotz =facilities::Util::stringToDouble( m7AttMessageTokens[11]);
  m_rotVelocity = CLHEP::Hep3Vector(m_rotx,m_roty,m_rotz);
}

const astro::Quaternion& ft2Util_2::AttMessage::getQuaternion() const
{
  return m_quaternion;
}

const CLHEP::Hep3Vector& ft2Util_2::AttMessage::getRotVelocity() const
{
  return m_rotVelocity;
}

const void ft2Util_2::AttMessage::printMessage()
{
  std::cout << "Date: " << m_date << std::endl
                 << "UTC Time: " << m_timeUTC << std::endl
                 << "MET: " << m_MET << std::endl
                 << "Quaternion:  (" << m_qsj_1 << ", " << m_qsj_2 << ", " << m_qsj_3 << ") , " << m_qsj_4 << std::endl
                 << "Rotational velocity: (" << m_rotx << ", " << m_roty << ", " << m_rotz << ")"
                 << std::endl;
}

const double ft2Util_2::AttMessage::getTime() const
{
  return m_MET;
}
