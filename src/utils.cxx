/** @file utils.cxx
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


//-------------------Get Files Names -------------------------
void FT2::getFileNames(int iargc, char * argv[], FT2 &FT2) {
  const char usage[] =
  "usage: makeFT2Entries.exe  [OPTIONS]\n"
  " -DigiFile <FileName>\n"
  " -MeritFile <FileName>\n"
  " -M7File <FileName>\n"
  " -FT2_txt_File <FileName>\n"
  " -FT2_fits_File <FileName> \n"
  "-Gaps_File <FileName> \n"
  " -Version <vesion of the file>\n"
  " --Gleam \n"
  " --verbose\n"
  " -h --help\n";
  printf("iargc=%d\n",iargc);
  if (iargc < 2) {
    std::cout << usage;
    std::exit(0);
  }   else  {
    int i;
    std::cout<<"command line"<<std::endl;
    for( i = 1; i < iargc; i++ ){
      std::string par = argv[i];
      if (argv[i][0] == '-'){
        std::cout<<argv[i]<<std::endl;
        if(par=="-DigiFile") {
          FT2.DigiFile = std::string(argv[i+1]);
          std::cout<<"Digi File="<<FT2.DigiFile<<std::endl;
        }
        if(par=="-MeritFile"){
          FT2.MeritFile = std::string(argv[i+1]);
          std::cout<<"MeritFile="<<FT2.MeritFile<<std::endl;
        }
        if(par=="-M7File"){
          FT2.M7File = std::string(argv[i+1]);
          std::cout<<"M7 File="<<FT2.M7File<<std::endl;
        }
        if(par=="-FT2_txt_File"){
          FT2.FT2_txt_File = std::string(argv[i+1]);
          std::cout<<"OUT TXT FILE="<<FT2.FT2_txt_File<<std::endl;
        }
        if(par=="-FT2_fits_File"){
          FT2.FT2_fits_File= std::string(argv[i+1]);
          std::cout<<"OUT FITS FILE="<<FT2.FT2_fits_File<<std::endl;
        }
        if(par=="-Gaps_File"){
          FT2.DigiGAPS=true;
          FT2.Gaps_File= std::string(argv[i+1]);
          std::cout<<"Gaps File="<<FT2.Gaps_File<<std::endl;
        }
        if(par=="-Version"){
          FT2.Version= std::string(argv[i+1]);
          
          std::cout<<"-Version="<<FT2.Version<<std::endl;
        }
        
        if(par=="-h"){
          std::cout << usage;
          std::exit(0);
        }
        if(par=="--help"){
          std::cout << usage;
          std::exit(0);
        }
        if(par=="--Gleam"){
          std::cout<<"Gleam FT2\n";
          FT2.Gleam=true;
        }
        if(par=="--verbose"){
          std::cout<<"verbose\n";
          FT2.verbose=true;
        }
        if(par=="--MC"){
          std::cout<<"MonteCarlo \n";
          FT2.MC=true;
        }
        if(par=="--test-quaternion"){
          FT2.TestQ=true;
        }
        
      }
    }
    std::cout<<"---------------------------------------------------------"
    <<std::endl;
  }
}





//-----------------Line Number Counter-------------------------
unsigned int  FT2::LineNumberCount(const std::string & infile){
  std::ifstream file(infile.c_str());
  //unsigned long Lines;
  unsigned int nlines(0);
  std::string line;
  //std::cout<<infile<<std::endl;
  while (std::getline(file, line, '\n')) {
    nlines++;
  }
  
  file.close();
  //std::cout<< nlines <<std::endl;
  return nlines;
}

void FT2::Get_DigiFileLineNumber(FT2 &FT2, const std::string & infile){
  //std::cout<<infile<<std::endl ;
  FT2.DigiFileLineNumber=LineNumberCount(infile);
}





//---------------Get FT2 Entry Index    ---------------------------------
void FT2::Get_FT2_Entry_Index(FT2 &FT2 , double time, unsigned int &i){
  unsigned Entries ;
  unsigned int l;
  
  //FT2 entries Number
  Entries=Get_FT2_Entries(FT2);
  
  
  //Set true the out of range value
  FT2.Set_OutOfRange_TRUE(FT2);
  
  //Look for the Entry index
  bool stop=false;
  for(l=0;l<Entries;l++){
    if((time<FT2.FT2_T.Tstop[l])&&(time>=FT2.FT2_T.Tstart[l]) && !stop){
      Set_OutOfRange_FALSE(FT2);
      i=l;
      stop=true;
    }
  }
  //l=Entries-1;
  
  //if((time<=FT2.FT2_T.Tstop[l])&&(time>=FT2.FT2_T.Tstart[l])){
  //  Set_OutOfRange_FALSE(FT2);
  // i=l;
  //}
  
  if (FT2.Get_OutOfRange(FT2)){
    std::cout<<"!!!Warning "
    <<"data out of any Bin time interval "
    <<"time "
    <<std::setprecision(20)
    <<time
    <<" Tstart of First Bin "
    <<FT2.FT2_T.Tstart[0]
    <<" Tstop of Last Bin "
    <<FT2.FT2_T.Tstop[FT2.FT2_T.Tstop.size()-1]
    <<std::endl;
  }
}


//----------------------------------------------------------------------------
int FT2::Get_FT2_Time_Bin(double time, double Tstart){
  unsigned int i;
  
  i=int((time-Tstart)/FT2_BIN_WIDTH);
  //std::cout<<"bin "
  //    <<i
  //    <<std::endl;
  
  return i;
}


//-------------------------------Get M-7 Time  -------------------------------------------
double  FT2::Get_M7_Time(const std::string &Time, const std::string &Frac_Time){
  double time , f_time;
  
  time=atof(Time.c_str());
  f_time=atof(Frac_Time.c_str());
  f_time*=1.0e-6;
  
  return (time+f_time);
}


double FT2::lininterp(double x1, double x2, double t1, double t2, double t){
  
  return x1+((x2-x1)/(t2-t1))*(t-t1);
}



//-------------------------- Parab Interpolation ------------------------------------------
// @author Andrea Tramacere <tramacer@slac.stanford.edu>
//
// Adpted from
//
// GNU General Public License Agreement
// Copyright (C) 2004-2007 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs.
// You must retain a copy of this licence in all copies.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------
//! Approximates an arbitrary function using parabolic least squares fitting.

void ParabInterp::GetCoeff(ParabInterp p, double &a, double &b, double &c) {
  //y=c*x^2+b*c+a
  a=p.pa;
  b=p.pb;
  c=p.pc;
}


void ParabInterp::Interp(std::vector <double> &x, std::vector <double> &y) {
  //y=c*x^2+b*x+a
  
  unsigned int n;
  n=x.size();
  
  double a0(1), a1(0), a2(0), a3(0), a4(0);
  double b0(0), b1(0), b2(0);
  
  for (unsigned int m = 0; m < n; m++) {
    
    double xx = x[m], yy = y[m];
    
    a1 += xx; xx *= x[m];
    a2 += xx; xx *= x[m];
    a3 += xx; xx *= x[m];
    a4 += xx; xx  = x[m];
    
    b0 += yy;
    b1 += yy * xx;
    b2 += yy * xx * xx;
    
  }
  a1 /= n; a2 /= n; a3 /= n; a4 /= n;
  b0 /= n; b1 /= n; b2 /= n;
  
  double d = a0 * (a2 * a4 - a3 * a3) - a1 * (a1 * a4 - a2 * a3) + a2 * (a1 * a3 - a2 * a2);
  
  pa = b0 * (a2 * a4 - a3 * a3) + b1 * (a2 * a3 - a1 * a4) + b2 * (a1 * a3 - a2 * a2);
  pb = b0 * (a2 * a3 - a1 * a4) + b1 * (a0 * a4 - a2 * a2) + b2 * (a1 * a2 - a0 * a3);
  pc = b0 * (a1 * a3 - a2 * a2) + b1 * (a2 * a1 - a0 * a3) + b2 * (a0 * a2 - a1 * a1);
  pa /= d; pb /= d; pc /= d;
  
}

void ParabInterp::GetInterp(ParabInterp p, double x, double &y){
  //y=c*x^2+b*x+a
  double a, b, c;
  p.GetCoeff(p, a, b, c);
  //printf("c=%e b=%e a=%e\n",c,b,a);
  y=x*x*c + x*b + a;
}

//-------------------------- Orb Interpolation Wrapper--------------------------
// @author Andrea Tramacere <tramacer@slac.stanford.edu>
// wrapper of the code from Lucas Guillemot,

void OrbInterp::Interp(double t, ORBIT  &Orb, unsigned int i1, unsigned int i2, unsigned int interp){
  
  double sctime1 = Orb.Tstart[i1];
  double sctime2 = Orb.Tstart[i2];
  double intposn[3];
  double scposn1[3] = { Orb.x[i1], Orb.y[i1], Orb.z[i1] };
  double scposn2[3] = { Orb.x[i2], Orb.y[i2], Orb.z[i2] };
  double fract = 0.;
  int ii;
  
  /* Interpolate. */
  fract = (t - sctime1) / (sctime2 - sctime1);
  
  double length1, length2, length12, intlength;
  double vector12[3], vectprod_out[3];
  
  /* linear interpolation for vector length */
  length1 = sqrt(inner_product(scposn1, scposn1));
  length2 = sqrt(inner_product(scposn2, scposn2));
  intlength = length1 + fract*(length2 - length1);
  
  /* compute a base vector on the orbital plane (vector12) */
  outer_product(scposn1, scposn2, vectprod_out);
  outer_product(vectprod_out, scposn1, vector12);
  length12 = sqrt(inner_product(vector12, vector12));
  
  /* check vectors scposn1 and scposn2 */
  if ((length1 == 0.0) && (length2 == 0.0)) {
    /* both vectors are null */
    for (ii = 0; ii < 3; ++ii) intposn[ii] = 0.0;
    
  } else if (length1 == 0.0) {
    /* scposn1 is null, but scposn2 is not */
    for (ii = 0; ii < 3; ++ii) {
      intposn[ii] = scposn2[ii] / length2 * intlength;
    }
    
  } else if ((length2 == 0.0) || (length12 == 0.0)) {
    /* left:  scposn2 is null, but scposn1 is not */
    /* right: either vector is not null, but they are parallel */
    for (ii = 0; ii < 3; ++ii) {
      intposn[ii] = scposn1[ii] / length1 * intlength;
    }
    
  } else { /* both has a non-zero length, and they are not parallel */
    double inttheta, factor_cos, factor_sin;
    /* linear interpolation for orbital phase */
    inttheta = fract * acos(inner_product(scposn1, scposn2)
            / length1 / length2);
    factor_cos = cos(inttheta);
    factor_sin = sin(inttheta);
    for (ii = 0; ii < 3; ++ii) {
      intposn[ii] = intlength * (scposn1[ii] / length1 * factor_cos
              + vector12[ii] / length12 * factor_sin);
    }
  }
  Orb.x[interp]= intposn[0];
  Orb.y[interp]= intposn[1];
  Orb.z[interp]= intposn[2];
}


