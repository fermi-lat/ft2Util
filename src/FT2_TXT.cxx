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
  FT2F<<"#Abs_FT2_ID FT2Tstart FT2tstop SC.x SY.y Sc.z lat_geo lon_geo rad_geo ra_zenith dec_zenith B_mcilwain L_mcilwain geom_lat is_saa ra_scz dec_scx ra_scx dec_scx lat_mode livetime Q1 Q2 Q3 Q4\n";
  

  unsigned int START,STOP;
  FT2.Get_FT2_Entry_Index(FT2, FT2.DT.RunStart,START);
  FT2.Get_FT2_Entry_Index(FT2, FT2.DT.RunStop ,STOP);
  printf("START=%d STOP=%d\n",START,STOP);
  
  for(unsigned int i=START;i<STOP;i++){
    
    FT2F<<FT2.FT2_T.bin[i]
	<<", "
	<<std::setprecision(30)
      //<<scientific
	<<FT2.FT2_T.Tstart[i]
	<<", "
    <<std::setprecision(30)        
	<<FT2.FT2_T.Tstop[i]
	<<", "
      //<<FT2.ATT.Tstart[i]
      //<<","
      //<<FT2.ORB.Tstart[i]
      //<<","
	<<FT2.ORB.x[i]
	<<", "
	<<FT2.ORB.y[i]
	<<", "
	<<FT2.ORB.z[i]
	<<", "
	<<FT2.FT2_SC.RA_SCZ[i]
	<<", "
	<<FT2.FT2_SC.DEC_SCZ[i]
	<<", "
	<<FT2.FT2_SC.RA_SCX[i]
	<<", "
	<<FT2.FT2_SC.DEC_SCX[i]
	<<", "
	<<FT2.FT2_SC.RA_ZENITH[i]
	<<", "
	<<FT2.FT2_SC.DEC_ZENITH[i]
	<<", "
	<<FT2.FT2_SC.LON_GEO[i]
	<<", "
	<<FT2.FT2_SC.LAT_GEO[i]
	<<", "
	<<FT2.FT2_SC.RAD_GEO[i]
	<<", "
	<<FT2.FT2_SC.GEOMAG_LAT[i]
	<<", "
	<<FT2.ORB.SAA[i]
	<<", "
	<<FT2.FT2_SC.B_MCILWAIN[i]
	<<", "
	<<FT2.FT2_SC.L_MCILWAIN[i]
	<<", "
	<<FT2.ORB.CM[i]
	<<", "
	<<FT2.FT2_T.LiveTime[i]
    <<", "
    <<FT2_SC.QS_J1[i]   
    <<", "
    <<FT2_SC.QS_J2[i]   
    <<", "
    <<FT2_SC.QS_J3[i]
    <<", "
    <<FT2_SC.QS_J4[i]  
    <<","
    <<FT2.DT.ReconDeadTime[i]     
    <<","
    <<FT2.DT.gap[i]
    <<","
    <<FT2.ATT.interp_flag[i]
    <<","
    <<FT2.ORB.interp_flag[i]  
	<<std::endl;
    
  }  
  FT2F.close();
}

void FT2::WriteMergedTextFile(FT2 &FT2) {

 
  std::ofstream FT2F(FT2.FT2_txt_File.c_str());
  
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  
  //----------- WRITE FT2 TXT FILE -----------------------------------------------------
  FT2F<<"#Abs_FT2_ID FT2Tstart FT2tstop SC.x SY.y Sc.z lat_geo lon_geo rad_geo ra_zenith dec_zenith B_mcilwain L_mcilwain geom_lat is_saa ra_scz dec_scx ra_scx dec_scx lat_mode livetime Q1 Q2 Q3 Q4\n";
  

 
  
  for(unsigned int i=0;i<FT2_ENTR ;i++){
    
    FT2F<<FT2.FT2_T.bin[i]
	<<", "
	<<std::setprecision(30)
      //<<scientific
	<<FT2.FT2_T.Tstart[i]
	<<", "
    <<std::setprecision(30)       
	<<FT2.FT2_T.Tstop[i]
	<<", "
      //<<FT2.ATT.Tstart[i]
      //<<","
      //<<FT2.ORB.Tstart[i]
      //<<","
	<<FT2.ORB.x[i]
	<<", "
	<<FT2.ORB.y[i]
	<<", "
	<<FT2.ORB.z[i]
	<<", "
	<<FT2.FT2_SC.RA_SCZ[i]
	<<", "
	<<FT2.FT2_SC.DEC_SCZ[i]
	<<", "
	<<FT2.FT2_SC.RA_SCX[i]
	<<", "
	<<FT2.FT2_SC.DEC_SCX[i]
	<<", "
	<<FT2.FT2_SC.RA_ZENITH[i]
	<<", "
	<<FT2.FT2_SC.DEC_ZENITH[i]
	<<", "
	<<FT2.FT2_SC.LON_GEO[i]
	<<", "
	<<FT2.FT2_SC.LAT_GEO[i]
	<<", "
	<<FT2.FT2_SC.RAD_GEO[i]
	<<", "
	<<FT2.FT2_SC.GEOMAG_LAT[i]
	<<", "
	<< FT2.ORB.SAA[i]
	<<", "
	<<FT2.FT2_SC.B_MCILWAIN[i]
	<<", "
	<<FT2.FT2_SC.L_MCILWAIN[i]
	<<", "
	<<FT2.ORB.CM[i]
	<<", "
	<<FT2.FT2_T.LiveTime[i]
    <<", "
    <<FT2_SC.QS_J1[i]   
    <<", "
    <<FT2_SC.QS_J2[i]   
    <<", "
    <<FT2_SC.QS_J3[i]
    <<", "
    <<FT2_SC.QS_J4[i]
    <<","
    <<FT2.ATT.interp_flag[i]
    <<","
    <<FT2.ORB.interp_flag[i] 
    <<std::endl;
    
  }  
  FT2F.close();
}
