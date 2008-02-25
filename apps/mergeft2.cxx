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
#include "ft2Util/MergeFT2.hpp"


void getFileNames(int iargc, char * argv[],std::string & FT2File,
		  std::string & fitsFile, std::string & Version) {
  const char usage[] = 
    "usage: mergeFT2Entries.exe  [OPTIONS]\n" 
    " -FT2_txt_File <FileName>\n" 
    " -FT2_fits_File <FileName> \n"
    " -Version <vesion of the file>\n"
    " -h --help\n";
  
  if (iargc < 5) {
    std::cout << usage;
    std::exit(0);
  } 
  else{
    int i;
    std::cout<<"command line"<<std::endl;
    for( i = 1; i < iargc; i++ ){
      std::string par = argv[i];
      if(par=="-FT2_txt_File" ) FT2File  = std::string(argv[i+1]);
      if(par=="-FT2_fits_File") fitsFile = std::string(argv[i+1]);
      if(par=="-Version")       Version= std::string(argv[i+1]);
      if(par=="-h"){ 
	std::cout << usage;
	std::exit(0);
	}
      if(par=="--help"){
	std::cout << usage;
	std::exit(0);  
      }
      }

    std::cout<<"FT2 FILE"
	     <<FT2File
	     <<" FITS FILE"
	     <<fitsFile
	     <<"Version of file"
	     <<Version
	     <<std::endl;
  }
}

  


