/** @file FT2_Time.cxx
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

*/



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

// headers for exceptions
#include <stdexcept>

 




int main(int argc, char **argv){
    
  try {
    //--------------------------- ROOT DECLARATIONS ---------------------------------
   



 
    //------ MAKE FT2 FILES ----------
    //This is the main class
    FT2 FT2;
    //--------------------------------
    
    
    //----------Operations for file I/O ------------------------------------------
    //--- strings for files ---
    //std::string MeritFile;
    //std::string DigiFile;
    //std::string M7File;
    //std::string FT2_txt_File;
    //std::string FT2_fits_File;
    

    printf("v1r1p35\n");
    
    //--- Get File Names ---

    FT2.Gleam=false;
    FT2.MC=false;
    FT2.verbose=false;
    FT2.getFileNames(argc, argv, FT2);
    
    std::cout<<"Digi File " 
	     <<FT2.DigiFile<<std::endl
	     <<"MeritFile"
	     <<FT2.MeritFile<<std::endl
	     <<"M7 File "
	     <<FT2.M7File<<std::endl
	     <<"OUT FILE"
	     <<FT2.FT2_txt_File
	     <<" FITS FILE"
	     <<FT2.FT2_fits_File
	     <<std::endl;
    
    if(FT2.Gleam==true){
      FT2.Gleam_FT2(FT2);
    }
    else if(FT2.MC==true){
      FT2.DigiMC_FT2(FT2);
    }else FT2.Digi_FT2(FT2);
      
  } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      std::exit(1);
  }
}


