/** @file FT2_Time.cxx
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


//---------------- FT2_TIME CLASS----------------------------------
FT2_Time::FT2_Time() {
}

void FT2_Time::Set_FT2Time_Size(FT2_Time &FT2_T, unsigned int size) {
  FT2_T.Tstart.resize(size);
  FT2_T.Tstop.resize(size);
  FT2_T.LiveTime.resize(size);
  FT2_T.bin.resize(size);
}

//double FT2_Time::Get_Tstart( unsigned int  timebin ){
//  return double(timebin)*30.0;
//}

//double FT2_Time::Get_Tstop(  unsigned int  timebin ){
//  return double(timebin)*30.0 + 30.0;
//}

//------------------------------------------------------------
//------------------------------------------------------------

//---------------- DigiTime CLASS ----------------------------------
DigiTime::DigiTime() {
  
}

void DigiTime::Set_DigiTime_Size(DigiTime &DigiT, unsigned long size){
  DigiT.Tstart.resize(size);
  DigiT.Tstop.resize(size);
  DigiT.Tstart_LiveTime.resize(size);
  DigiT.Tstop_LiveTime.resize(size);
  DigiT.LiveTime.resize(size);
  DigiT.DeadTime.resize(size);
  DigiT.ReconDeadTime.resize(size);
  DigiT.RealGapsDeadTime.resize(size);
  DigiT.GapsDeadTime.resize(size);
  DigiT.update.resize(size);
  DigiT.gap.resize(size);
  //DigiT.bin.resize(size);
  //DigiT.Tstart_evt.resize(size);
  //DigiT.Tstop_evt.resize(size);
}
//------------------------------------------------------------
//------------------------------------------------------------


//---------------- Gaps CLASS ----------------------------------
GAPS::GAPS() {
  
}

void GAPS::Set_GAPS_Size(GAPS &GP, unsigned long size){
  GP.GemStart.resize(size);
  GP.GemStop.resize(size);
  GP.Tstart.resize(size);
  GP.Tstop.resize(size);
  GP.LiveTstart.resize(size);
  GP.LiveTstop.resize(size);
}
//------------------------------------------------------------
//------------------------------------------------------------


//-------------------- FT2_CLASS  -----------------------------------------------
FT2::FT2() {
  init();
  
}

void FT2::init(){
  verbose=false;
  DigiGAPS=false;
  Gleam=false;
  MC=false;
  ATT.TestQ=false;
  printf("FT2 initialized\n");
  //Initialize constants to default
  FT2_BIN_WIDTH=1.0;
  FT2_MERGED_BIN_WIDTH=30.0;
  ATT.NomrTolerance=1.0e-5;
  ATT.DeltaT_TstatTolerance=1.0e-5;
  ORB.DeltaT_TstatTolerance=1.0e-5;
}


//--------------  Update the private FT2  Entries ----------------------------
void FT2::Update_FT2_Entries(FT2 &FT2, int i){
  FT2.Entries=i;
}

unsigned int FT2::Get_FT2_Entries(FT2 &FT2){
  return FT2.Entries;
}

void FT2::Set_OutOfRange_TRUE(FT2 &FT2){
  FT2.OutOfRange=true;
}

void FT2::Set_OutOfRange_FALSE(FT2 &FT2){
  FT2.OutOfRange=false;
}

bool FT2::Get_OutOfRange(FT2 &FT2){
  return FT2.OutOfRange;
}

//------------------------------------------------------------
//------------------------------------------------------------



//-------------------EVALUATE LIVE TIME -----------------------
void FT2::Evaluate_Live_Time(FT2 &FT2){
  
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  double fraction;//fraction_tot=0;
  double liveTot_sum(0);
  double liveTot_elaps(0);
  double deadTot(0), deadGapsTot(0), RealdeadTot(0);
  double T1;
  bool first(true);
  for(unsigned int i=0;i<FT2_ENTR;i++){
    
    FT2.DT.DeadTime[i]+=FT2.DT.ReconDeadTime[i]+FT2.DT.GapsDeadTime[i];
    
    deadTot+=FT2.DT.DeadTime[i];
    
    RealdeadTot+=FT2.DT.ReconDeadTime[i]+FT2.DT.RealGapsDeadTime[i];
    
    deadGapsTot+=FT2.DT.GapsDeadTime[i];
    
    //FT2.FT2_T.LiveTime[i]=0;
    if(FT2.verbose)printf("Entry %d \n", i);
    if(FT2.DT.update[i]){
      
      
      if(first) T1=FT2.DT.Tstart_LiveTime[i];
      first=false;
      
      FT2.FT2_T.LiveTime[i]=
              (FT2.DT.Tstop_LiveTime[i] -  FT2.DT.Tstart_LiveTime[i])-
              FT2.DT.DeadTime[i];
      
      if(FT2.verbose){
        
        std::cout<<"upadte "
        << FT2.DT.update[i]
        <<std::endl;
        
        std::cout<<"Tstart "
        <<FT2.FT2_T.Tstart[i]
        <<"\nTstop "
        <<FT2.FT2_T.Tstop[i]
        <<"\nTstart Live "
        <<FT2.DT.Tstart_LiveTime[i]
        <<"\nTstop Live "
        <<FT2.DT.Tstop_LiveTime[i]
        <<"\ndead "
        <<FT2.DT.DeadTime[i]
        <<"\nlive "<<FT2.FT2_T.LiveTime[i]
        <<std::endl;;
      }
      
      
      if(FT2.DT.update[i-1] && i>0){
        fraction=1.0/(FT2.DT.Tstart[i]-FT2.DT.Tstop[i-1]);
        fraction*=(FT2.DT.Tstart[i]-FT2.FT2_T.Tstart[i]);
        fraction*=(FT2.DT.Tstart_LiveTime[i]-FT2.DT.Tstop_LiveTime[i-1]);
        if(FT2.verbose){
          std::cout<<"fraction back "
          <<fraction
          <<"\n";
          printf("DT.Tstart_Live[i]=%30.28e DT.Tstop_Live[i-1]=%30.28e\n", FT2.DT.Tstart_LiveTime[i], FT2.DT.Tstop_LiveTime[i-1]);
        }
        FT2.FT2_T.LiveTime[i]+= fraction;
        //fraction_tot+=fraction;
      }
      
      
      if(FT2.DT.update[i+1] && i<FT2_ENTR-1){
        fraction=(FT2.FT2_T.Tstop[i]-FT2.DT.Tstop[i])/(FT2.DT.Tstart[i+1]-FT2.DT.Tstop[i]);
        fraction*=(FT2.DT.Tstart_LiveTime[i+1]-FT2.DT.Tstop_LiveTime[i]);
        if(FT2.verbose){
          std::cout<<"fraction fwd "
          <<fraction
          <<"\n";
          printf("DT.Tstart_Live[i+1]=%30.28e DT.Tstop_Live[i]=%30.28e\n", FT2.DT.Tstart_LiveTime[i+1], FT2.DT.Tstop_LiveTime[i]);
        }
        FT2.FT2_T.LiveTime[i]+= fraction;
        //fraction_tot+=fraction;
      }
      
      liveTot_sum+=FT2.FT2_T.LiveTime[i]-fraction;
      
      
      
      liveTot_elaps=FT2.DT.Tstop_LiveTime[i]-T1;
      
      if(FT2.verbose){
        printf("i=%d \nLivetime i-0 =%30.28g, summed up to now =%30.28g \n(Livetime i-0 - summed up to now )=%30.28g\n",
                i,
                liveTot_elaps,
                liveTot_sum,
                liveTot_elaps-liveTot_sum);
        
        printf("Real dead up to now=%30.28g, dead subtr up to now=%30.28g, \ndead subtr from gaps up to now=%30.28g\n",
                RealdeadTot,
                deadTot,
                deadGapsTot);
        
        printf("(Livetime i-0 - summed up to now )-(Real dead up to now)=%30.28g\n",
                (liveTot_elaps-liveTot_sum)-RealdeadTot
                );
      }
      
      liveTot_sum+=fraction;
    }
    
    
    if(FT2.verbose){
      std::cout<<"live "
      <<std::setprecision(30)
      <<FT2.FT2_T.LiveTime[i]
      <<std::endl
      <<"-------------------------------------------------------------------------"
      <<std::endl;
    }
    //------ inter bin correction --------
    
  }
  
  
}

//--------------  Set Gaps ----------------------------------
void FT2::Set_GAPS(FT2 &FT2){
  unsigned int Entries(0);
  printf("Set Gaps from file\n");
  //File Handlign
  std::string buf, comment; //buffer string
  std::string line;
  std::ifstream  GPF;
  GPF.open(FT2.Gaps_File.c_str());
  
  if(GPF.fail()) {
    printf(" FT2::Set_GAPS(FT2 &FT2) \n");
    std::cout<<"The file *** \'"
    <<FT2.Gaps_File.c_str()
    <<"\' *** could not be opened!\n"
    <<"Abnormal Exit !!!\n";
    exit(1);
  }
  
  //Read file
  while (std::getline(GPF, line, '\n')) {
    //A SIMPLE TOKENIZER
    std::vector<std::string> tokens; // Create vector to hold our words
    std::stringstream ss(line); // Insert the string into a stream
    while (ss >> buf)	tokens.push_back(buf);
    unsigned long GapsRun=FT2.Get_Run_ID(tokens[0]);
    std::cout<<"GapsRunID= "<<GapsRun<<"\n";
    std::cout<<"RunId=     "<<FT2.RunID<<"\n";
    //SKIP lines that start with # character
    comment=line.substr(0, 1);
    if(comment.find( "#", 0) == std::string::npos  && GapsRun==FT2.RunID){
      //increase Gaps size
      Entries++;
      FT2.GP.Set_GAPS_Size(FT2.GP, Entries);
      std::cout<<"gaps entries "<<Entries<<"\n";
      FT2.GP.GemStart[Entries-1]=std::strtoul(tokens[1].c_str(), NULL, 10);
      FT2.GP.GemStop[Entries-1] =std::strtoul(tokens[2].c_str(), NULL, 10);
      
    }
  }
  printf("Digi Gaps Informations\n");
  for (unsigned long i=0;i<Entries;i++){
    std::cout<<"GemStart="
    <<FT2.GP.GemStart[i]
    <<" GemStop="
    <<FT2.GP.GemStop[i]
    <<"\n";
  }
  
  if (Entries==0){
    std::cout<<"Gaps dont belong to this run, set DigiGaps false\n";
    FT2.DigiGAPS=false;
  }
  GPF.close();
}


//--------------  Set Gaps DeadTime----------------------------------

void FT2::Set_GAPS_DeadTime(FT2 &FT2){
  unsigned int T1, T2, DeltaT;
  double frac, GapTime, GapDeadTime, Total_GapDeadTime(0);
  printf("Set Gaps Dead Time\n");
  for (unsigned int i = 0; i < FT2.GP.Tstart.size(); ++i){
    
    Get_FT2_Entry_Index(FT2, FT2.GP.Tstart[i], T1);
    Get_FT2_Entry_Index(FT2, FT2.GP.Tstop[i], T2);
    printf("Gap=%d, TimeStart=%30.28e FT2_index=%d Tstop=%30.28e FT2_index=%d\n",
            i, FT2.GP.Tstart[i], T1, FT2.GP.Tstop[i], T2);
    
    DeltaT=T2-T1;
    
    GapTime=FT2.GP.Tstop[i]-FT2.GP.Tstart[i];
    
    GapDeadTime=FT2.GP.LiveTstop[i]-FT2.GP.LiveTstart[i];
    
    Total_GapDeadTime+=GapDeadTime;
    
    if(DeltaT==0){
      FT2.DT.GapsDeadTime[T1]+=GapDeadTime;
      FT2.DT.RealGapsDeadTime[T1]+=GapDeadTime;
      printf("DeltaT=0 dead time=%e\n", FT2.DT.DeadTime[T1]);
    }
    
    if(DeltaT==1){
      frac=(FT2.FT2_T.Tstop[T1]-FT2.GP.Tstart[i])/GapTime;
      FT2.DT.GapsDeadTime[T1]+=frac*GapDeadTime;
      FT2.DT.RealGapsDeadTime[T1]+=frac*GapDeadTime;
      printf("DeltaT=1 dead time=%e\n", FT2.DT.DeadTime[T1]);
      
      frac=(FT2.GP.Tstop[i]-FT2.FT2_T.Tstart[T2])/GapTime;
      FT2.DT.GapsDeadTime[T2]+=frac*GapDeadTime;
      FT2.DT.RealGapsDeadTime[T1]+=frac*GapDeadTime;
      printf("DeltaT=1 dead time=%e\n", FT2.DT.DeadTime[T2]);
    }
    
    if(DeltaT>1){
      double d1, d2, d;
      double test;
      printf("GapDeadTime=%e\n", GapDeadTime);
      frac=(FT2.FT2_T.Tstop[T1]-FT2.GP.Tstart[i])/GapTime;
      d1=frac*GapDeadTime;
      //!!!THIS DEAD TIME MUST NOT BE SUBTRACTED
      //FT2.DT.GapsDeadTime[T1]+=d1;
      FT2.DT.RealGapsDeadTime[T1]+=d1;
      printf("DeltaT>1 dead time=%e\n", FT2.DT.DeadTime[T1]);
      printf("d1=%e\n", d1);
      
      frac=(FT2.GP.Tstop[i]-FT2.FT2_T.Tstart[T2])/GapTime;
      d2=frac*GapDeadTime;
      //!!!THIS DEAD TIME MUST NOT BE SUBTRACTED
      //FT2.DT.GapsDeadTime[T2]+=d2;
      FT2.DT.RealGapsDeadTime[T2]+=d2;
      printf("DeltaT>1 dead time=%e\n", FT2.DT.DeadTime[T2]);
      printf("d2=%e\n", d2);
      
      d=(GapDeadTime-d1-d2)/(DeltaT-1);
      
      for(unsigned l=1;l<DeltaT;l++){
        FT2.DT.GapsDeadTime[T1+l]+=d;
        FT2.DT.RealGapsDeadTime[T1+l]+=d;
        printf("DeltaT>1 dead time=%e\n", FT2.DT.DeadTime[T1+l]);
      }
      printf("d=%e\n", d);
    }
    
  }
  printf("Total Gaps Dead Time=%e\n", Total_GapDeadTime);
}

//-------------- Fix fake Gaps----------------------------------
//if a time bins has zero events but is NOT in the middle of a gap
//if the fake gap is in the middle of the GAP you can't fix!!!!!
//It DOES NOT work for the FIRST and LAST Entry
///-------------------------------------------------------------

void FT2::Fix_Fake_GAPS(FT2 &FT2){
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  double fwd, bkw, frac, frac1, span, live;
  
  for(unsigned int i=1;i<FT2_ENTR-1;i++){
    if(!FT2.DT.update[i] && !FT2.DT.gap[i]){
      if(FT2.DT.update[i-1] &&FT2.DT.update[i+1]){
        //looks for the live in the previous entry
        bkw=FT2.DT.Tstop_LiveTime[i-1];
        //looks for the live time in the next entry
        fwd=FT2.DT.Tstart_LiveTime[i+1];
        live=fwd-bkw;
        span=FT2.DT.Tstart[i+1]-FT2.DT.Tstop[i-1];
        
        //correct the live time in the previous entry
        //it is different from the case of update=true
        //here we have not DT.Tstart values nor DT.Tstop
        frac=(FT2.FT2_T.Tstart[i]-FT2.DT.Tstop[i-1])/span;
        frac*=live;
        FT2.FT2_T.LiveTime[i-1]+= frac;
        
        //correct the live time in the next entry
        frac1=(FT2.DT.Tstart[i+1]-FT2.FT2_T.Tstop[i])/span;
        frac1*=live;
        FT2.FT2_T.LiveTime[i+1]+= frac1;
        
        FT2.FT2_T.LiveTime[i]=live-frac-frac1;
        
        
        
      }
    }
    
  }
}




void FT2::RunCut(FT2 &FT2, double Tstart_Run, double Tstop_Run){
  unsigned int Current_FT2_Entry, FT2_Entries;
  
  printf("---------------------------------------------------------\n");
  printf("Erase Extra Entries to fit Run lenght -\n");
  
  //add new entry
  printf("FT2 entries %d \n", Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2, Tstart_Run, Current_FT2_Entry);
  printf("FT2 Entry of Digi Tstart =%d\n", Current_FT2_Entry);
  
  if(!FT2.Get_OutOfRange(FT2) && Current_FT2_Entry>0){
    
    //Erase FT2
    FT2.FT2_T.Tstart.erase(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.begin()+Current_FT2_Entry);
    FT2.FT2_T.Tstop.erase(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.begin()+Current_FT2_Entry);
    FT2.FT2_T.LiveTime.erase(FT2.FT2_T.LiveTime.begin(), FT2.FT2_T.LiveTime.begin()+Current_FT2_Entry);
    FT2.FT2_T.bin.erase(FT2.FT2_T.bin.begin(), FT2.FT2_T.bin.begin()+Current_FT2_Entry);
    
    //Erase ATT
    FT2.ATT.Tstart.erase(FT2.ATT.Tstart.begin(), FT2.ATT.Tstart.begin()+Current_FT2_Entry);
    FT2.ATT.x.erase(FT2.ATT.x.begin(), FT2.ATT.x.begin()+Current_FT2_Entry);
    FT2.ATT.y.erase(FT2.ATT.y.begin(), FT2.ATT.y.begin()+Current_FT2_Entry);
    FT2.ATT.z.erase(FT2.ATT.z.begin(), FT2.ATT.z.begin()+Current_FT2_Entry);
    FT2.ATT.w.erase(FT2.ATT.w.begin(), FT2.ATT.w.begin()+Current_FT2_Entry);
    FT2.ATT.vx.erase(FT2.ATT.vx.begin(), FT2.ATT.vx.begin()+Current_FT2_Entry);
    FT2.ATT.vy.erase(FT2.ATT.vy.begin(), FT2.ATT.vy.begin()+Current_FT2_Entry);
    FT2.ATT.vz.erase(FT2.ATT.vz.begin(), FT2.ATT.vz.begin()+Current_FT2_Entry);
    FT2.ATT.entr.erase(FT2.ATT.entr.begin(), FT2.ATT.entr.begin()+Current_FT2_Entry);
    
    //Erase ORB
    FT2.ORB.Tstart.erase(FT2.ORB.Tstart.begin(), FT2.ORB.Tstart.begin()+Current_FT2_Entry);
    FT2.ORB.x.erase(FT2.ORB.x.begin(), FT2.ORB.x.begin()+Current_FT2_Entry);
    FT2.ORB.y.erase(FT2.ORB.y.begin(), FT2.ORB.y.begin()+Current_FT2_Entry);
    FT2.ORB.z.erase(FT2.ORB.z.begin(), FT2.ORB.z.begin()+Current_FT2_Entry);
    FT2.ORB.vx.erase(FT2.ORB.vx.begin(), FT2.ORB.vx.begin()+Current_FT2_Entry);
    FT2.ORB.vy.erase(FT2.ORB.vy.begin(), FT2.ORB.vy.begin()+Current_FT2_Entry);
    FT2.ORB.vz.erase(FT2.ORB.vz.begin(), FT2.ORB.vz.begin()+Current_FT2_Entry);
    FT2.ORB.CM.erase(FT2.ORB.CM.begin(), FT2.ORB.CM.begin()+Current_FT2_Entry);
    FT2.ORB.SAA.erase(FT2.ORB.SAA.begin(), FT2.ORB.SAA.begin()+Current_FT2_Entry);
    FT2.ORB.entr.erase(FT2.ORB.entr.begin(), FT2.ORB.entr.begin()+Current_FT2_Entry);
    
    printf("Erased  entry from 0 to %d nwe FT2 entries size %d \n", Current_FT2_Entry, Get_FT2_Entries(FT2));
    
  }
  
  FT2.Get_FT2_Entry_Index(FT2, Tstop_Run, Current_FT2_Entry);
  printf("FT2 Entry of Digi Tstop =%d\n", Current_FT2_Entry);
  if(!FT2.Get_OutOfRange(FT2) ){
    
    //Erase FT2
    FT2.FT2_T.Tstart.erase(FT2.FT2_T.Tstart.begin()+Current_FT2_Entry, FT2.FT2_T.Tstart.end());
    FT2.FT2_T.Tstop.erase(FT2.FT2_T.Tstop.begin()+Current_FT2_Entry, FT2.FT2_T.Tstop.end());
    FT2.FT2_T.LiveTime.erase(FT2.FT2_T.LiveTime.begin()+Current_FT2_Entry, FT2.FT2_T.LiveTime.end());
    FT2.FT2_T.bin.erase(FT2.FT2_T.bin.begin()+Current_FT2_Entry, FT2.FT2_T.bin.end());
    
    //Erase ATT
    FT2.FT2_T.Tstart.erase(FT2.FT2_T.Tstart.begin()+Current_FT2_Entry, FT2.FT2_T.Tstart.end());
    FT2.ATT.x.erase(FT2.ATT.x.begin()+Current_FT2_Entry, FT2.ATT.x.end());
    FT2.ATT.y.erase(FT2.ATT.y.begin()+Current_FT2_Entry, FT2.ATT.y.end());
    FT2.ATT.z.erase(FT2.ATT.z.begin()+Current_FT2_Entry, FT2.ATT.z.end());
    FT2.ATT.w.erase(FT2.ATT.w.begin()+Current_FT2_Entry, FT2.ATT.w.end());
    FT2.ATT.vx.erase(FT2.ATT.vx.begin()+Current_FT2_Entry, FT2.ATT.vx.end());
    FT2.ATT.vy.erase(FT2.ATT.vy.begin()+Current_FT2_Entry, FT2.ATT.vy.end());
    FT2.ATT.vz.erase(FT2.ATT.vz.begin()+Current_FT2_Entry, FT2.ATT.vz.end());
    FT2.ATT.entr.erase(FT2.ATT.entr.begin()+Current_FT2_Entry, FT2.ATT.entr.end());
    
    //Erase ORB
    FT2.ORB.Tstart.erase(FT2.ORB.Tstart.begin()+Current_FT2_Entry, FT2.ORB.Tstart.end());
    FT2.ORB.x.erase(FT2.ORB.x.begin()+Current_FT2_Entry, FT2.ORB.x.end());
    FT2.ORB.y.erase(FT2.ORB.y.begin()+Current_FT2_Entry, FT2.ORB.y.end());
    FT2.ORB.z.erase(FT2.ORB.z.begin()+Current_FT2_Entry, FT2.ORB.z.end());
    FT2.ORB.vx.erase(FT2.ORB.vx.begin()+Current_FT2_Entry, FT2.ORB.vx.end());
    FT2.ORB.vy.erase(FT2.ORB.vy.begin()+Current_FT2_Entry, FT2.ORB.vy.end());
    FT2.ORB.vz.erase(FT2.ORB.vz.begin()+Current_FT2_Entry, FT2.ORB.vz.end());
    FT2.ORB.CM.erase(FT2.ORB.CM.begin()+Current_FT2_Entry, FT2.ORB.CM.end());
    FT2.ORB.SAA.erase(FT2.ORB.SAA.begin()+Current_FT2_Entry, FT2.ORB.SAA.end());
    FT2.ORB.entr.erase(FT2.ORB.entr.begin()+Current_FT2_Entry, FT2.ORB.entr.end());
    
    printf("Erased  entry from End  to %d new FT2 entries size %d \n", Current_FT2_Entry, Get_FT2_Entries(FT2));
    
  }
  
  
  printf("---------------------------------------------------------\n");
  
  //printf("---------------------------------------------------------\n");
  //for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){
  //   FT2.FT2_T.LiveTime[i]=0;
  //   FT2.FT2_T.bin[i]=i;
  //   printf("Entry Id=%d Tstart=%20.18g  Tstop=%20.18g\n",i,FT2.FT2_T.Tstart[i],FT2.FT2_T.Tstop[i]);
  //}
  printf("---------------------------------------------------------\n");
}
