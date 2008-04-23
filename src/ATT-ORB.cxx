/** @file ATT-ORB.cxx
 * @brief
 *
 * @author Andrea Tramacere <tramacer@slac.stanford.edu>
 *
 */

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

//---------------- ATTITTUDE CLASS --------------------------------
ATTITUDE::ATTITUDE() {
}

void ATTITUDE::Set_ATT_Size(ATTITUDE &ATT, unsigned int size){
  ATT.Tstart.resize(size);
  ATT.x.resize(size);
  ATT.y.resize(size);
  ATT.z.resize(size);
  ATT.w.resize(size);
  ATT.vx.resize(size);
  ATT.vy.resize(size);
  ATT.vz.resize(size);
  ATT.entr.resize(size);
  ATT.interp_flag.resize(size);
  ATT.gap.resize(size);
}

void ATTITUDE::Print_ATT_Entries(ATTITUDE &ATT){
  std::cout<<"ATT elements --- "<<"\n";
  for (unsigned int i = 0; i < ATT.entr.size(); ++i){
    std::cout
    << i
    <<" "
    <<ATT.Tstart[i]
    <<" "
    << ATT.x[i]
    <<" "
    << ATT.y[i]
    <<" "
    << ATT.z[i]
    <<" "
    << ATT.w[i]
    <<" "
    << ATT.vx[i]
    <<" "
    << ATT.vy[i]
    <<" "
    << ATT.vz[i]
    <<"\n";
  }
}
//----------------------------------------------------------
//------------------------------------------------------------




//-------------------- ORBIT CLASS ------------------------------
ORBIT::ORBIT() {
}
void ORBIT::Set_ORB_Size(ORBIT &ORB, unsigned int size){
  ORB.Tstart.resize(size);
  ORB.x.resize(size);
  ORB.y.resize(size);
  ORB.z.resize(size);
  ORB.vx.resize(size);
  ORB.vy.resize(size);
  ORB.vz.resize(size);
  ORB.CM.resize(size);
  ORB.SAA.resize(size);
  ORB.entr.resize(size);
  ORB.interp_flag.resize(size);
  ORB.gap.resize(size);
}
//------------------------------------------------------------


//------------------------------------------------------------
void ORBIT::Print_ORB_Entries(ORBIT &ORB){
  std::cout<<"ORB elements  "<<"\n";
  std::cout<<"Ts x y z vx vy vz CM SAA \n";
  for (unsigned int i = 0; i < ORB.entr.size(); ++i){
    std::cout
    << i
    <<" "
    << ORB.Tstart[i]
    <<" "
    << ORB.x[i]
    <<" "
    << ORB.y[i]
    <<" "
    << ORB.z[i]
    <<" "
    << ORB.vx[i]
    <<" "
    << ORB.vy[i]
    <<" "
    << ORB.vz[i]
    <<" "
    << ORB.CM[i]
    <<" "
    << ORB.SAA[i]
    <<"\n";
  }
}
//------------------------------------------------------------
//------------------------------------------------------------

