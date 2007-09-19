// stl/c++ headers
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>

// headers for this package
#include "ft2Util/FT2_Time.hpp"

// headers for exceptions
#include <stdexcept>


void FT2::Gleam_FT2(FT2 &FT2){
 

  //-------- WORK ON  M7 FILE ---------------------------------------------------------
  std::cout<<"M-7 file"<<std::endl;
  //Read M-7 File and Set FT2 Entries
  FT2.Set_M7_Entries(FT2); 
  std::cout<<"======================================================"<<std::endl;
  //-----------------------------------------------------------------------------------
 
  
  

  //-------- Fill the M7 Entries ---------------------------------------
  FT2.Fill_M7_Entries(FT2);
  FT2.ATT.Print_ATT_Entries(FT2.ATT);
  FT2.ORB.Print_ORB_Entries(FT2.ORB);
  //--------------------------------------------------------------------
    
  
  //-------- Fill the SC Entries ---------------------------------------
  FT2.Fill_SC_Entries(FT2);
  //--------------------------------------------------------------------
  

  //-------- Set LiveTime 0 -------------------------------------------
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  for(unsigned int i=0;i<FT2_ENTR;i++){
    FT2.FT2_T.LiveTime[i]=0;
  }
   
  
  //Write FT2 Txt file
  FT2.WriteTextFile(FT2);
  
  //Write FT2 fits file
  FT2.WriteFitsFile(FT2);

}
