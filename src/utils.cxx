/** @file utils.cxx
 * @brief
 *
 * @author Andrea Tramacere <tramacer@slac.stanford.edu>
 *
 */

//c/ headers
#include <stdio.h>
#include <stdlib.h>

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
  " -Gaps_File <FileName> \n"
  " -DigiTstart <Time of first Digi event>\n "
  " -DigiTstop <Time of last Digi event> \n "
  " -Version <vesion of the file>\n"
  " -LatConfig <LAT config  for run intent>\n"
  " -DataQual <data quality ID>\n"
  " -ATTNomrTolerance <Tolerance for The normaliazation of the quaternion (def=1e-5)>\n"
  " -ATTDeltaT_TstartTolerance <Tolerance for moving ATT entry for actual TSTAMP to enetry TSTART (def=1e-5 s)>\n"
  " -ORBDeltaT_TstartTolerance <Tolerance for moving ORB entry for actual TSTAMP to enetry TSTART (def=1e-5 s)>\n"
  " -FT2_BIN_WIDTH <span of the bin of the non merged FT2 file (def=1 s)>\n"
  " -new_tpl <to path to an alternative ft2 tpl file>\n "
  " --MC\n"
  " --Gleam\n"
  " --verbose\n"
  " -h --help\n"
  " --test-quaternion\n";
  
  printf("iargc=%d\n", iargc);
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
         if(par=="-LatConfig"){
          FT2.LAT_CONFIG= atoi(std::string(argv[i+1]).c_str());
          std::cout<<"-LatConfig="<<FT2.LAT_CONFIG<<std::endl;
        }
        if(par=="-DataQual"){
          FT2.DATA_QUAL= atoi(std::string(argv[i+1]).c_str());
          std::cout<<"-DataQual="<<FT2.DATA_QUAL<<std::endl;
        }
        if(par=="-DigiTstart"){
          FT2.GleamDigiTstart=atof(std::string(argv[i+1]).c_str());
          printf("DigiTstart=%20.18g\n", FT2.GleamDigiTstart);
        }
        if(par=="-DigiTstop"){
          FT2.GleamDigiTstop=atof(std::string(argv[i+1]).c_str());
          printf("DigiTstop=%20.18g\n", FT2.GleamDigiTstop);
        }
        if(par=="-new_tpl" ){
            FT2.new_tpl=true;
            FT2.path=std::string(argv[i+1]);
            std::cout<<"You are using an alternative  ft2 template"<<FT2.path <<" \n";
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
          std::cout<<"TestQ \n";
          FT2.ATT.TestQ=true;
        }
        
        if(par=="--test-quaternion-all"){
          std::cout<<"TestQ_all \n";
          FT2.ATT.TestQ=true;
          FT2.ATT.TestQ_all=true;
        }
        



        if(par=="-ATTNomrTolerance"){
          FT2.ATT.NomrTolerance=atof(std::string(argv[i+1]).c_str());
          printf("ATTNomrTolerancen =%e\n", FT2.ATT.NomrTolerance);
        }
        if(par=="-ATTDeltaT_TstartTolerance"){
          FT2.ATT.DeltaT_TstatTolerance=atof(std::string(argv[i+1]).c_str());
          printf("ATTDeltaT_TstartTolerance =%e\n", FT2.ATT.DeltaT_TstatTolerance);
        }
        if(par=="-ORBDeltaT_TstartTolerance"){
          FT2.ORB.DeltaT_TstatTolerance=atof(std::string(argv[i+1]).c_str());
          printf("ORBDeltaT_TstartTolerance =%e\n", FT2.ORB.DeltaT_TstatTolerance);
        }
        if(par=="-FT2_BIN_WIDTH"){
          FT2.FT2_BIN_WIDTH=atof(std::string(argv[i+1]).c_str());
          printf("FT2_BIN_WIDTH =%e\n", FT2.FT2_BIN_WIDTH);
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


unsigned long FT2::Get_Run_ID(std::string GapsRun){
  std::cout<<"Run String "<< GapsRun<<"\n";
  GapsRun.erase(0, 1);
  std::cout<<"Run String "<< GapsRun<<"\n";
  unsigned long Run=std::strtoul(GapsRun.c_str(), NULL, 10);
  return Run;
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



// -------------------------DEG RAD CONVERSION----------------------------------    
double Rad2Deg (double Angle) {
  static double ratio = 180.0 / 3.141592653589793238;
  return Angle * ratio;
}

double Deg2Rad (double Angle) {
  static double ratio =  3.141592653589793238/180.0;
  return Angle * ratio;
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
// ! Approximates an arbitrary function using parabolic least squares fitting.

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
// wrapper of the code from gtbary,

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
void ATTITUDE::Eval_w(ATTITUDE &Att, unsigned int entry){
  if(Att.TestQ){
    Att.CorrectAndEval_w(Att, entry);
  }else{
    Att.CheckAndEval_w(Att, entry);
  }
}

//Check and Correct W
void ATTITUDE::CorrectAndEval_w(ATTITUDE &Att, unsigned int i){
  double aw, vec_norm, delta;
  printf("Eeval Norm and correct\n");
  vec_norm=Att.Eval_VecNorm(Att, i);
  delta=vec_norm-1.0;
  printf("Test Q4, delta=%e vec_norm=%e \n",delta,vec_norm);
  //check the normalization of the quaternion
  if (delta>=0.0){
    double norm=sqrt(vec_norm);
    Att.x[i]=Att.x[i]/norm;
    Att.y[i]=Att.y[i]/norm;
    Att.z[i]=Att.z[i]/norm;
    aw=0;
    if(delta>Att.NomrTolerance){
      printf("!!!Warning ATT vector normalization: W !FORCED! to 0, vec_norm was= %15e\n", vec_norm);
    }
    
    Att.interp_flag[i]=4;
  }else{
    printf("Normalization is good did not renormalized\n");
    aw=sqrt(1.0-vec_norm);
    //Sign Correction on the scalar component
    //if(Att.w[i]<0){
    //  aw=-aw;
    //}
  }
  Att.w[i]=aw;
}

//Only check but don't Correct W
void ATTITUDE::CheckAndEval_w(ATTITUDE &Att, unsigned int i){
  printf("Eeval Norm and Don't Correct\n");
  double aw, vec_norm;
  vec_norm=Att.Eval_VecNorm(Att, i);
  if ((vec_norm-1.0)>=0.0){
    printf("!!!Warning ATT vector normalization: W !NOT FORCED! to 0, will be NAN!!!, vec_norm was= %15.15e\n", vec_norm);
    Att.interp_flag[i]=5;
  }
  aw=sqrt(1.0-vec_norm);
  
  //Sign Correction on the scalar component
  //if(Att.w[i]<0){
  //  aw=-aw;
  // }
  Att.w[i]=aw;
}


double ATTITUDE::Eval_VecNorm(ATTITUDE &Att, unsigned int i){
  using namespace astro;
  using CLHEP::Hep3Vector;
  
  double ax2, ay2, az2, vnorm, delta;
  ax2=Att.x[i]*Att.x[i];
  ay2=Att.y[i]*Att.y[i];
  az2=Att.z[i]*Att.z[i];
  vnorm=ax2+ay2+az2;
  
  delta=vnorm-1.0;
  
  //Quaternion q_test(Hep3Vector(ax2,ay2,az2));
  
  //std::cout<<"scalar"<<q_test.scalar()<<"\n";
  //std::cout<<"norm"<<q_test.norm()<<"\n";
  //std::cout<<"norm"<<q_test.sgn()<<"\n";
  
  //std::cout<<"vnorm"<<vnorm<<"\n";
  
  if ( (vnorm-1.0)>=0.0){
    if(delta>Att.NomrTolerance){
      printf("!!!Warning ATT vector normalization greater then 1.0 and exceed the tolerance : Entry=%d vec_norm>1 vec_norm=%15.15e\n", i, vnorm);
    }else{
      printf("!!!Warning ATT vector normalization greater then 1.0 but did not exceed tolerance : Entry=%d vec_norm>1 vec_norm=%15.15e\n", i, vnorm);
    }
  }
  return vnorm;
}