//---------PARABOLIC Interpolation ORB if entr=0--------------
void FT2::Interp_ORB_Entries_PARAB_FW(FT2 &FT2, unsigned int i, bool &failed_f){
  //using namespace  ParabInterp;
  double deltat;
  unsigned int jump_b, jump_tot, jump_min, size;
  //Min points to make parabolic regression
  jump_min=3;
  //Number of points to make parabolic regression
  jump_tot=20;
  
  unsigned int max= FT2.ORB.entr.size();
  printf("------------------------------------------------\n");
  printf("-----------PARABOLIC FW EXTRAPOLATION-----------\n");
  printf("Entry =%d\n", i);
  
  FT2.ORB.Tstart[i]=FT2.FT2_T.Tstart[i];
  ParabInterp P;
  size=0;
  
  failed_f = false;
  std::vector<double> x, y, z, t;
  std::vector<double> xr, yr, zr, tr;
  //cerca i punti indietro
  
  jump_b=0;
  for(unsigned int l=0; i>l && jump_b<=jump_tot;l++){
    //printf("i=%d l=%d\n",i,l);
    if(ORB.entr[i-l]>0 && !ORB.gap[i-l]){
      size++;
      jump_b++;
      
      xr.resize(size);
      yr.resize(size);
      zr.resize(size);
      tr.resize(size);
      
      xr[size-1]= FT2.ORB.x[i-l];
      yr[size-1]= FT2.ORB.y[i-l];
      zr[size-1]= FT2.ORB.z[i-l];
      tr[size-1]= FT2.ORB.Tstart[i-l];
      
      if(FT2.verbose){
        std::cout<<"gap "<<FT2.ORB.gap[i-l]<<"\n";
        printf("t=%20.18g, x=%e, y=%e, z=%e \n",
                tr[size-1],
                xr[size-1],
                yr[size-1],
                zr[size-1]);
        printf("jump_b in lopp=%d\n", jump_b);
      }
      
      FT2.ORB.CM[i] = FT2.ORB.CM[i-l];
      FT2.ORB.SAA[i]= FT2.ORB.SAA[i-l];
    }
  }
  
  //ordino temporalmente le ascisse
  if(size>0){
    x.resize(size);
    y.resize(size);
    z.resize(size);
    t.resize(size);
    for(unsigned int l=0;l<size;l++){
      x[l]=xr[size-1-l];
      y[l]=yr[size-1-l];
      z[l]=zr[size-1-l];
      t[l]=tr[size-1-l];;
    }
  }
  
  //printf("jump_b out of the loop=%d\n", jump_b);
  if(jump_b<jump_min){
    failed_f=true;
    if(FT2.verbose){
      printf("failed PARABOLIC FW extrapolation\n");
    }
  }
  
  if(FT2.verbose){
    for(unsigned int l=0;l<size;l++){
      printf("%20.18g %e %e %e\n",
              t[l],
              x[l],
              y[l],
              z[l]);
    }
  }
  
  if(failed_f){
    printf("PARABOLIC FW Extrapolation of ORB\n");
    printf("Extrapolation FAILED !!!!!\n");
  }
  else{
    //interpola
    //y=c*x^2+b*c+a
    double Tshift=t[0];
    
    for(unsigned int l=0;l<size;l++){
      //printf("t=%20.8g\n", t[l]);
      t[l]=t[l]-Tshift;
      //printf("t=%20.8g\n", t[l]);
      x[l]=x[l]/1e3;
      y[l]=y[l]/1e3;
      z[l]=z[l]/1e3;
    }
    
    //N=t.size();
    P.Interp(t, x);
    
    double time=FT2.ORB.Tstart[i]-Tshift;
    P.GetInterp(P, time, FT2.ORB.x[i]);
    FT2.ORB.x[i]*=1e3;
    P.Interp(t, y);
    P.GetInterp(P, time, FT2.ORB.y[i]);
    FT2.ORB.y[i]*=1e3;
    P.Interp(t, z);
    P.GetInterp(P, time, FT2.ORB.z[i]);
    FT2.ORB.z[i]*=1e3;
    
    if(FT2.verbose){
      printf("PARABOLIC FW Extrapolation of ORB\n");
      printf("Successful time=%e  FT2.ORB.x=%e\n", time, FT2.ORB.x[i]);
      printf("Successful T=%e FT2.ORB.x=%e\n", FT2.ORB.Tstart[i], FT2.ORB.x[i]);
      
    }
    printf("------------------------------------------------\n");
  }
}



