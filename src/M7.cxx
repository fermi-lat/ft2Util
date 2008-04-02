/** @file M7.cxx
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

//--------------------------------Set Entries According to M7 file -----------------------
void FT2::Set_M7_Entries(FT2 &FT2){
  
  double time, Tstart;
  //int new_entry(1);
  unsigned int M7LineCounter(0);
  int  TimeBin(0), NewTimeBin(0), OldTimeBin(0), MODE(0), OLD_MODE;
  bool first_orb_entry(true);
  unsigned int Current_FT2_Entries(0);
  
  //File Handlign
  std::string buf, comment; //buffer string
  std::string line;
  std::ifstream M7F;
  
  M7F.open(FT2.M7File.c_str());
  
  if(M7F.fail()) {
    std::cout<<"The file *** \'"
    <<FT2.M7File.c_str()
    <<"\' *** could not be opened!\n"
    <<"Abnormal Exit !!!\n";
    exit(1);
  }
  
  printf("----------- Set FT2 Entries from M7 file -----------\n");
  
  
  //Read M-7 File
  while (std::getline(M7F, line, '\n')) {
    //A SIMPLE TOKENIZER
    std::vector<std::string> tokens; // Create vector to hold our words
    std::stringstream ss(line); // Insert the string into a stream
    while (ss >> buf)	tokens.push_back(buf);
    
    //SKIP lines that start with # character
    comment=line.substr(0, 1);
    if(comment.find( "#", 0) == std::string::npos ){
      
      time=FT2.Get_M7_Time(tokens[3], tokens[4]);
      
      
      if(FT2.verbose){
        std::cout<<"Time="
        <<std::setprecision(20)
        <<time
        <<" lable"
        <<tokens[2]
        <<std::endl;
        
      }
      
      if (M7LineCounter==0){
        Tstart=time;
      }
      
      //-------FT2 time bin Id-----------------
      TimeBin=FT2.Get_FT2_Time_Bin(time, Tstart);
      
      
      //------------ Read MODE from ORB Entry ----------------------
      if (tokens[2]=="ORB"){
        MODE=atoi(tokens[11].c_str());
        //std::cout<<"Mode "<<MODE<<"   OLD Mode "<<OLD_MODE<<"\n";
        if(first_orb_entry){
          first_orb_entry=false;
          OLD_MODE=MODE;
        }
        
        //------------ Make a new Entry if MODE Changes ----------------------
        if (MODE!=OLD_MODE){
          
          NewTimeBin=1;
          
          if(FT2.verbose){
            printf("---------------------------------------------------------\n");
            std::cout<<"New Entry due to Changed Mode form "
            <<OLD_MODE
            <<" to "
            <<MODE
            <<"\n";
            std::cout<<"Previous Entry Time Id "
            <<std::setprecision(20)
            <<FT2.FT2_T.bin[Current_FT2_Entries-1]
            <<" FT2 Tstart "
            <<FT2.FT2_T.Tstart[Current_FT2_Entries-1]
            <<" FT2 Tstop "
            <<FT2.FT2_T.Tstop[Current_FT2_Entries-1]
            <<std::endl;
          }
          
        }
      }
      
      
      //------------ Make a new Entry if Time Bin Changes ----------------------
      if(TimeBin!=OldTimeBin && M7LineCounter>0){
        
        NewTimeBin=1;
        
        //-----Put  empity entries if Delta_ID>1--------//
        bool backjump=false;
        if((TimeBin-OldTimeBin)<0) backjump=true;
        
        unsigned int Delta_Gap=(TimeBin-OldTimeBin);
        if(Delta_Gap>1 && !backjump){
          printf("---------------------------------------------------------\n");
          printf("Found Gap, Delta_Gap=%d\n", Delta_Gap);
          printf("Time=%20.20e Tstop previous entry=%20.20e\n", time, FT2.FT2_T.Tstop[Current_FT2_Entries-2]);
          for(unsigned int i=1;i<Delta_Gap;i++){
            if(FT2.verbose){
              printf("---------------------------------------------------------\n");
              printf("add an empty entry in the Gap N=%d\n", i);
              std::cout<<"!!!GAP Previous Entry Time Id "
              <<std::setprecision(20)
              <<FT2.FT2_T.bin[Current_FT2_Entries-1]
              <<" FT2 Tstart "
              <<FT2.FT2_T.Tstart[Current_FT2_Entries-1]
              <<" FT2 Tstop "
              <<FT2.FT2_T.Tstop[Current_FT2_Entries-1]
              <<std::endl;
            }
            
            
            //UPADTE THE NUMBER OF ENTRIES IN THE FT2 CLASS
            Current_FT2_Entries++;
            FT2.Update_FT2_Entries(FT2, Current_FT2_Entries);
            
            //Resize FT2_Time class
            FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, Current_FT2_Entries);
            FT2.FT2_T.bin[Current_FT2_Entries-1]=TimeBin-(Delta_Gap)+i;
            
            //Update FT2_Time class
            FT2.FT2_T.Tstart[Current_FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entries-2];
            FT2.FT2_T.Tstop[Current_FT2_Entries-1]=FT2.FT2_T.Tstart[Current_FT2_Entries-1]+FT2_BIN_WIDTH;
            
            
            
            
            
            if(FT2.verbose){
              std::cout<<"New Entry, Current number of  FT2 Entries "
              <<Current_FT2_Entries
              <<" Current Entry Id"
              <<FT2.FT2_T.bin[Current_FT2_Entries-1]
              <<" M7 line "
              <<M7LineCounter
              <<std::endl;
              std::cout<<Tstart <<" +"<<time<<std::endl;
              
            }
          }
        }
        if(backjump){
          printf("!!!!Back Jump = %d , M7 entries are not time ordered I will not add extra entries here\n", TimeBin-OldTimeBin);
        }
        //---------------------------------------------------------
        
        if(FT2.verbose){
          printf("---------------------------------------------------------\n");
          std::cout<<"Previous Entry Time Id "
          <<std::setprecision(20)
          <<FT2.FT2_T.bin[Current_FT2_Entries-1]
          <<" FT2 Tstart "
          <<FT2.FT2_T.Tstart[Current_FT2_Entries-1]
          <<" FT2 Tstop "
          <<FT2.FT2_T.Tstop[Current_FT2_Entries-1]
          <<std::endl;
        }
      }
      
      //IF we are at the beginning of a new TimeBin or
      //at the start UpadteSomeThing
      if((M7LineCounter==0)||(NewTimeBin)){
        
        
        
        //UPADTE THE NUMBER OF ENTRIES IN THE FT2 CLASS
        Current_FT2_Entries++;
        FT2.Update_FT2_Entries(FT2, Current_FT2_Entries);
        
        
        //Resize FT2_Time class
        FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, Current_FT2_Entries);
        FT2.FT2_T.bin[Current_FT2_Entries-1]=TimeBin;
        
        //Update FT2_Time class
        if(M7LineCounter==0){
          FT2.FT2_T.Tstart[Current_FT2_Entries-1]=Tstart;
          
          FT2.FT2_T.Tstop[Current_FT2_Entries-1]=Tstart+FT2_BIN_WIDTH;
          
        }
        else{
          //!!!!!!!! IF there is some missing data
          //that makes the following entry distant
          //in time more than one entry time span
          //than tacke Tstart not form previous entry
          //Tstop but from current M7 file time
          //printf("(TimeBin-OldTimeBin=%d\n",TimeBin-OldTimeBin);
          
          //if(TimeBin-OldTimeBin<=1){
          FT2.FT2_T.Tstart[Current_FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entries-2];
          //}else{
          //FT2.FT2_T.Tstart[Current_FT2_Entries-1]=time;
          ///	}
          
          FT2.FT2_T.Tstop[Current_FT2_Entries-1]=FT2.FT2_T.Tstart[Current_FT2_Entries-1]+FT2_BIN_WIDTH;
        }
        
        
        
        
        //Live Time set to zero!!!
        FT2.FT2_T.LiveTime[Current_FT2_Entries-1]=0;
        
        NewTimeBin=0;
        if(FT2.verbose){
          std::cout<<"New Entry, Current number of  FT2 Entries "
          <<Current_FT2_Entries
          <<" Current Entry Id"
          <<FT2.FT2_T.bin[Current_FT2_Entries-1]
          <<" M7 line "
          <<M7LineCounter
          <<std::endl;
          std::cout<<Tstart <<" +"<<time<<std::endl;
        }
        
        
      }
      else{
        //FT2.FT2_T.Tstop[Current_FT2_Entries-1]=time;
      }
      
      //SWAPPING
      OldTimeBin=TimeBin;
      OLD_MODE=MODE;
      
      
      M7LineCounter++;
    }
  }
  
  M7F.close();
  
  if(FT2.verbose){
    for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){
      printf("%d Tstart=%20.18g  Tstop=%20.18g bin=%d \n", i, FT2.FT2_T.Tstart[i], FT2.FT2_T.Tstop[i], FT2.FT2_T.bin[i]);
    }
  }
  
}



//--------------  Fill M7 Entries from M7 File ----------------------------------
void FT2::Fill_M7_Entries(FT2 &FT2){
  
  double time, Tstart;
  //int new_entry(1);
  unsigned int M7LineCounter(0);
  bool  NewEntry(false);
  unsigned int FT2_Entries(0), Current_FT2_Entry(0), Old_FT2_Entry(0);
  
  //File Handlign
  std::string buf, comment; //buffer string
  std::string line;
  std::ifstream M7F(FT2.M7File.c_str());
  
  printf("---------------------------------------------------------\n");
  printf("----------- Fill M7 Entries from  M7 file -----------\n");
  
  
  //Resize ATT & ORB
  FT2_Entries=Get_FT2_Entries(FT2);
  FT2.ATT.Set_ATT_Size(FT2.ATT, FT2_Entries);
  FT2.ORB.Set_ORB_Size(FT2.ORB, FT2_Entries);
  
  
  //Read M-7 File
  while (std::getline(M7F, line, '\n')) {
    //A SIMPLE TOKENIZER
    std::vector<std::string> tokens; // Create vector to hold our words
    std::stringstream ss(line); // Insert the string into a stream
    while (ss >> buf)	tokens.push_back(buf);
    
    //SKIP lines that start with # character
    comment=line.substr(0, 1);
    if(comment.find( "#", 0) == std::string::npos ){
      
      
      
      time=FT2.Get_M7_Time(tokens[3], tokens[4]);
      
      
      if (M7LineCounter==0){
        Tstart=time;
      }
      
      
      
      //FT2 Entry Index
      Get_FT2_Entry_Index(FT2, time, Current_FT2_Entry);
      
      if(Current_FT2_Entry!=Old_FT2_Entry && M7LineCounter>0){
        NewEntry=true;
        if(FT2.verbose){
          std::cout
          <<"-------------------------------------------------\n"
          <<"M7 time"
          <<time
          <<"Current Entry "
          <<Current_FT2_Entry
          <<" Current Entry Id "
          <<std::setprecision(20)
          <<FT2.FT2_T.bin[Current_FT2_Entry]
          <<" FT2 Tstart "
          <<FT2.FT2_T.Tstart[Current_FT2_Entry]
          <<" FT2 Tstop "
          <<FT2.FT2_T.Tstop[Current_FT2_Entry]
          <<"--------------------------------------------------"
          <<std::endl;
        }
      }
      //IF we are at the beginning of a new TimeBin or
      //at the start UpadteSomeThing
      if((M7LineCounter==0)||(NewEntry)){
        NewEntry=false;
        FT2.Clean_ATT_Quaternions(FT2.ATT, Current_FT2_Entry);
        FT2.Clean_ORB(FT2.ORB, Current_FT2_Entry);
        
      }
      
      
      if (tokens[2]=="ATT"){
        FT2.Update_ATT_Quaternions(FT2.ATT, tokens, Current_FT2_Entry);
      }
      if (tokens[2]=="ORB"){
        FT2.Update_ORB(FT2.ORB, tokens, Current_FT2_Entry, FT2.FT2_T.Tstart[Current_FT2_Entry]);
      }
      
      
      //SWAPPING
      Old_FT2_Entry=Current_FT2_Entry;
      
      
      M7LineCounter++;
      
    }
  }
  
  
  
  M7F.close();
  
  
  if(FT2.verbose){
    for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){
      printf("%d Tstart=%20.18g  Tstop=%20.18g bin=%d \n", i, FT2.FT2_T.Tstart[i], FT2.FT2_T.Tstop[i], FT2.FT2_T.bin[i]);
    }
  }
  
  ///!!!!!!!!MAY BE THAT WILL BE NO MOER USED
  ///FT2.Interp_ORB_Vel_Entries(FT2);
  
  FT2.Interp_ORB_Entries(FT2);
  FT2.Interp_ORB_Tstart(FT2);
  
  FT2.Interp_ATT_Entries(FT2);
  FT2.Interp_ATT_Tstart(FT2);
  
  if(FT2.verbose){
    printf("---------------------------------------------------------\n");
  }
}



//------------------------------ Merge M7 and Digi Entries --------------------------------------------
/*---------------------------------------------------------------------------
 *             MERGE M7 and DIGI ENTRIES
 *Merge The M7 entries with the entries in the Digi
 *corresponding to the first and last event
 *It cheks if the last and first DIGI entry actually fall inside
 *the FT2 time spane, otherwise it adds entry according
 *THIS SHOULD NEVER HAPPEN!!!!!!!!!!!!!!!!
 *-------------------------------------------------------------------------*/
