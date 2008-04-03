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


//-------------Interplates ORB if Vel=0------------------
void FT2::Interp_ORB_Vel_Entries(FT2 &FT2){
  double deltat ;
  unsigned int jump_b, jump_b1, jump_f, jump_f1;
  double t, t1, t2, v1, v2;
  
  printf("--------------------------- Interplates ORB if Vel=0 ---------------------------\n");
  
  //First Entry
  unsigned int i=0;
  if(FT2.ORB.vx[i]==0 && FT2.ORB.entr[i]>0 ){
    //printf("Interpolation of ORB\n");
    //looks for the first entries wiht non zero velocities
    jump_f=1;
    while(FT2.ORB.vx[i+jump_f]==0){
      jump_f++;
    }
    
    jump_f1=jump_f+1;
    while(FT2.ORB.vx[i+jump_f1]==0){
      jump_f1++;
    }
    //FT2.ORB.Tstart[i]=FT2_T.Tstart[i];
    t1= FT2.ORB.Tstart[i+jump_f];
    t2= FT2.ORB.Tstart[i+jump_f1];
    t = FT2.ORB.Tstart[i];
    FT2.ORB.vx[i]= FT2.lininterp(FT2.ORB.vx[i+jump_f], FT2.ORB.vx[i+jump_f1], t1, t2, t);
    FT2.ORB.vy[i]= FT2.lininterp(FT2.ORB.vy[i+jump_f], FT2.ORB.vy[i+jump_f1], t1, t2, t);
    FT2.ORB.vz[i]= FT2.lininterp(FT2.ORB.vz[i+jump_f], FT2.ORB.vz[i+jump_f1], t1, t2, t);
    
    printf("jump_f=%d jump_f1=%d vx=%e vy=%e vz=%e\n", jump_f, jump_f1, FT2.ORB.vx[i], FT2.ORB.vy[i], FT2.ORB.vz[i]);
  }
  
  
  // Second to N-1 entry
  unsigned int max= FT2.ORB.entr.size()-1;
  for (unsigned int i = 1; i < FT2.ORB.entr.size()-1; ++i){
    
    
    if(FT2.ORB.vx[i]==0 && FT2.ORB.entr[i]>0 ){
      
      //looks for the first entries wiht non zero velocities
      jump_b=1;
      while(FT2.ORB.vx[i-jump_b]==0 && (i-jump_b>0)){
        jump_b++;
      }
      
      jump_f=1;
      while(FT2.ORB.vx[i+jump_f]==0 && (i+jump_f<max)){
        jump_f++;
      }
      
      
      //If the jump fails replace
      if(i+jump_f==max){
        FT2.ORB.vx[i]= FT2.ORB.vx[i-jump_b];
        FT2.ORB.vy[i]= FT2.ORB.vy[i-jump_b];
        FT2.ORB.vz[i]= FT2.ORB.vz[i-jump_b];
        printf("jump failed in forward\n");
      }
      else if(i-jump_b==0){
        FT2.ORB.vx[i]= FT2.ORB.vx[i+jump_f];
        FT2.ORB.vy[i]= FT2.ORB.vy[i+jump_f];
        FT2.ORB.vz[i]= FT2.ORB.vz[i+jump_f];
        printf("jump failed in  backward\n");
      }else{
        //!!! Interpolate velocities
        t1= FT2.ORB.Tstart[i-jump_b];
        t2= FT2.ORB.Tstart[i+jump_f];
        t = FT2.ORB.Tstart[i];
        FT2.ORB.vx[i]= FT2.lininterp(FT2.ORB.vx[i-jump_b], FT2.ORB.vx[i+jump_f], t1, t2, t);
        FT2.ORB.vy[i]= FT2.lininterp(FT2.ORB.vy[i-jump_b], FT2.ORB.vy[i+jump_f], t1, t2, t);
        FT2.ORB.vz[i]= FT2.lininterp(FT2.ORB.vz[i-jump_b], FT2.ORB.vz[i+jump_f], t1, t2, t);
      }
      
      if(FT2.verbose){
        printf("--------------------------\n");
        printf("Interpolation of ORB Vel\n");
        std::cout<<"ORB elements in Entry "<<i<<","<<FT2.ORB.entr[i]<<"\n";
        
        printf("jump_b=%d i=%d vx_b=%20.18g\n", jump_b, i, FT2.ORB.vx[i-jump_b]);
        printf("jump_f=%d i=%d vx_f=%20.18g\n", jump_f, i, FT2.ORB.vx[i+jump_f]);
        printf("vx lin interp=%20.18g\n", FT2.ORB.vx[i]);
        printf("--------------------------\n");
      }
    }
    
  }
  
  //Last Entry
  i=FT2.ORB.entr.size()-1;
  if(FT2.ORB.vx[i]==0 && FT2.ORB.entr[i]>0 ){
    //printf("Interpolation of ORB\n");
    //looks for the first entries wiht non zero velocities
    jump_b=1;
    while(FT2.ORB.vx[i-jump_b]==0){
      jump_b++;
    }
    jump_b1=jump_b+1;
    
    while(FT2.ORB.vx[i-jump_b1]==0){
      jump_b1++;
    }
    
    //FT2.ORB.Tstart[i]=FT2_T.Tstart[i];
    
    t1= FT2.ORB.Tstart[i-jump_b1];
    t2= FT2.ORB.Tstart[i-jump_b];
    t = FT2.ORB.Tstart[i];
    FT2.ORB.vx[i]= FT2.lininterp(FT2.ORB.vx[i-jump_b1], FT2.ORB.vx[i-jump_b], t1, t2, t);
    FT2.ORB.vy[i]= FT2.lininterp(FT2.ORB.vy[i-jump_b1], FT2.ORB.vy[i-jump_b], t1, t2, t);
    FT2.ORB.vz[i]= FT2.lininterp(FT2.ORB.vz[i-jump_b1], FT2.ORB.vz[i-jump_b], t1, t2, t);
    
    if(FT2.verbose){
      printf("jump_b=%d jump_b1=%d \n", jump_b, jump_b1);
    }
  }
  
  
}