//----------PARBOLIC Interpolates ORB if entr=0------------------
void FT2::Interp_ORB_Entries_PARAB_BW(FT2 &FT2, unsigned int i, bool &failed_b){
  //using namespace  ParabInterp;
  double deltat;
  unsigned int jump_f, jump_tot, jump_min, size;
  //Min points to make parabolic regression
  jump_min=3;
  //Number of points to make parabolic regression
  jump_tot=20;
  
  unsigned int max= FT2.ORB.entr.size();
  printf("------------------------------------------------\n");
  printf("--------PARABOLIC BW EXTRAPOLAION---------------\n");
  printf("Entry =%d\n", i);
  
  FT2.ORB.Tstart[i]=FT2.FT2_T.Tstart[i];
  ParabInterp P;
  size=0;
  
  failed_b = false;
  std::vector<double> x, y, z, t;
  std::vector<double> xr, yr, zr, tr;
  
  //cerca i punti avanti
  jump_f=0;
  for(unsigned int l=0;(i+l)<max && jump_f<=jump_tot;l++){
    if(FT2.ORB.entr[i+l]>0 && !FT2.ORB.gap[i+l] ){
      jump_f++;
      size++;
      x.resize(size);
      y.resize(size);
      z.resize(size);
      t.resize(size);
      x[size-1]= FT2.ORB.x[i+l];
      y[size-1]= FT2.ORB.y[i+l];
      z[size-1]= FT2.ORB.z[i+l];
      t[size-1]= FT2.ORB.Tstart[i+l];
      if(FT2.verbose){
        printf("t=%20.18g, x=%e, y=%e, z=%e \n",
                t[size-1],
                x[size-1],
                y[size-1],
                z[size-1]);
        printf("jump_f in lopp=%d\n", jump_f);
      }
      
      FT2.ORB.CM[i] = FT2.ORB.CM[i+l];
      FT2.ORB.SAA[i]= FT2.ORB.SAA[i+l];
    }
  }
  //printf("jump_f out of the loop=%d\n", jump_f);
  
  if(jump_f<jump_min) {
    failed_b=true;
    if(FT2.verbose){
      printf("failed PARABOLIC BW extrapolation\n");
    }
  }
  
  if(FT2.verbose){
    for(unsigned int l=0;l<size;l++){
      printf("%20.18g %e %e %e\n",
              t[l],
              x[l],
              y[l],
              z[l]);
    }
  }
  
  if(failed_b){
    printf("PARABOLIC BW extraopolation of ORB\n");
    printf("Extrapolation FAILED !!!!!\n");
  }
  else{
    //interpola
    //y=c*x^2+b*c+a
    double Tshift=t[0];
    
    for(unsigned int l=0;l<size;l++){
      //printf("t=%20.8g\n", t[l]);
      t[l]=t[l]-Tshift;
      //printf("t=%20.8g\n", t[l]);
      x[l]=x[l]/1e3;
      y[l]=y[l]/1e3;
      z[l]=z[l]/1e3;
    }
    
    //N=t.size();
    P.Interp(t, x);
    
    double time=FT2.ORB.Tstart[i]-Tshift;
    P.GetInterp(P, time, FT2.ORB.x[i]);
    FT2.ORB.x[i]*=1e3;
    P.Interp(t, y);
    P.GetInterp(P, time, FT2.ORB.y[i]);
    FT2.ORB.y[i]*=1e3;
    P.Interp(t, z);
    P.GetInterp(P, time, FT2.ORB.z[i]);
    FT2.ORB.z[i]*=1e3;
    
    if(FT2.verbose){
      printf("PARABOLIC BW Extrapolation of ORB\n");
      printf("Successful time=%e  FT2.ORB.x=%e\n", time, FT2.ORB.x[i]);
      printf("Successful T=%e FT2.ORB.x=%e\n", FT2.ORB.Tstart[i], FT2.ORB.x[i]);
      
    }
    printf("------------------------------------------------\n");
  }
}


