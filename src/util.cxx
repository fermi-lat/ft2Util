#include <algorithm>
#include<fstream>
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include "util.h"
#include <fstream>
#include <facilities/Util.h>
#include "Configuration.h"

#include <TFile.h>

namespace ft2Util_2
{


void writeQDPfile(const std::string filename,
                  std::vector<double> x, std::vector<double> y,std::vector<double> errors, double offset)
{
    std::ofstream outfile;
    outfile.open(filename.c_str());
    outfile.precision(20);
    //QDP file header
    outfile << "READ SERR 2" << std::endl;
    outfile << "LINE STEP" << std::endl;
    if (outfile != 0)
    {
        outfile << "! offset= " << offset << std::endl;
        //Apply the given offset on the x axis.
        std::transform(x.begin(), x.end(), x.begin(),
                       std::bind2nd(std::minus<double>(), offset));
    }

    for (int i=0;i < (int) x.size(); ++i)
    {
        outfile << x[i] << " " << y[i] << " "
        << errors[i] << std::endl;
    }
    outfile.close();
}

namespace  util
{
const double const_PI=3.14159265358979323846264338327950288;
const double secondsPerDay = 86400.0;

void checkFileExists(std::string filename)
{
  bool fileExists=true;
  //Use the rootFileExists function if filename is a root file
  std::string extension = ft2Util_2::util::GetFileExtension(filename);
  if(extension=="root") 
  {
    fileExists = ft2Util_2::util::rootFileExists(filename);
  } else
  {
    int ret = ft2Util_2::util::FileExists(filename);
    if(ret!=1) fileExists = false;
  }
  
  if (! fileExists)
  {
    std::stringstream str;
    str << "Fatal error: file " << filename << " does not exist or is not readable.";
    throw std::runtime_error(str.str());
  }
}

//This return -1 if the file/directory does not exist or is not accessible,
// 0 if the file is actually a directory
// 1 if the file exist and is readable
int FileExists(std::string strFilename) {
    struct stat stFileInfo;
    int intStat;

    // Attempt to get the file attributes
    intStat = stat(strFilename.c_str(),&stFileInfo);
    if (intStat == 0) {
        // We were able to get the file attributes
        // so the file obviously exists.
        if(S_ISREG (stFileInfo.st_mode)) return 1;
        if(S_ISDIR (stFileInfo.st_mode)) return 0;
    } else {
        // We were not able to get the file attributes.
        //  File does not exist or is not accessible
        return -1;
    }
    //Should never reach here
    return -1;
}

//This is needed for big files, since the stat function used in FileExists cannot handle files
//bigger than a few Gb
bool rootFileExists(std::string strFilename){
  TFile f(strFilename.c_str(),"READ");
  if(f.IsZombie()) { return false; } else { return true; }
  f.Close();
}

std::string GetFileExtension(const std::string& FileName)
{
  if(FileName.find_last_of(".") != std::string::npos)
    return FileName.substr(FileName.find_last_of(".")+1);
  return "";
}

double Deg2Rad ( double Angle )
{
    return double( Angle * const_PI / 180.0 );
}

double Rad2Deg ( double Angle )
{
    return double( Angle * 180.0/const_PI );
}
astro::SkyDir getSunPosition(double time)
{
    //Get the position of the sun
    double jd = astro::JulianDate((ft2Util_2::Configuration::Instance()->JulianDateMissionStart + time)/util::secondsPerDay);
    astro::SolarSystem sunAstro(astro::SolarSystem::SUN);
    return sunAstro.direction(jd);
}

}

}