/* compute vector inner product */
double OrbInterp:: inner_product(double vect_x[], double vect_y[]) {
  return vect_x[0]*vect_y[0] + vect_x[1]*vect_y[1] + vect_x[2]*vect_y[2];
}

/* compute vector outer product */
double OrbInterp::outer_product(double vect_x[], double vect_y[], double vect_z[]) {
  vect_z[0] = vect_x[1]*vect_y[2] - vect_x[2]*vect_y[1];
  vect_z[1] = vect_x[2]*vect_y[0] - vect_x[0]*vect_y[2];
  vect_z[2] = vect_x[0]*vect_y[1] - vect_x[1]*vect_y[0];
}




//------------------------------- Eval W-------------------------------------------

void ATTITUDE::Eval_w(ATTITUDE &Att, unsigned int i){
  double ax, ay, az, aw;
  ax=Att.x[i]*Att.x[i];
  ay=Att.y[i]*Att.y[i];
  az=Att.z[i]*Att.z[i];
  aw=sqrt(1.0-(ax+ay+az));
  //Sign Correction on the scalar component
  if(Att.w[i]<0){
    aw=-aw;
  }
  Att.w[i]=aw;
}



//------------------------- Test Quaternion ---------------------------------------
void FT2::TestQuaternion(){
  using namespace astro;
  Quaternion q1(Hep3Vector( 0.305193, 0.431166, 0.161976));
  Quaternion q2(Hep3Vector( 0.305161, 0.431174, 0.161991));
  for(unsigned int i=0;i<100;i++){
    double fraction=double(i)/100.0;
    Quaternion interp(q1.interpolate(q2, fraction));
    printf("%e %e %e\n",q1.vector().x(),q2.vector().x(),interp.vector().x() );
  }
  double a1x=q1.vector().x()*q1.vector().x();
  double a1y=q1.vector().y()*q1.vector().y();
  double a1z=q1.vector().z()*q1.vector().z();
  double w1=sqrt(1.0-(a1x+a1y+a1z));
  
  Quaternion q3(Hep3Vector( 0.305193, 0.431166, 0.161976),w1);

  double a2x=q2.vector().x()*q2.vector().x();
  double a2y=q2.vector().y()*q2.vector().y();
  double a2z=q2.vector().z()*q2.vector().z();
  double w2=sqrt(1.0-(a2x+a2y+a2z));
  
  Quaternion q4(Hep3Vector( 0.305161, 0.431174, 0.161991),w2);
  for(unsigned int i=0;i<100;i++){
    double fraction=double(i)/100.0;
    Quaternion interp1(q3.interpolate(q4, fraction));
    printf("%e %e %e\n",q3.vector().x(),q4.vector().x(),interp1.vector().x() );
  }
  
}