//-------------Interpolates ORB if entr=0------------------
void FT2::Interp_ORB_Entries(FT2 &FT2){
  //using namespace  ParabInterp;
  double deltat;
  unsigned int jump_b, jump_f, size;
  unsigned int max= FT2.ORB.entr.size();
  unsigned int interp_entr=0;
  printf("----------- Interpolation ORB if entr=0 -----------------\n");
  
  for (unsigned int i = 0; i < FT2.ORB.entr.size(); ++i){
    
    if(FT2.ORB.entr[i]==0){
      interp_entr++;
      
      if(FT2.verbose){
        printf("------------------------------------------------\n");
        printf("Entry =%d\n", i);
      }
      
      FT2.ORB.Tstart[i]=FT2.FT2_T.Tstart[i];
      
      size=0;
      //booleanm variables to test failure
      //to find bw or fw extra/interpolating points
      bool failed_jump_f = false;
      bool failed_jump_b = false;
      
      //booleanm variables to test failure
      //to perform bw or fw extrapolation
      //Note that failed_jump_f = true implies failed_b
      bool failed_f = false;
      bool failed_b = false;
      
      //cerca i punti indietro
      for(jump_b=0;  i>jump_b && FT2.ORB.entr[i-jump_b]==0;jump_b++){
      }
      
      if(jump_b==0|| FT2.ORB.entr[i-jump_b]==0){
        failed_jump_b=true;
        failed_f=true;
        if(FT2.verbose){
          printf("no bkw interpolation points\n");
        }
      }else{
        FT2.ORB.CM[i] =  FT2.ORB.CM[i-jump_b];
        FT2.ORB.SAA[i]= FT2.ORB.SAA[i-jump_b];
      }
      
      //cerca i punti avanti
      for(jump_f=0; (jump_f+i)<max-1  && FT2.ORB.entr[i+jump_f]==0;jump_f++){
      }
      
      if(FT2.verbose){
        printf("jump_f=%d\n", jump_f);
      }
      
      if(jump_f==0 || FT2.ORB.entr[i+jump_f]==0) {
        failed_jump_f=true;
        failed_b=true;
        if(FT2.verbose){
          printf("no fwd interpolation points\n");
        }
      }else{
        FT2.ORB.CM[i] =  FT2.ORB.CM[i+jump_f];
        FT2.ORB.SAA[i]= FT2.ORB.SAA[i+jump_f];
      }
      
      
      if(failed_jump_b && failed_jump_f){
        printf("Interpolation of ORB\n");
        printf("Interpolation FAILED !!!!!\n");
        FT2.ORB.interp_flag[i]=-1;
      }
      
      if(failed_jump_b && !failed_jump_f){
        FT2.Interp_ORB_Entries_PARAB_BW(FT2, i, failed_b);
        FT2.ORB.interp_flag[i]=2;
        //IF ALSO PARAB FAILS
        if(failed_b){
          FT2.ORB.interp_flag[i]=3;
          printf("Also PARAB FAILED I will extrapolate using velocities\n");
          deltat=FT2.ORB.Tstart[i-jump_b]-FT2_T.Tstart[i];
          FT2.ORB.x[i]=FT2.ORB.x[i-jump_b]+FT2.ORB.vx[i-jump_b]*(-deltat);
          FT2.ORB.y[i]=FT2.ORB.y[i-jump_b]+FT2.ORB.vy[i-jump_b]*(-deltat);
          FT2.ORB.z[i]=FT2.ORB.z[i-jump_b]+FT2.ORB.vz[i-jump_b]*(-deltat);
          FT2.ORB.CM[i] =  FT2.ORB.CM[i-jump_b];
          FT2.ORB.SAA[i]= FT2.ORB.SAA[i-jump_b];
        }
      }
      
      if(!failed_jump_b && failed_jump_f){
        FT2.Interp_ORB_Entries_PARAB_FW(FT2, i, failed_f);
        FT2.ORB.interp_flag[i]=2;
        //IF ALSO PARAB FAILS
        if(failed_f){
          FT2.ORB.interp_flag[i]=3;
          printf("Also PARAB FAILED I will extrapolate using velocities\n");
          deltat=FT2.ORB.Tstart[i+jump_f]-FT2_T.Tstart[i];
          FT2.ORB.x[i]=FT2.ORB.x[i+jump_f]+FT2.ORB.vx[i+jump_f]*(-deltat);
          FT2.ORB.y[i]=FT2.ORB.y[i+jump_f]+FT2.ORB.vy[i+jump_f]*(-deltat);
          FT2.ORB.z[i]=FT2.ORB.z[i+jump_f]+FT2.ORB.vz[i+jump_f]*(-deltat);
          FT2.ORB.CM[i] = FT2.ORB.CM[i+jump_f];
          FT2.ORB.SAA[i]= FT2.ORB.SAA[i+jump_f];
        }
      }
      
      if(!failed_jump_b && !failed_jump_f){
        OrbInterp P;
        P.Interp( FT2.ORB.Tstart[i], FT2.ORB, i-jump_b, i+jump_f, i);
        FT2.ORB.interp_flag[i]=1;
        if(FT2.verbose){
          printf("Interpolation of ORB\n");
          printf("Successful T=%e FT2.ORB.x=%e\n", FT2.ORB.Tstart[i], FT2.ORB.x[i]);
          printf("------------------------------------------------\n");
        }
        
      }
      
    }
  }
  printf("Total entries with ORB interpolation=%d\n", interp_entr);
  printf("================================================\n");
}


