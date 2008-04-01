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
  
  if (iargc < 8) {
    std::cout << usage;
    std::exit(0);
  }   else  {
    int i;
    std::cout<<"command line"<<std::endl;
    for( i = 1; i < iargc; i++ ){
      std::string par = argv[i];
      if (argv[i][0] == '-'){
        std::cout<<argv[i]<<std::endl;
        if(par=="-DigiFile") FT2.DigiFile = std::string(argv[i+1]);
        if(par=="-MeritFile") FT2.MeritFile = std::string(argv[i+1]);
        if(par=="-M7File") FT2.M7File = std::string(argv[i+1]);
        if(par=="-FT2_txt_File") FT2.FT2_txt_File = std::string(argv[i+1]);
        if(par=="-FT2_fits_File") FT2.FT2_fits_File= std::string(argv[i+1]);
        if(par=="-Gaps_File"){
          FT2.DigiGAPS=true;
          FT2.Gaps_File= std::string(argv[i+1]);
        }
        if(par=="-Version")	  FT2.Version= std::string(argv[i+1]);
        
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
        
      }
    }
    std::cout<<"Digi File="
    <<FT2.DigiFile<<std::endl
    <<"MeritFile="
    <<FT2.MeritFile<<std::endl
    <<"M7 File="
    <<FT2.M7File<<std::endl
    <<"OUT FILE="
    <<FT2.FT2_txt_File<<std::endl
    <<"FITS FILE="
    <<FT2.FT2_fits_File<<std::endl
    <<"Gleam="
    <<FT2.Gleam<<std::endl
    <<"Gaps File="
    <<FT2.Gaps_File<<std::endl
    <<"---------------------------------------------------------"
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
    if((time<=FT2.FT2_T.Tstop[l])&&(time>=FT2.FT2_T.Tstart[l]) && !stop){
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