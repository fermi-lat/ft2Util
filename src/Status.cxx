//Status.cxx

#include "ft2Util_2/Status.h"

ft2Util_2::Status::Status ( const CLHEP::Hep3Vector &position, 
                          const astro::Quaternion &orientation, 
                          const astro::EarthCoordinate &earthPos,
                          const ft2Util_2::LatCondition &condition,
                          const astro::SkyDir &sunDir)
        : m_spacecraft ( position, orientation, earthPos), m_lat ( m_spacecraft, condition ), m_sun(sunDir)
{
//   Nothing to do here!
}

ft2Util_2::Status::~Status()
{

}