int main(int iargc, char **argv){
  try { 
    
    FT2 FT2;
    
    printf("v1r1p41\n");
    
    //File Handlign
    std::string FT2File;
    std::string fitsFile;
    std::string line,comment;
    std::string Version;

    double time, t_stop,time_elapsed(0),live_time(0);
    unsigned int Entries(0);
    bool first_bin(true), new_entry(false);
    bool in_saa;
    
    getFileNames(iargc, argv, FT2File,fitsFile, Version);
    
    std::ifstream inFT2File(FT2File.c_str());

    FT2.Version= Version;
 
    //Read FT2 File 
    while (std::getline(inFT2File, line, '\n')) {

      

      //A SIMPLE TOKENIZER
      std::vector<std::string> tokens; // Create vector to hold our words
      std::string delimiters=",";
      // skip delimiters at beginning.
      std::string::size_type lastPos = line.find_first_not_of(delimiters, 0);
      // find first "non-delimiter".
      std::string::size_type pos = line.find_first_of(delimiters, lastPos);
      while (std::string::npos != pos || std::string::npos != lastPos)
    	{
	  // found a token, add it to the vector.
	  tokens.push_back(line.substr(lastPos, pos - lastPos));
	  
	  // skip delimiters.  Note the "not_of"
	  lastPos = line.find_first_not_of(delimiters, pos);
	  
	  // find next "non-delimiter"
	  pos = line.find_first_of(delimiters, lastPos);
    	}


      //SKIP lines that start with # character
      comment=line.substr(0,1);
      if(comment.find( "#", 0) == std::string::npos ){
	
	if(first_bin||new_entry){
	  
	  first_bin=false;
	  new_entry=false;
	  
	  Entries++;
	  
	  //printf("new entries, entr=%d\n",Entries);
	  //--------------------!!!!!!! Very Important ------------------
	  //Fills arrayes from FT2txt file 
	  //!!!Attention the fields MUST be put in as
	  //expected from the WriteFitsFile Method
	  //This implies that SC.x SC.y and SC.z must
	  //converted to the ORB.x ect.. format i.e. im Km
	  //and The in_saa entry MUST be coverted
	  //to ORB.SAA with 0 false and 1 true
	  //--------------------------------------------------------------
	  
	  //update arrays size
	  FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,Entries);
	  FT2.Update_FT2_Entries(FT2,Entries);
	  FT2.ORB.Set_ORB_Size(FT2.ORB,Entries);
	  FT2.FT2_SC.Set_FT2SC_Size(FT2.FT2_SC,Entries);
	  
	  //printf("arrays resized\n");

	 
	  //store values
	  FT2.FT2_T.Tstart[Entries-1]=atof(tokens[1].c_str());
	  //printf("sono qui\n");
	  FT2.ORB.x[Entries-1]=atof(tokens[3].c_str());
	  //std::cout<<tokens[3]<<" ";
	  FT2.ORB.y[Entries-1]=atof(tokens[4].c_str());
	  //std::cout<<tokens[4]<<" ";
	  FT2.ORB.z[Entries-1]=atof(tokens[5].c_str());
	  //std::cout<<tokens[5]<<" ";
	  FT2.FT2_SC.RA_SCZ[Entries-1]=atof(tokens[6].c_str());
	  //std::cout<<tokens[6]<<" ";
	  FT2.FT2_SC.DEC_SCZ[Entries-1]=atof(tokens[7].c_str());
	  //std::cout<<tokens[7]<<" ";
	  FT2.FT2_SC.RA_SCX[Entries-1]=atof(tokens[8].c_str());
	  //std::cout<<tokens[8]<<" ";
	  FT2.FT2_SC.DEC_SCX[Entries-1]=atof(tokens[9].c_str());
	  //std::cout<<tokens[9]<<" ";
	  FT2.FT2_SC.RA_ZENITH[Entries-1]=atof(tokens[10].c_str());
	  //std::cout<<tokens[10]<<" ";
	  FT2.FT2_SC.DEC_ZENITH[Entries-1]=atof(tokens[11].c_str());
	  //std::cout<<tokens[11]<<std::endl;
	  FT2.FT2_SC.LON_GEO[Entries-1]=atof(tokens[12].c_str());
	  FT2.FT2_SC.LAT_GEO[Entries-1]=atof(tokens[13].c_str());
	  FT2.FT2_SC.RAD_GEO[Entries-1]=atof(tokens[14].c_str());
	  FT2.FT2_SC.GEOMAG_LAT[Entries-1]=atof(tokens[15].c_str());
	  if(tokens[16]=="true"){
	    FT2.ORB.SAA[Entries-1]==1;
	  }else{
	    FT2.ORB.SAA[Entries-1]==0;
	  }
	  FT2.FT2_SC.B_MCILWAIN[Entries-1]=atof(tokens[17].c_str());
	  FT2.FT2_SC.L_MCILWAIN[Entries-1]=atof(tokens[18].c_str());
	  FT2.ORB.CM[Entries-1]=atoi(tokens[19].c_str());
    	
	}
	//std::cout<<"tokens"
	//	 <<tokens[18]
	//	 <<" "
	//	 <<tokens[19]
	//		 <<" "
	//	 <<tokens[20]
	//	 <<std::endl;
	live_time+=atof(tokens[20].c_str());
	//printf("live time=%e\n",live_time);
	time_elapsed+=(atof(tokens[2].c_str())-atof(tokens[1].c_str()));
	
	if(time_elapsed>=30.0){ 
	  FT2.FT2_T.Tstop[Entries-1]=atof(tokens[2].c_str());
	  //store t_stop
	  //store live_time
	  //add en element to the arrays
	  FT2.FT2_T.LiveTime[Entries-1]=live_time;
	  new_entry=true;
	  live_time=0;
	  time_elapsed=0;
	}    
	
	t_stop=atof(tokens[2].c_str());
      }
    }
    if(new_entry==false){
      FT2.FT2_T.Tstop[Entries-1]=t_stop;
      //store t_stop
      //store live_time
      //add en element to the arrays
      FT2.FT2_T.LiveTime[Entries-1]=live_time;
      new_entry=true;
      live_time=0;
      time_elapsed=0;
    }
    
    inFT2File.close();

    
    //Write Fits File
    FT2.FT2_fits_File=fitsFile;
    FT2.WriteFitsFileMerged(FT2);
 
  }catch (std::exception & eObj) {
    std::cout << eObj.what() << std::endl;
    std::exit(1);
  }
}
