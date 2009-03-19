
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

// Headers from astro package
#include "astro/Quaternion.h"
#include "astro/SkyDir.h"
#include "astro/EarthCoordinate.h"
#include "astro/GPS.h"
#include "astro/JulianDate.h"


//Constructor
FT2_SpaceCraft::FT2_SpaceCraft() {
    
}

void FT2_SpaceCraft::Set_FT2SC_Size(FT2_SpaceCraft &SC, unsigned int size) {
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
    SC.ORBITAL_POLE_RA.resize(size);
    SC.ORBITAL_POLE_DEC.resize(size);
    SC.ROCKING_ANGLE.resize(size);
    SC.QS_J1.resize(size);
    SC.QS_J2.resize(size);
    SC.QS_J3.resize(size);
    SC.QS_J4.resize(size);
}











//Filling the SC entries
void FT2::Fill_SC_Entries(FT2 &FT2){
    
    using namespace astro;
    //using astro::Quaternion;
    using CLHEP::Hep3Vector;
    
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //FT2 template requires SC parameters at the beginning of the FT2 interval
    //The first ATT and ORB entry do not come at the same time and more-over it may happens
    //that during the fligth ATT and ORB time-stamp are misaligned
    //We have to understand if one secondo is neglible as approximation, or we have
    //to make some interpolations
    
    unsigned long FT2_Entries=Get_FT2_Entries(FT2);
    FT2.FT2_SC.Set_FT2SC_Size(FT2.FT2_SC, FT2_Entries);
    
    std::cout<<"----------------- Fill SC entries --------------------"<<std::endl;
    
    for (unsigned int i = 0; i < FT2.FT2_SC.RA_SCX.size(); ++i){
        
        //printf(" 1 entry=%d Size=%d\n",i,FT2.FT2_SC.RA_SCX.size());
        
        //RA and DEC of SC X and Z axis
        static Hep3Vector xaxis(1, 0, 0), zaxis(0, 0, 1);
        Quaternion q(Hep3Vector(FT2.ATT.x[i], FT2.ATT.y[i], FT2.ATT.z[i]), FT2.ATT.w[i]);
        SkyDir scxaxis(q.rotate(xaxis)), sczaxis(q.rotate(zaxis));
        double rax(scxaxis.ra()), decx(scxaxis.dec()),  raz(sczaxis.ra()), decz(sczaxis.dec());
        
        FT2.FT2_SC.RA_SCX[i]=rax;
        FT2.FT2_SC.RA_SCZ[i]=raz;
        FT2.FT2_SC.DEC_SCX[i]=decx;
        FT2.FT2_SC.DEC_SCZ[i]=decz;
        
        
        //Position Vector
        Hep3Vector pos(FT2.ORB.x[i], FT2.ORB.y[i], FT2.ORB.z[i]);
        Hep3Vector pos_km =  pos/1e3;
        
        //SKY DIR RA and DEC of ZENITH
        SkyDir zenith(pos);
        
        FT2.FT2_SC.RA_ZENITH[i]=(zenith.ra());
        FT2.FT2_SC.DEC_ZENITH[i]=(zenith.dec());
        
        //LAT LON and RAD  GEO
        // !!!!!!!!!!! Here we have
        //The time of the ORBIT stamp
        EarthCoordinate EC(pos_km, FT2_T.Tstart[i]);
        
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
        
        //Quaternions
        FT2.FT2_SC.QS_J1[i]=FT2.ATT.x[i];
        FT2.FT2_SC.QS_J2[i]=FT2.ATT.y[i];
        FT2.FT2_SC.QS_J3[i]=FT2.ATT.z[i];
        FT2.FT2_SC.QS_J4[i]=FT2.ATT.w[i];
        
        //Rocking_angle
        FT2.FT2_SC.EvalRockingAngle(FT2_SC, i );
        
        //Orbital Pole
        if(i>0){
            Hep3Vector pos1(FT2.ORB.x[i-1], FT2.ORB.y[i-1], FT2.ORB.z[i-1]);
            Hep3Vector pos2(FT2.ORB.x[i], FT2.ORB.y[i], FT2.ORB.z[i]);
            FT2.FT2_SC.EvalOrbitalPole( pos1, pos2, FT2_SC, i);
        }
        
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

void FT2_SpaceCraft::EvalRockingAngle(FT2_SpaceCraft &SC, unsigned int i){
    
    double ra1=SC.RA_ZENITH[i];
    double dec1=SC.DEC_ZENITH[i];
    double ra2= SC.RA_SCZ[i];
    double dec2= SC.DEC_SCZ[i];
    
    ra1=Deg2Rad(ra1);
    dec1=Deg2Rad(dec1);
    ra2=Deg2Rad(ra2);
    dec2=Deg2Rad(dec2);
    
    double rocking_angle = acos(cos(dec1)*cos(dec2)*cos(ra1-ra2) + sin(dec1)*sin(dec2));
    
    if (dec2 < dec1) {rocking_angle*=-1.0; }
    
    SC.ROCKING_ANGLE[i]= Rad2Deg(rocking_angle);
}



void FT2_SpaceCraft::EvalOrbitalPole( Hep3Vector pos1, Hep3Vector pos2, FT2_SpaceCraft &SC, unsigned int i){
    using namespace astro;
    Hep3Vector v=pos1.cross(pos2);
    SkyDir npole(v);
    SC.ORBITAL_POLE_RA[i]=npole.ra();
    SC.ORBITAL_POLE_DEC[i]=npole.dec();
    if(i==1){
        SC.ORBITAL_POLE_RA[0]=SC.ORBITAL_POLE_RA[1];
        SC.ORBITAL_POLE_DEC[0]=SC.ORBITAL_POLE_DEC[1];
    }
}


