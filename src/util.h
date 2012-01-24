#include <sys/stat.h>

#include "tip/IFileSvc.h"
#include "tip/TipFile.h"
#include "tip/Table.h"
#include <astro/SkyDir.h>
#include <astro/SolarSystem.h>
namespace ft2Util
{

void writeQDPfile(const std::string filename,
                  std::vector<double> x, std::vector<double> y,std::vector<double> errors, double offset=0);

namespace  util
{
extern const double const_PI;
extern const double secondsPerDay;

void checkFileExists(std::string filename);

int FileExists(std::string strFilename);

bool rootFileExists(std::string strFilename);

std::string GetFileExtension(const std::string& FileName);

double Deg2Rad ( double Angle );

double Rad2Deg ( double Angle );

astro::SkyDir getSunPosition(double time);
}

}
