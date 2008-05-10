//--------------------------- ROOT DECLARATIONS ---------------------------------

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


void FT2::Gleam_FT2(FT2 &FT2){

   double Tstart_Run, Tstop_Run;
   
   Tstart_Run=FT2.GleamDigiTstart;
   Tstop_Run=FT2.GleamDigiTstop;
 
   

  //-------- WORK ON  M7 FILE ---------------------------------------------------------
  std::cout<<"M-7 file"<<std::endl;
  //Read M-7 File and Set FT2 Entries
  double M7padding=2.0;

  FT2.Set_M7_Entries(FT2,Tstart_Run-M7padding,Tstop_Run+M7padding); 
  std::cout<<"======================================================"<<std::endl;
  
  unsigned int FT2_Entries=Get_FT2_Entries(FT2);
  
  printf("Padding Time=%e\n",M7padding);
  printf("Time of the firtst Digi evt %20.18g\n", Tstart_Run);
  printf("Tstart of the the M7 file   %20.18g\n", FT2.FT2_T.Tstart[0]);
  
  
  printf("Time of the last Digi evt %20.18g\n", Tstop_Run);
  printf("Tstop of the the M7       %20.18g\n", FT2.FT2_T.Tstop[FT2_Entries-1]);
  
  //printf("Tstart RUN=%30.28g  Tstop RUN=%30.28g\n", Tstart_Run, Tstop_Run);
  printf("---------------------------------------------------\n");
 
  //-----------------------------------------------------------------------------------
 
   if(FT2.verbose){
    for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){ 
      printf("%d Tstart=%20.18g  Tstop=%20.18g bin=%d \n",i,FT2.FT2_T.Tstart[i],FT2.FT2_T.Tstop[i],FT2.FT2_T.bin[i]);
    }
  }

//--------------------------------------------------------------------
printf("------------ Merging ----------------------------------------------\n");
  bool redo=false;
  FT2.Merge_M7_Digi_Entries(FT2, Tstart_Run-M7padding, Tstop_Run+M7padding , redo);
  if(redo){
    redo=false;
    printf("Re Merge to cut rigth at Digi boundaries\n");
    FT2.Merge_M7_Digi_Entries(FT2, Tstart_Run-M7padding , Tstop_Run+M7padding, redo);
  }

  //-------- Fill the M7 Entries ---------------------------------------
  FT2.Fill_M7_Entries(FT2,Tstart_Run-M7padding,Tstop_Run+M7padding);
  if(FT2.verbose){
    FT2.ATT.Print_ATT_Entries(FT2.ATT);
    FT2.ORB.Print_ORB_Entries(FT2.ORB);
  }
  //--------------------------------------------------------------------
    
  
  //-------- Fill the SC Entries ---------------------------------------
  FT2.Fill_SC_Entries(FT2);
  //--------------------------------------------------------------------
  

  //-------- Set LiveTime 0 -------------------------------------------
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  for(unsigned int i=0;i<FT2_ENTR;i++){
    FT2.FT2_T.LiveTime[i]=0;
  }
   
  printf("write txt file\n");
  //Write FT2 Txt file
  FT2.WriteMergedTextFile(FT2);
   
  //Write FT2 fits file 
  FT2.WriteFitsFileMerged(FT2);

}
