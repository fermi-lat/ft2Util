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
  unsigned int Current_FT2_Entry,FT2_Entries;

  //-------- WORK ON  M7 FILE ---------------------------------------------------------
  std::cout<<"M-7 file"<<std::endl;
  //Read M-7 File and Set FT2 Entries
  FT2.Set_M7_Entries(FT2); 
  std::cout<<"======================================================"<<std::endl;
  //-----------------------------------------------------------------------------------
 
   if(FT2.verbose){
    for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){ 
      printf("%d Tstart=%20.18g  Tstop=%20.18g bin=%d \n",i,FT2.FT2_T.Tstart[i],FT2.FT2_T.Tstop[i],FT2.FT2_T.bin[i]);
    }
  }
  
  //-------- Add fake extra Entries at end and beginning ---------------
  printf("Adding entries before that M7 file starts");
  printf("FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
  FT2_Entries= Get_FT2_Entries(FT2);
  FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
  printf("added entry, FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.FT2_T.Tstart[FT2_Entries-1]=FT2.FT2_T.Tstart[0]-1.0;
  FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstart[0];
  //!!!!-1 flag added entries
  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());
  //FT2.FT2_T.bin[0]=-1;
  
  printf("Adding entries after that M7 file ends"); 
  printf("FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
  FT2_Entries= Get_FT2_Entries(FT2);
  FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
  printf("added entry, FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.FT2_T.Tstart[FT2_Entries-1]=FT2.FT2_T.Tstop[FT2_Entries-2];
  FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[FT2_Entries-2]+1.0;
  //!!!!-1 flag added entrie
  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());
  //FT2.FT2_T.bin[FT2_Entries-1]=-1;
  //!!!! Set the bin entry correct
  for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){ 
    FT2.FT2_T.bin[i]=i;
  }
  if(FT2.verbose){
    for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){ 
      printf("%d Tstart=%20.18g  Tstop=%20.18g bin=%d \n",i,FT2.FT2_T.Tstart[i],FT2.FT2_T.Tstop[i],FT2.FT2_T.bin[i]);
    }
  }

  //--------------------------------------------------------------------


  //-------- Fill the M7 Entries ---------------------------------------
  FT2.Fill_M7_Entries(FT2);
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
   
  
  //Write FT2 Txt file
  FT2.WriteMergedTextFile(FT2);
   
  //Write FT2 fits file 
  FT2.WriteFitsFileMerged(FT2);

}