//-------------Interpolates ORB if entr=0------------------
void FT2::Interp_ORB_Entries(FT2 &FT2){
  //using namespace  ParabInterp;
  double deltat;
  unsigned int jump_b, jump_f, jump_tot, jump_min, size;
  
  //Min points to make parabolic regression
  jump_min=3;
  
  //Number of points to make parabolic regression
  jump_tot=50;
  
  unsigned int max= FT2.ORB.entr.size();
  
  printf("----------- Interpolates ORB if entr=0 -----------------\n");
  
  for (unsigned int i = 0; i < FT2.ORB.entr.size(); ++i){
    
    if(FT2.ORB.entr[i]==0){
      printf("------------------------------------------------\n");
      printf("Entry =%d\n", i);
      
      FT2.ORB.Tstart[i]=FT2_T.Tstart[i];
      
      ParabInterp P;
      
      
      size=0;
      
      bool failed_f = false;
      bool failed_b = false;
      std::vector<double> x, y, z, t;
      std::vector<double> xr, yr, zr, tr;
      //cerca i punti indietro
      
      jump_b=0;
      for(unsigned int l=0; i>l && jump_b<=25;l++){
        //printf("i=%d l=%d\n",i,l);
        if(FT2.ORB.entr[i-l]>0 && !FT2.ORB.gap[i-l]){
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
          
          FT2.ORB.CM[i] =  FT2.ORB.CM[i-l];
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
      
      if(jump_b==0){
        failed_b=true;
        if(FT2.verbose){
          printf("failed bkw interpolation\n");
        }
      }
      
      //cerca i punti avanti
      jump_f=0;
      for(unsigned int l=0;(i+l)<max && jump_f<(jump_tot-jump_b);l++){
        
        
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
          
          FT2.ORB.CM[i] =  FT2.ORB.CM[i+l];
          FT2.ORB.SAA[i]= FT2.ORB.SAA[i+l];
        }
      }
      //printf("jump_f out of the loop=%d\n", jump_f);
      
      if(jump_f==0) {
        failed_f=true;
        if(FT2.verbose){
          printf("failing fwd interpolation\n");
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
      
      if(failed_b & failed_f){
        printf("Interpolation of ORB\n");
        printf("Interpolation FAILED !!!!!\n");
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
          printf("Interpolation of ORB\n");
          printf("Successful time=%e  FT2.ORB.x=%e\n", time, FT2.ORB.x[i]);
          printf("Successful T=%e FT2.ORB.x=%e\n", FT2.ORB.Tstart[i], FT2.ORB.x[i]);
          
        }
        printf("------------------------------------------------\n");
      }
      
    }
  }
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
    deltat=-(FT2.ORB.Tstart[i]-FT2_T.Tstart[i]);
    
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
    deltat=-(FT2.ATT.Tstart[i]-FT2_T.Tstart[i]);
    
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
  
  printf("----------- Interpolates ATT if entr=0 -----------------\n");
  for (unsigned int i = 0; i < FT2.ATT.entr.size(); ++i){
    
    if(FT2.ATT.entr[i]==0){
      
      FT2.ATT.Tstart[i]=FT2_T.Tstart[i];
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
        printf("---LinIntep of ATT,bkw interp failed I will not use slerp but velocities\n");
        deltat=FT2.ORB.Tstart[i+jump_f]-FT2_T.Tstart[i];
        FT2.ATT.x[i]=FT2.ATT.x[i+jump_f]+FT2.ATT.vx[i+jump_f]*(-deltat);
        FT2.ATT.y[i]=FT2.ATT.y[i+jump_f]+FT2.ATT.vy[i+jump_f]*(-deltat);
        FT2.ATT.z[i]=FT2.ATT.z[i+jump_f]+FT2.ATT.vz[i+jump_f]*(-deltat);
        FT2.ATT.Eval_w(FT2.ATT,i);
        //printf("Att_w=%e\n",FT2.ATT.w[i]);
        if(FT2.verbose){
          printf("Entry i=%d Time=%20.18g\n", i, FT2_T.Tstart[i]);
          printf("ATT.x[i+jump_f]=%e \n",FT2.ATT.x[i+jump_f]);
          printf("i+jump_f=%d\n", i+jump_f);
          std::cout<<"deltat  "<<deltat<<"\n";
          std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
          printf("delta+_ATT_x=%e\n",FT2.ATT.x[i+jump_f]-FT2.ATT.x[i]);
        }
      }
      
      if(failed_f && !failed_b){
        printf("---LinIntep of ATT, fwd interp failed I will not use slerp but velocities\n");
        deltat=FT2.ORB.Tstart[i-jump_b]-FT2_T.Tstart[i];
        FT2.ATT.x[i]=FT2.ATT.x[i-jump_b]+FT2.ATT.vx[i-jump_b]*(-deltat);
        FT2.ATT.y[i]=FT2.ATT.y[i-jump_b]+FT2.ATT.vy[i-jump_b]*(-deltat);
        FT2.ATT.z[i]=FT2.ATT.z[i-jump_b]+FT2.ATT.vz[i-jump_b]*(-deltat);
        FT2.ATT.Eval_w(FT2.ATT,i);
        //printf("Att_w=%e\n",FT2.ATT.w[i]);
        if(FT2.verbose){       
          printf("Entry i=%d Time=%20.18g\n", i, FT2_T.Tstart[i]);
          printf("ATT.x[i-jump_b]=%e \n",FT2.ATT.x[i-jump_b]);
          printf("i-jump_b=%d\n", i-jump_b);
          std::cout<<"deltat  "<<deltat<<"\n";
          std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
          printf("delta-_ATT_x=%e\n",FT2.ATT.x[i-jump_b]-FT2.ATT.x[i]);
        }
      }
      
      if(!failed_b && !failed_f){
        deltat = FT2_T.Tstart[i+jump_f]-FT2.ATT.Tstart[i-jump_b];
        
        fraction = (FT2_T.Tstart[i]-FT2.ATT.Tstart[i-jump_b])/deltat;
        
       

        Quaternion q1(Hep3Vector(FT2.ATT.x[i-jump_b], FT2.ATT.y[i-jump_b], FT2.ATT.z[i-jump_b]), FT2.ATT.w[i-jump_b]);
        Quaternion q2(Hep3Vector(FT2.ATT.x[i+jump_f], FT2.ATT.y[i+jump_f], FT2.ATT.z[i+jump_f]), FT2.ATT.w[i+jump_f]);
        Quaternion interp(q1.interpolate(q2, fraction));
        
        
        
        FT2.ATT.x[i]=interp.vector().x();
        FT2.ATT.y[i]=interp.vector().y();
        FT2.ATT.z[i]=interp.vector().z();
        FT2.ATT.Eval_w(FT2.ATT,i);
        //printf("Att_w=%e\n",FT2.ATT.w[i]);
        if(FT2.verbose){          
          printf("---Slerp Interpolation of ATT\n");
          printf("Entry i=%d Time=%20.18g\n", i, FT2_T.Tstart[i]);
          printf("ATT.x[i-jump_b]=%e ATT.x[i+jump_f]=%e interp=%e\n",
                  FT2.ATT.x[i-jump_b],
                  FT2.ATT.x[i+jump_f],
                  interp.vector().x());
          printf("i-jump_b=%d i+jump_f=%d\n", i-jump_b, i+jump_f);
          std::cout<<"deltat  "<<deltat<<"\n";
          std::cout<<"fraction  "<<fraction<<"\n";
          std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
          printf("jump_f=%d jump_b=%d delta-_ATT_x=%e delta+_ATT_x=%e\n",
                  jump_f,
                  jump_b,
                  FT2.ATT.x[i]- FT2.ATT.x[i-jump_b],
                  FT2.ATT.x[i+jump_f]-FT2.ATT.x[i]);
          
        }
      }
      if(failed_b && failed_f) printf("ATT Interpolation failed\n");
      printf("----------------------------------------------------------\n");
    }
  }
  
  
}




//-------------- Update and clean M7 fields in the FT2 Entries -----------------------
void FT2::Clean_ATT_Quaternions(ATTITUDE &Att, unsigned int entry){
  Att.entr[entry]=0;
  Att.Tstart[entry]=0;
  Att.y[entry]=0;
  Att.z[entry]=0;
  Att.w[entry]=0;
  Att.vx[entry]=0;
  Att.vy[entry]=0;
  Att.vz[entry]=0;
  Att.gap[entry]=true;
  
}


void FT2::Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens, unsigned int entry){
  Att.entr[entry]++;
  
  //Update only if the entry is the first for the time bin
  if( Att.entr[entry]==1){
    Att.Tstart[entry]=FT2::Get_M7_Time(tokens[3], tokens[4]);
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


void FT2::Update_ORB(ORBIT &Orb, const std::vector<std::string> &tokens, unsigned int entry, double Entry_Tstart){
  Orb.entr[entry]++;
  double deltat;
  //Update only if the entry is the first for the time bin
  if (Orb.entr[entry]==1){
    //printf("--\n");
    Orb.Tstart[entry]=FT2::Get_M7_Time(tokens[3], tokens[4]);
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

