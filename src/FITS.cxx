// c++/stl headers
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Headers from this package
#include "ft2Util/FT2_Time.hpp"


#include "facilities/Timestamp.h"
#include "facilities/Util.h"

#include "astro/EarthCoordinate.h"

#include "fitsGen/Ft2File.h"



void FT2::WriteFitsFile(FT2 &FT2) {

  
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);

  std::cout<<"Start Instance of Ft2File\n";
  std::cout<<"The file name is "<<FT2.FT2_fits_File<<"\n";
  std::cout<<"The file lenght is "<<FT2_ENTR<<"\n";
  fitsGen::Ft2File ft2(FT2.FT2_fits_File,FT2_ENTR);
  std::cout<<"Instanced\n";
  
  //!!!!!!!!!!!!!!Verify this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //ft2.header().addHistory("Input pointing history file: " + pointingFile);
  
  //std::ifstream d2(pointingFile.c_str()); 
  //std::string line;
  //std::vector<std::string> dataFields;
  std::cout<<"SC vector\n";
  std::vector<float> scPosition(3);
  std::cout<<"done\n";
  
  
  std::cout<<"Loop over fields\n";
  for(unsigned int i=0;i<FT2_ENTR;i++){
    ft2["start"].set(FT2_T.Tstart[i]);
    ft2["stop"].set(FT2.FT2_T.Tstop[i]);
    //!!!!!!!!Check the conversion from km to meters!!!!
    scPosition[0] = FT2.ORB.x[i]*1.e3;
    scPosition[1] = FT2.ORB.y[i]*1.e3;
    scPosition[2] = FT2.ORB.z[i]*1.e3;
    ft2["sc_position"].set(scPosition);
    ft2["ra_scz"].set(FT2.FT2_SC.RA_SCZ[i]);
    ft2["dec_scz"].set(FT2.FT2_SC.DEC_SCZ[i]);
    ft2["ra_scx"].set(FT2.FT2_SC.RA_SCX[i]);
    ft2["dec_scx"].set(FT2.FT2_SC.DEC_SCX[i]);
    ft2["ra_zenith"].set(FT2.FT2_SC.RA_ZENITH[i]);
    ft2["dec_zenith"].set(FT2.FT2_SC.DEC_ZENITH[i]);
    ft2["lon_geo"].set(FT2.FT2_SC.LON_GEO[i]);
    ft2["lat_geo"].set(FT2.FT2_SC.LAT_GEO[i]);
    ft2["rad_geo"].set(FT2.FT2_SC.RAD_GEO[i]);
    ft2["geomag_lat"].set(FT2.FT2_SC.GEOMAG_LAT[i]);
    ft2["b_mcilwain"].set(FT2_SC.B_MCILWAIN[i]);
    ft2["l_mcilwain"].set(FT2_SC.L_MCILWAIN[i]);
    ft2["livetime"].set(FT2_T.LiveTime[i]);
    ft2["lat_mode"].set(FT2.ORB.CM[i]);
    if (FT2.ORB.SAA[i]) {
      ft2["in_saa"].set(true);
    } else {
      ft2["in_saa"].set(false);
    }
    ft2.next();
  }
  std::cout<<"done\n";
}
