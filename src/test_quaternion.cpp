#include <fstream>
#include <stdexcept>
#include <cmath>
#include <sstream>

#include "ft2Util_2/Magic7.h"
#include "ft2Util_2/Extrapolator.h"
#include "ft2Util_2/Interpolator.h"
#include "util.h"
#include "facilities/Util.h"

astro::Quaternion normalize(astro::Quaternion q){
  double w;
  double norm = sqrt(q.norm());
  CLHEP::Hep3Vector v(q.vector());
  v = v/norm;
  w=q.scalar()/norm;
  return astro::Quaternion(v,w);  
}

int main () {
  double t1 = 50.0;
  double t2 = 51.0;
  double time = 1.0;
  double fraction = (t1-time)/(t2-t1);
  //5 deg
  astro::Quaternion q_ = astro::Quaternion(CLHEP::Hep3Vector(0.9990482215818578,0.0,-0.04361938736533599),0.0);
  //45 deg rotation
  astro::Quaternion q1 = astro::Quaternion(CLHEP::Hep3Vector(0.9238795325112867,0.0,-0.3826834323650897),0.0);
  //50 deg rotation
  astro::Quaternion q2 = normalize(astro::Quaternion(CLHEP::Hep3Vector(0.9063077870366499,0.0,-0.42261826174069944),0.0));
  
  astro::Quaternion qPrime = normalize(q1.interpolate(q2, fraction));
  astro::Quaternion wrong = normalize(q1.interpolate(q2, fraction*(-1)));
  std::cerr << "Fraction: " << fraction << std::endl;
  std::cerr << "Wrong: " << wrong.rotation() << std::endl;
  astro::Quaternion qStar = q1.conjugate()*qPrime;
  astro::Quaternion result = normalize(qStar*q1);
  CLHEP::Hep3Vector v(1,3,2);
  std::cerr << "Res: " << result.rotation() << std::endl;
  std::cerr << result.rotate(v) << std::endl;
  std::cerr << "Crtl: " << q_.rotation() << std::endl; 
  std::cerr << q_.rotate(v) << std::endl;
//   
//   std::cerr << result.vector().x() << " " << result.vector().y() << " " << result.vector().z() << " " << result.scalar() << std::endl;
}
