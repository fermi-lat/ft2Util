// stl/c++ headers
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


// headers for this package
#include "ft2Util/FT2_Time.hpp"


void FT2::WriteTextFile(FT2 &FT2) {

 
  std::ofstream FT2F(FT2.FT2_txt_File.c_str());
  
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  
  //----------- WRITE FT2 TXT FILE -----------------------------------------------------
  FT2F<<"#Abs_FT2_ID FT2Tstart FT2tstop SC.x SY.y Sc.z lat_geo lon_geo rad_geo ra_zenith dec_zenith B_mcilwain L_mcilwain geom_lat is_saa ra_scz dec_scx ra_scx dec_scx lat_mode livetime\n";
  

  
  
  
  for(unsigned int i=0;i<FT2_ENTR;i++){
    
    FT2F<<FT2.FT2_T.bin[i]
	<<","
	<<std::setprecision(20)
      //<<scientific
	<<FT2.FT2_T.Tstart[i]
	<<","
	<<FT2.FT2_T.Tstop[i]
	<<","
      //<<FT2.ATT.Tstart[i]
      //<<","
      //<<FT2.ORB.Tstart[i]
      //<<","
	<<FT2.ORB.x[i]
	<<","
	<<FT2.ORB.y[i]
	<<","
	<<FT2.ORB.z[i]
	<<","
	<<FT2.FT2_SC.LAT_GEO[i]
	<<","
	<<FT2.FT2_SC.LAT_GEO[i]
	<<","
	<<FT2.FT2_SC.LAT_GEO[i]
	<<","
	<<FT2.FT2_SC.RA_ZENITH[i]
	<<","
	<<FT2.FT2_SC.DEC_ZENITH[i]
	<<","
	<<FT2.FT2_SC.B_MCILWAIN[i]
	<<","
	<<FT2.FT2_SC.L_MCILWAIN[i]
	<<","
	<<FT2.FT2_SC.GEOMAG_LAT[i]
	<<","
	<<FT2.ORB.SAA[i]
	<<","
	<<FT2.FT2_SC.RA_SCZ[i]
	<<","
	<<FT2.FT2_SC.DEC_SCZ[i]
	<<","
	<<FT2.FT2_SC.RA_SCX[i]
	<<","
	<<FT2.FT2_SC.DEC_SCX[i]
	<<","
	<<FT2.ORB.CM[i]
	<<","
	<<FT2.FT2_T.LiveTime[i]
	<<std::endl;
    
  }  
  FT2F.close();
}
