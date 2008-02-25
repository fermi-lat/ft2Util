
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

// Headers from astrp package
#include "astro/Quaternion.h"
#include "astro/SkyDir.h" 
#include "astro/EarthCoordinate.h"
#include "astro/GPS.h"
#include "astro/JulianDate.h"


//Constructor
FT2_SpaceCraft::FT2_SpaceCraft()
{

}

void FT2_SpaceCraft::Set_FT2SC_Size(FT2_SpaceCraft &SC, unsigned int size)
{
  SC.RA_SCX.resize(size);
  SC.RA_SCZ.resize(size);
  SC.DEC_SCX.resize(size);
  SC.DEC_SCZ.resize(size);
  SC.RA_ZENITH.resize(size);
  SC.DEC_ZENITH.resize(size);
  SC.LAT_GEO.resize(size);
  SC.LON_GEO.resize(size);
  SC.RAD_GEO.resize(size);
  SC.B_MCILWAIN.resize(size);
  SC.L_MCILWAIN.resize(size);
  SC.GEOMAG_LAT.resize(size);
}











//Filling the SC entries
void FT2::Fill_SC_Entries(FT2 &FT2){
  
  using namespace astro;
  //using astro::Quaternion;
  using CLHEP::Hep3Vector;


  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //FT2 template requires SC parameters at the beginning of the FT2 interval
  //The first ATT and ORB entry do not come at the same time and more-over it may happens 
  //that during the fligth ATT and ORB time-stamp are misaligned
  //We have to understand if one secondo is neglible as approximation, or we have
  //to make some interpolations
  
  unsigned long FT2_Entries=Get_FT2_Entries(FT2);
  FT2.FT2_SC.Set_FT2SC_Size(FT2.FT2_SC,FT2_Entries);

  std::cout<<"----------------- Fill SC entries --------------------"<<std::endl;
 
  for (unsigned int i = 0; i < FT2.FT2_SC.RA_SCX.size(); ++i){   
    
     //printf(" 1 entry=%d Size=%d\n",i,FT2.FT2_SC.RA_SCX.size()); 
      
    //RA and DEC of SC X and Z axis
    static Hep3Vector xaxis(1,0,0), zaxis(0,0,1);
    Quaternion q(Hep3Vector(FT2.ATT.x[i],FT2.ATT.y[i],FT2.ATT.z[i]),FT2.ATT.w[i]);
    SkyDir scxaxis(q.rotate(xaxis)), sczaxis(q.rotate(zaxis));
    double rax(scxaxis.ra()), decx(scxaxis.dec()),  raz(sczaxis.ra()), decz(sczaxis.dec());
 
    FT2.FT2_SC.RA_SCX[i]=rax;
    FT2.FT2_SC.RA_SCZ[i]=raz;
    FT2.FT2_SC.DEC_SCX[i]=decx;
    FT2.FT2_SC.DEC_SCZ[i]=decz;

    
    //Position Vector
    Hep3Vector pos(FT2.ORB.x[i],FT2.ORB.y[i],FT2.ORB.z[i]);
    Hep3Vector pos_km =  pos/1e3;

    //SKY DIR RA and DEC of ZENITH
    SkyDir zenith(pos);

    FT2.FT2_SC.RA_ZENITH[i]=(zenith.ra());       
    FT2.FT2_SC.DEC_ZENITH[i]=(zenith.dec());

    //LAT LON and RAD  GEO
    //!!!!!!!!!!! Here we have
    //The time of the ORBIT stamp
    EarthCoordinate EC(pos_km,FT2_T.Tstart[i]);
 
    FT2.FT2_SC.LAT_GEO[i]=EC.latitude();
    FT2.FT2_SC.LON_GEO[i]=EC.longitude();
    //rad_geo in meters!!!
    FT2.FT2_SC.RAD_GEO[i]=EC.altitude()*1000;
    
    //MCILWAIN coordinates
    FT2.FT2_SC.B_MCILWAIN[i]=EC.B();
    FT2.FT2_SC.L_MCILWAIN[i]=EC.L();
 
    //Geomag LAT
    FT2.FT2_SC.GEOMAG_LAT[i]=EC.geolat();

    //EarthCoordinate GEOM(EC.latitude(),EC.longitude());
    //FT2.FT2_SC.GEOMAG_LAT[i]=GEOM.geolat();
    
    //FT2.FT2_SC.B_MCILWAIN[i]=GEOM.B();
    //FT2.FT2_SC.L_MCILWAIN[i]=GEOM.L();


    //printf("lat %e lon %e geolat %e\n",EC.latitude(),EC.longitude (),GEOM.geolat());
    
    if(FT2.verbose){
      std::cout<<i
	       <<", "
	       <<rax
	       <<", "
	       <<raz
	       <<", "
	       <<decx
	       <<", "
	       <<decz
	       <<", "	
	       <<FT2.FT2_SC.RA_ZENITH[i]
	       <<", "
	       <<FT2.FT2_SC.DEC_ZENITH[i]
	       <<", "
	       <<FT2.FT2_SC.LAT_GEO[i]
	       <<", "
	       <<FT2.FT2_SC.LON_GEO[i]
	       <<", "
	       <<FT2.FT2_SC.RAD_GEO[i]
	       <<", "
	       <<FT2.FT2_SC.B_MCILWAIN[i]
	       <<", "
	       <<FT2.FT2_SC.L_MCILWAIN[i]
	       <<", "
	       <<FT2.FT2_SC.GEOMAG_LAT[i]
	       <<std::endl;
    }
  }
}