//-------------Interpolates ORB entry to Tstart------------------
void FT2::Interp_ORB_Tstart(FT2 &FT2){
  double deltat ;
  printf("--------------- Interplates ORB entry to Tstart -------------\n");
  for (unsigned int i = 0; i < FT2.ORB.entr.size(); ++i){
    if(FT2.verbose){
      std::cout<<"ORB elements in Entry "<<i<<","<<FT2.ORB.entr[i]<<"\n";
    }
    
    //if(FT2.ORB.entr[i]>0){
    deltat=-(FT2.ORB.Tstart[i]-FT2.FT2_T.Tstart[i]);
    
    //!!!Interpolate entry to Tstart
    
    
    FT2.ORB.x[i]= FT2.ORB.x[i]+ FT2.ORB.vx[i]*deltat;
    FT2.ORB.y[i]= FT2.ORB.y[i]+ FT2.ORB.vy[i]*deltat;
    FT2.ORB.z[i]= FT2.ORB.z[i]+ FT2.ORB.vz[i]*deltat;
    if(FT2.verbose){
      printf("--- Interpolate to Tstart\n");
      printf("deltat=%e deltax=%e\n", deltat, FT2.ORB.vx[i]*deltat);
    }
    //}
    
  }
}



//-------------Interpolates ATT entry to Tstart------------------
void FT2::Interp_ATT_Tstart(FT2 &FT2){
  double deltat ;
  printf("--------------- Interplates ATT entry to Tstart -------------\n");
  for (unsigned int i = 0; i < FT2.ATT.entr.size(); ++i){
    if(FT2.verbose){
      std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
    }
    
    //if(FT2.ATT.entr[i]>0){
    deltat=-(FT2.ATT.Tstart[i]-FT2.FT2_T.Tstart[i]);
    
    //!!!Interpolate entry to Tstart
    
    
    FT2.ATT.x[i]= FT2.ATT.x[i]+ FT2.ATT.vx[i]*deltat;
    FT2.ATT.y[i]= FT2.ATT.y[i]+ FT2.ATT.vy[i]*deltat;
    FT2.ATT.z[i]= FT2.ATT.z[i]+ FT2.ATT.vz[i]*deltat;
    FT2.ATT.w[i]=sqrt(1- FT2.ATT.x[i]* FT2.ATT.x[i]-
            FT2.ATT.y[i]*FT2.ATT.y[i]-
            FT2.ATT.z[i]*FT2.ATT.z[i]);
    if(FT2.verbose){
      printf("--- Interpolate to Tstart\n");
      printf("deltat=%e deltax=%e\n", deltat, FT2.ATT.vx[i]*deltat);
    }
    //}
    
  }
}





