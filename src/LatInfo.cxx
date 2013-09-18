#include "ft2Util/LatInfo.h"
#include "astro/PointingHistory.h"
#include "ft2Util/ExtendedPointingInfo.h"
#include "ft2Util/Boresight.h"
#include "ft2Util/LatCondition.h"

bool ft2Util::LatInfo::s_angles_have_been_updated=false;
//ft2Util::Boresight ft2Util::LatInfo::s_boresight;

ft2Util::LatInfo::LatInfo ( const ft2Util::ExtendedPointingInfo& spacecraft,
                                            const ft2Util::LatCondition& condition)
        : m_spacecraft(spacecraft), 
        m_condition(condition)
{
  //Initialize the boresight angles with the default values
  s_boresight = Boresight();
}


ft2Util::LatInfo::~LatInfo()
{
}

void ft2Util::LatInfo::updateBoresightAngles(double Rx, double Ry, double Rz)
{
  ft2Util::Boresight new_boresight(Rx, Ry, Rz);
  this->setBoresight(new_boresight);
}

void ft2Util::LatInfo::updateBoresightAngles(const std::string& borefile)
{
  ft2Util::Boresight new_boresight(borefile);
  this->setBoresight(new_boresight);
}

void ft2Util::LatInfo::setBoresight(ft2Util::Boresight new_boresight)
{
  s_boresight=new_boresight;
  s_angles_have_been_updated=true;
}

CLHEP::HepRotation ft2Util::LatInfo::getBoresightMatrix()
{
  return s_boresight.getRotation();
}

astro::SkyDir ft2Util::LatInfo::boresightCorrection ( CLHEP::HepRotation matrix, astro::SkyDir vector ) const
{
    //direction of the vector
    CLHEP::Hep3Vector tdir ( vector() );
    //get it in SC coordinates
    CLHEP::Hep3Vector latdir ( matrix.inverse() *tdir );
    //apply the correction, and transform back to sky
    CLHEP::Hep3Vector new_vector (matrix * s_boresight.getRotation().inverse() * latdir );
    return astro::SkyDir(new_vector);
}

astro::SkyDir ft2Util::LatInfo::xAxis() const
{
    return ft2Util::LatInfo::boresightCorrection ( m_spacecraft.rotation(), m_spacecraft.xAxis() );
}

astro::SkyDir ft2Util::LatInfo::zAxis() const
{
    return ft2Util::LatInfo::boresightCorrection ( m_spacecraft.rotation(), m_spacecraft.zAxis() );
}

astro::Quaternion ft2Util::LatInfo::quaternion()
{
  //Compose the two quaternions
  return s_boresight.getQuaternion()*m_spacecraft.quaternion();
}

short int ft2Util::LatInfo::mode() const
{
  return m_condition.mode();
}
short int ft2Util::LatInfo::config() const
{
  return m_condition.config();
}
short int ft2Util::LatInfo::dataQuality() const
{
  return m_condition.dataQuality();
}
