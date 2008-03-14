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

//#include "st_stream/StreamFormatter.h"
//#include "st_app/AppParGroup.h"
//#include "st_app/StApp.h"
//#include "st_app/StAppFactory.h"

void FT2::WriteFitsFile(FT2 &FT2) {

  
  //using namespace fitsGen;
  unsigned int START,STOP;
  FT2.Get_FT2_Entry_Index(FT2, FT2.DT.RunStart,START);
  FT2.Get_FT2_Entry_Index(FT2, FT2.DT.RunStop ,STOP);
  printf("START=%d STOP=%d\n",START,STOP);

  //unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  unsigned long FT2_ENTR=STOP-START;


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
  
  
  for(unsigned int i=START;i<STOP;i++){
    ft2["start"].set(FT2_T.Tstart[i]);
    ft2["stop"].set(FT2.FT2_T.Tstop[i]);
    scPosition[0] = FT2.ORB.x[i];
    scPosition[1] = FT2.ORB.y[i];
    scPosition[2] = FT2.ORB.z[i];
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
    ft2["qsj_1"].set(FT2.FT2_SC.QS_J1[i]);
    ft2["qsj_2"].set(FT2.FT2_SC.QS_J2[i]);
    ft2["qsj_3"].set(FT2.FT2_SC.QS_J3[i]);
    ft2["qsj_4"].set(FT2.FT2_SC.QS_J4[i]);
    if (FT2.ORB.SAA[i]) {
      ft2["in_saa"].set(true);
    } else {
      ft2["in_saa"].set(false);
    }
    ft2.next();
  }
  
  ft2.setObsTimes(FT2_T.Tstart[START], FT2_T.Tstop[STOP]);
  std::ostringstream creator;
  creator << "ft2Util";
  ft2.setPhduKeyword("CREATOR", creator.str());
  //std::string version = m_pars["file_version"];
  //std::string version = FT2.VERSION.srt();
  ft2.setPhduKeyword("VERSION", FT2.Version);
  std::string filename(facilities::Util::basename(FT2.FT2_fits_File));
  ft2.setPhduKeyword("FILENAME", filename);
  std::cout<<"done\n";
}


void FT2::WriteFitsFileMerged(FT2 &FT2) {

  
  //using namespace fitsGen;

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
    scPosition[0] = FT2.ORB.x[i];
    scPosition[1] = FT2.ORB.y[i];
    scPosition[2] = FT2.ORB.z[i];
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
    ft2["qsj_1"].set(FT2.FT2_SC.QS_J1[i]);
    ft2["qsj_2"].set(FT2.FT2_SC.QS_J2[i]);
    ft2["qsj_3"].set(FT2.FT2_SC.QS_J3[i]);
    ft2["qsj_4"].set(FT2.FT2_SC.QS_J4[i]);
    if (FT2.ORB.SAA[i]) {
      ft2["in_saa"].set(true);
    } else {
      ft2["in_saa"].set(false);
    }
    ft2.next();
  }
  
  ft2.setObsTimes(FT2_T.Tstart[0], FT2_T.Tstop[FT2_ENTR-1]);
  std::ostringstream creator;
  creator << "ft2Util";
  ft2.setPhduKeyword("CREATOR", creator.str());
  //std::string version = m_pars["file_version"];
  //std::string version = FT2.VERSION.srt();
  ft2.setPhduKeyword("VERSION", FT2.Version);
  std::string filename(facilities::Util::basename(FT2.FT2_fits_File));
  ft2.setPhduKeyword("FILENAME", filename);
  std::cout<<"done\n";
}