//-------------Interplates ATT if entr=0------------------
void FT2::Interp_ATT_Entries(FT2 &FT2){
  using namespace astro;
  using CLHEP::Hep3Vector;
  
  double deltat, fraction;
  unsigned int jump_b, jump_b1, jump_f, jump_f1;
  
  unsigned int max= FT2.ATT.entr.size()-1;
  unsigned int interp_entr=0;
  
  printf("----------- Interpolates ATT if entr=0 -----------------\n");
  for (unsigned int i = 0; i < FT2.ATT.entr.size(); ++i){
    
    if(FT2.ATT.entr[i]==0){
      
      interp_entr++;
      
      FT2.ATT.Tstart[i]=FT2.FT2_T.Tstart[i];
      //printf("Att_w=%e\n",FT2.ATT.w[i]);
      jump_f=0;
      bool failed_f = false;
      
      while(FT2.ATT.entr[i+jump_f]==0 && ((i+jump_f)<max) || FT2.ATT.gap[i+jump_f]){
        jump_f++;
        //printf("jump_f in lopp=%d\n", jump_f);
      }
      //printf("jump_f out of the loop=%d jump_f+i=%d max=%d\n", jump_f, jump_f+i, max);
      
      if(((i+jump_f)>max)||(jump_f==0)) failed_f=true;
      
      bool failed_b = false;
      jump_b=0;
      while(FT2.ATT.entr[i-jump_b]==0 && (i-jump_b)>0 || FT2.ATT.gap[i+jump_f]){
        jump_b++;
        //printf("jump_b in lopp=%d\n", jump_b);
      }
      //printf("jump_b out of the loop=%d, i-jump_b=%d \n", jump_b, i-jump_b);
      
      if((jump_b>i)||(jump_b==0)) failed_b=true;
      
      if(failed_b && !failed_f){
        FT2.ATT.interp_flag[i]=3;
        printf("---LinIntep of ATT,bkw interp failed I will not use slerp but velocities\n");
        deltat=FT2.ATT.Tstart[i+jump_f]-FT2.FT2_T.Tstart[i];
        FT2.ATT.x[i]=FT2.ATT.x[i+jump_f]+FT2.ATT.vx[i+jump_f]*(-deltat);
        FT2.ATT.y[i]=FT2.ATT.y[i+jump_f]+FT2.ATT.vy[i+jump_f]*(-deltat);
        FT2.ATT.z[i]=FT2.ATT.z[i+jump_f]+FT2.ATT.vz[i+jump_f]*(-deltat);
        FT2.ATT.Eval_w(FT2.ATT, i);
        //printf("Att_w=%e\n",FT2.ATT.w[i]);
        if(FT2.verbose){
          printf("Entry i=%d Time=%20.18g\n", i, FT2.FT2_T.Tstart[i]);
          printf("ATT.x[i+jump_f]=%e \n", FT2.ATT.x[i+jump_f]);
          printf("i+jump_f=%d\n", i+jump_f);
          std::cout<<"deltat  "<<deltat<<"\n";
          std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
          printf("delta+_ATT_x=%e\n", FT2.ATT.x[i+jump_f]-FT2.ATT.x[i]);
        }
      }
      
      if(failed_f && !failed_b){
        FT2.ATT.interp_flag[i]=3;
        printf("---LinIntep of ATT, fwd interp failed I will not use slerp but velocities\n");
        deltat=FT2.ATT.Tstart[i-jump_b]-FT2.FT2_T.Tstart[i];
        FT2.ATT.x[i]=FT2.ATT.x[i-jump_b]+FT2.ATT.vx[i-jump_b]*(-deltat);
        FT2.ATT.y[i]=FT2.ATT.y[i-jump_b]+FT2.ATT.vy[i-jump_b]*(-deltat);
        FT2.ATT.z[i]=FT2.ATT.z[i-jump_b]+FT2.ATT.vz[i-jump_b]*(-deltat);
        FT2.ATT.Eval_w(FT2.ATT, i);
        //printf("Att_w=%e\n",FT2.ATT.w[i]);
        if(FT2.verbose){
          printf("Entry i=%d Time=%20.18g\n", i, FT2.FT2_T.Tstart[i]);
          printf("ATT.x[i-jump_b]=%e \n", FT2.ATT.x[i-jump_b]);
          printf("i-jump_b=%d\n", i-jump_b);
          std::cout<<"deltat  "<<deltat<<"\n";
          std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
          printf("delta-_ATT_x=%e\n", FT2.ATT.x[i-jump_b]-FT2.ATT.x[i]);
        }
      }
      
      if(!failed_b && !failed_f){
        FT2.ATT.interp_flag[i]=1;
        deltat = FT2.ATT.Tstart[i+jump_f]-FT2.ATT.Tstart[i-jump_b];
        
        fraction = (FT2.FT2_T.Tstart[i]-FT2.ATT.Tstart[i-jump_b])/deltat;
        unsigned int index;
        index=i-jump_b;
        FT2.ATT.Eval_w(FT2.ATT, index);
        index=i+jump_f;
        FT2.ATT.Eval_w(FT2.ATT, index);
        Quaternion q1(Hep3Vector(FT2.ATT.x[i-jump_b], FT2.ATT.y[i-jump_b], FT2.ATT.z[i-jump_b]), FT2.ATT.w[i-jump_b]);
        Quaternion q2(Hep3Vector(FT2.ATT.x[i+jump_f], FT2.ATT.y[i+jump_f], FT2.ATT.z[i+jump_f]), FT2.ATT.w[i+jump_f]);
        //Quaternion q1(Hep3Vector(FT2.ATT.x[i-jump_b], FT2.ATT.y[i-jump_b], FT2.ATT.z[i-jump_b]));
        //Quaternion q2(Hep3Vector(FT2.ATT.x[i+jump_f], FT2.ATT.y[i+jump_f], FT2.ATT.z[i+jump_f]));
        Quaternion interp(q1.interpolate(q2, fraction));
        
        FT2.ATT.x[i]=interp.vector().x();
        FT2.ATT.y[i]=interp.vector().y();
        FT2.ATT.z[i]=interp.vector().z();
        FT2.ATT.Eval_w(FT2.ATT, i);
        //printf("Att_w=%e\n",FT2.ATT.w[i]);
        if(FT2.verbose){
          printf("---Slerp Interpolation of ATT\n");
          printf("Entry i=%d Time=%20.18g\n", i, FT2.FT2_T.Tstart[i]);
          printf("ATT.x[i-jump_b]=%e ATT.x[i+jump_f]=%e interp=%e\n",
                  FT2.ATT.x[i-jump_b],
                  FT2.ATT.x[i+jump_f],
                  interp.vector().x());
          printf("i-jump_b=%d i+jump_f=%d\n", i-jump_b, i+jump_f);
          printf("deltat=%20.18e fraction=%20.18e\n", deltat, fraction);
          std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
          printf("jump_f=%d jump_b=%d delta-_ATT_x=%e delta+_ATT_x=%e\n",
                  jump_f,
                  jump_b,
                  FT2.ATT.x[i]- FT2.ATT.x[i-jump_b],
                  FT2.ATT.x[i+jump_f]-FT2.ATT.x[i]);
          printf("----------------------------------------------------------\n");
        }
      }
      if(failed_b && failed_f){
        printf("ATT Interpolation failed\n");
        FT2.ATT.interp_flag[i]=-1;
      }
      
    }
  }
  printf("Total entries with ATT interpolation=%d\n", interp_entr);
  printf("----------------------------------------------------------\n");
}




