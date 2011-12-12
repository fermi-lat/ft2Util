//ExtendedPointingInfo.cxx


#include "ft2Util_2/ExtendedPointingInfo.h"
#include "astro/PointingInfo.h"
#include "util.h"

ft2Util_2::ExtendedPointingInfo::ExtendedPointingInfo ( const CLHEP::Hep3Vector &position,
        const astro::Quaternion &orientation,
        const astro::EarthCoordinate &earthPos )
        : m_pointingInfo ( position, orientation, earthPos ),
        m_quaternion ( orientation )
{
//   std::cout << "ExtendedPointingInfo constructor" << std::endl;
}

ft2Util_2::ExtendedPointingInfo::~ExtendedPointingInfo()
{
}

double ft2Util_2::ExtendedPointingInfo::rockingAngle() const
{
    double zenith_ra = ft2Util_2::util::Deg2Rad ( this->zenith().ra() );
    double zenith_dec = ft2Util_2::util::Deg2Rad ( this->zenith().dec() );
    double zaxis_ra = ft2Util_2::util::Deg2Rad ( this->zAxis().ra() );
    double zaxis_dec = ft2Util_2::util::Deg2Rad ( this->zAxis().dec() );

    double rocking_angle = acos ( cos ( zenith_dec ) * cos ( zaxis_dec )
                                  * cos ( zenith_ra - zaxis_ra )
                                  + sin ( zenith_dec ) * sin ( zaxis_dec ) );

    if ( zaxis_dec < zenith_dec )
        {
            rocking_angle *= -1.0;
        }

    return ft2Util_2::util::Rad2Deg ( rocking_angle );

}

const astro::Quaternion & ft2Util_2::ExtendedPointingInfo::quaternion() const
{
    return m_quaternion;
}

bool ft2Util_2::ExtendedPointingInfo::insideSAA() const
    {
        return m_pointingInfo.earthCoord().insideSAA();
    }
bool ft2Util_2::ExtendedPointingInfo::insideSAA ( double lat, double lon ) const
    {
      return m_pointingInfo.earthCoord().insideSAA ( lat, lon );
    }
double ft2Util_2::ExtendedPointingInfo::latitude() const
    {
      return m_pointingInfo.earthCoord().latitude();
    }
double ft2Util_2::ExtendedPointingInfo::longitude() const
    {
      return m_pointingInfo.earthCoord().longitude();
    }
double ft2Util_2::ExtendedPointingInfo::altitude() const
    {
      return m_pointingInfo.earthCoord().altitude();
    }
double ft2Util_2::ExtendedPointingInfo::L() const
    {
      return m_pointingInfo.earthCoord().L();
    }
double ft2Util_2::ExtendedPointingInfo::B() const
    {
      return m_pointingInfo.earthCoord().B();
    }
double ft2Util_2::ExtendedPointingInfo::lambda() const
    {
      return m_pointingInfo.earthCoord().lambda();
    }
double ft2Util_2::ExtendedPointingInfo::R() const
    {
      return m_pointingInfo.earthCoord().R();
    }
double ft2Util_2::ExtendedPointingInfo::geolat() const
    {
      return m_pointingInfo.earthCoord().geolat();
    }
double ft2Util_2::ExtendedPointingInfo::geolon() const
    {
      return m_pointingInfo.earthCoord().geolon();
    }
const CLHEP::Hep3Vector &  ft2Util_2::ExtendedPointingInfo::magnetic_field () const
{
  return m_pointingInfo.earthCoord().magnetic_field();
}
void ft2Util_2::ExtendedPointingInfo::setSAAboundary ( const std::vector<std::pair<double, double> >& boundary )
{
  m_pointingInfo.earthCoord().setSAAboundary ( boundary );
}

astro::SkyDir ft2Util_2::ExtendedPointingInfo::xAxis() const
{
  return m_pointingInfo.xAxis();
}

astro::SkyDir ft2Util_2::ExtendedPointingInfo::zAxis() const
{
  return m_pointingInfo.zAxis();
}

astro::SkyDir ft2Util_2::ExtendedPointingInfo::zenith() const
{
  return m_pointingInfo.zenith();
}

const CLHEP::Hep3Vector& ft2Util_2::ExtendedPointingInfo::position() const
{
  return m_pointingInfo.position();
}

CLHEP::HepRotation ft2Util_2::ExtendedPointingInfo::rotation() const
{
  return m_pointingInfo.rotation();
}