void FT2::Merge_M7_Digi_Entries(FT2 &FT2, double Tstart_Run , double Tstop_Run){
  
  unsigned int Current_FT2_Entry, FT2_Entries;
  
  //!!!!padding for new bins
  double padding=1.0e-5;
  
  printf("---------------------------------------------------------\n");
  printf("Merge M7 with Digi entries -\n");
  
  printf("FT2 entries %d \n", Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2, Tstart_Run, Current_FT2_Entry);
  if(!FT2.Get_OutOfRange(FT2)){
    printf("Entry index=%d \n", Current_FT2_Entry);
    FT2.Update_FT2_Entries(FT2, Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, FT2_Entries);
    printf("add entry, FT2 entries %d \n", Get_FT2_Entries(FT2));
    
    //FT2.FT2_T.Tstart[Current_FT2_Entry]=FT2.FT2_T.Tstart[Current_FT2_Entry];
    FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entry];
    FT2.FT2_T.Tstop[Current_FT2_Entry]=Tstart_Run ;
    FT2.FT2_T.Tstart[FT2_Entries-1]=Tstart_Run ;
  }else{
    printf("Adding entries before that M7 file starts");
    FT2.Update_FT2_Entries(FT2, Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, FT2_Entries);
    printf("add entry, FT2 entries %d \n", Get_FT2_Entries(FT2));
    printf("Entry index=0\n");
    //Here add the padding!!!
    FT2.FT2_T.Tstart[FT2_Entries-1]=Tstart_Run-padding;
    FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstart[0];
  }
  
  
  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());
  
  
  //add new entry
  printf("FT2 entries %d \n", Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2, Tstop_Run, Current_FT2_Entry);
  if(!FT2.Get_OutOfRange(FT2)){
    printf("Entry index=%d \n", Current_FT2_Entry);
    FT2.Update_FT2_Entries(FT2, Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, FT2_Entries);
    printf("add entry, FT2 entries %d \n", Get_FT2_Entries(FT2));
    
    //FT2.FT2_T.Tstart[Current_FT2_Entry]=FT2.FT2_T.Tstart[Current_FT2_Entry];
    FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entry];
    FT2.FT2_T.Tstop[Current_FT2_Entry]=Tstop_Run ;
    FT2.FT2_T.Tstart[FT2_Entries-1]=Tstop_Run ;
  }else{
    printf("Adding entries after that M7 file ends");
    FT2.Update_FT2_Entries(FT2, Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, FT2_Entries);
    printf("add entry, FT2 entries %d \n", Get_FT2_Entries(FT2));
    printf("Entry index=%d \n", FT2_Entries-1);
    FT2.FT2_T.Tstart[FT2_Entries-1]=FT2.FT2_T.Tstop[FT2_Entries-2] ;
    //Here add the padding!!!
    FT2.FT2_T.Tstop[FT2_Entries-1]=Tstop_Run+padding;
  }
  
  
  
  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());
  
  //SORTING
  //
  printf("---------------------------------------------------------\n");
  
  printf("---------------------------------------------------------\n");
  if(FT2.verbose){
    for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){
      FT2.FT2_T.LiveTime[i]=0;
      FT2.FT2_T.bin[i]=i;
      printf("Entry Id=%d Tstart=%20.18g  Tstop=%20.18g\n", i, FT2.FT2_T.Tstart[i], FT2.FT2_T.Tstop[i]);
    }
    printf("---------------------------------------------------------\n");
  }
}