//-------------- Update and clean M7 fields in the FT2 Entries -----------------------
void FT2::Clean_ATT_Quaternions(ATTITUDE &Att, unsigned int entry){
  Att.entr[entry]=0;
  Att.interp_flag[entry]=0;
  Att.Tstart[entry]=0;
  Att.x[entry]=0;
  Att.y[entry]=0;
  Att.z[entry]=0;
  Att.w[entry]=0;
  Att.vx[entry]=0;
  Att.vy[entry]=0;
  Att.vz[entry]=0;
  Att.gap[entry]=true;
  
}


void FT2::Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens , double time, unsigned int entry){
  Att.entr[entry]++;
  
  //Update only if the entry is the first for the time bin
  if( Att.entr[entry]==1){
    //printf("ATT update\n");
    Att.Tstart[entry]=time;
    Att.x[entry]=atof(tokens[5].c_str());
    Att.y[entry]=atof(tokens[6].c_str());
    Att.z[entry]=atof(tokens[7].c_str());
    Att.w[entry]=atof(tokens[8].c_str());
    Att.vx[entry]=atof(tokens[9].c_str());
    Att.vy[entry]=atof(tokens[10].c_str());
    Att.vz[entry]=atof(tokens[11].c_str());
    Att.gap[entry]=false;
    
  }
}

void FT2::Clean_ORB(ORBIT &Orb, unsigned int entry){
  Orb.entr[entry]=0;
  Orb.interp_flag[entry]=0;
  Orb.Tstart[entry]=0;
  Orb.x[entry]=0;
  Orb.y[entry]=0;
  Orb.z[entry]=0;
  Orb.vx[entry]=0;
  Orb.vy[entry]=0;
  Orb.vz[entry]=0;
  Orb.CM[entry]=0;
  Orb.SAA[entry]=0;
  Orb.gap[entry]=true;
}


void FT2::Update_ORB(ORBIT &Orb, const std::vector<std::string> &tokens, double time, unsigned int entry){
  Orb.entr[entry]++;
  double deltat;
  //Update only if the entry is the first for the time bin
  if (Orb.entr[entry]==1){
    //printf("ORB update\n");
    Orb.Tstart[entry]=time;
    Orb.x[entry]=atof(tokens[5].c_str());
    Orb.y[entry]=atof(tokens[6].c_str());
    Orb.z[entry]=atof(tokens[7].c_str());
    Orb.vx[entry]=atof(tokens[8].c_str());
    Orb.vy[entry]=atof(tokens[9].c_str());
    Orb.vz[entry]=atof(tokens[10].c_str());
    Orb.CM[entry]=atoi(tokens[11].c_str());
    Orb.SAA[entry]=atoi(tokens[12].c_str());
    Orb.gap[entry]=false;
    
  }
  
}

