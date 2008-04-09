/** @file Digi.cxx
 * @brief
 * FT2::Digi_FT2(FT2 &FT2) is the used class to evaluate live time from Digi File.
 * Digi file is used to evaluate the LiveTime for eache FT2 entry. The Merit File is used to find
 * recon crasches and correct LiveTime Accrdingly. The M7 file is used t o generate FT2 time entries
 *
 * @author Andrea Tramacere <tramacer@slac.stanford.edu>
 *
 */

/*-------------------------------------------------------------------------------
 *FT2::Digi_FT2(FT2 &FT2) is the class to evaluate live time from Digi File.
 *It uses Merit File to find recon craches and correct LiveTime Accrdingly
 *It uses M7 file to generate FT2 time entries
 *-----------------------------------------------------------------------------*/
//--------------------------- ROOT DECLARATIONS ---------------------------------

// ROOT Headers
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TH1.h"
#include "TStyle.h"
#include "TCollection.h"  // Declares TIte

// Enums
#include "enums/Lsf.h"

// Digi Classes
#include "digiRootData/LsfTime.h"
#include "digiRootData/RunInfo.h"
#include "digiRootData/DatagramInfo.h"
#include "digiRootData/GemScalers.h"
#include "digiRootData/Configuration.h"
#include "digiRootData/DigiEvent.h"

// stl/c++ headers
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>

// headers for this package
#include "ft2Util/FT2_Time.hpp"

// headers for exceptions
#include <stdexcept>


void FT2::Digi_FT2(FT2 &FT2){
  
  //!!! These values are very important
  double conv=50.0/1e9;
  double RollOver = 33554432.0
  
  double curr_live, curr_elapsed;
  ULong64_t currentElapsed;
  ULong64_t previousElapsed;
  ULong64_t currentLive;
  ULong64_t deltaElapsed;
  ULong64_t currentDeadTime;
  ULong64_t previousDeadTime;
  ULong64_t deltaDeadTime;
  char number_string[100];
  //-------------------------------------------------------------------------------
  
  
  
  //-------------------  Check-up variables ---------------------------------------
  double First_Live_Time, Last_Live_Time;
  double Tstart_Run, Tstop_Run;
  double Digi_Live_Time, FT2_Live_Time, FT2_Live_Time1;
  double Total_Dead_Time(0);
  unsigned int EvID_Tstart_Live, EvID_Tstop_Live;
  //-------------------------------------------------------------------------------
  
  
  /*------------------------ FT2 DECLARATIONS ----------------------------------
   *Declarations of variable used for the Digi File and Live Time
   *unsigned int DigiLineCounter(0), M7LineCounter(0);
   *---------------------------------------------------------------------------*/
  double DigiTime, Current_LiveTime, Old_LiveTime, LiveTime_Tstart, LiveTime_Tstop;
  double DeadTime(0);
  bool first_bin(true);
  unsigned int New_FT2_Entry(0), Current_FT2_Entry, Old_FT2_Entry;
  unsigned int Digi_i, Merit_i, Digi_Start, Digi_Stop;
  //----------------------------------------------------------------------------
  
  std::string buf, comment; //buffer string
  std::string line;
  std::ofstream out("Digi-out.txt");
  
  //-----------------------------------------------------------------------------
  
  
  /*----------------------------------------------------------------------------------
   *----------------- START HANDLING ROOT DIGI & MERIT FILE --------------------------
   *---------------------------------------------------------------------------------*/
  
  /*---------------------------------------------------------
   *------------------ ROOT Start TTree DIGI ----------------
   *---------------------------------------------------------*/
  TFile *fDigi=new TFile(FT2.DigiFile.c_str());
  TTree *T = (TTree*)fDigi->Get("Digi");
  //--- ROOT Disable all branches DIGI ---
  T->SetBranchStatus("*", 0);
  //--- ROOT Enable only needed branch DIGI --
  T->SetBranchStatus("m_eventId", 1);
  T->SetBranchStatus("m_liveTime", 1);
  T->SetBranchStatus("m_timeStamp", 1);
  T->SetBranchStatus("m_metaEvent", 1);
  T->SetBranchStatus("m_gem", 1);
  //--- ROOT Digi Inizialization ---
  DigiEvent * evt;
  evt = 0;
  T->SetBranchAddress("DigiEvent", &evt);
  UInt_t Digi_nEvt = (UInt_t)T->GetEntries();
  UInt_t Digi_EvtId;
  //---------------------------------------------------------
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //Digi_nEvt=100000;
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  
  /*-------- WORK ON  M7 FILE ---------------------------------------------------------
   *Read the M7 file and generates FT2 time entries according to M7 file entries
   *----------------------------------------------------------------------------------*/
  std::cout<<"M-7 file"<<std::endl;
  //Read M-7 File and Set FT2 Entries
  
  
  FT2.Set_M7_Entries(FT2);
  std::cout<<"======================================================"<<std::endl;
  //--------------------------------------------------------------------------
  
  //-------------------------------------------------------------------
  //------- Gaps ------------------------------------------------------
  if(FT2.DigiGAPS)FT2.Set_GAPS(FT2);
  //--------------------------------------------------------------------------
  
  
  //---------------------------------------------------------
  //---------------- ROOT Start TTree Merit -----------------
  TFile *fMerit=new TFile(FT2.MeritFile.c_str());
  TTree *MT= (TTree*)fMerit->Get("MeritTuple");
  //--- ROOT Disable all branches MERIT ---
  MT->SetBranchStatus("*", 0);
  //--- ROOT Enable only needed branch MERIT ---
  MT->SetBranchStatus("EvtEventId", 1);
  //ROOT Start TTree
  UInt_t Merit_EvtId;
  TBranch* brMT_EvId = MT->GetBranch("EvtEventId");
  brMT_EvId->SetAddress(&Merit_EvtId);
  UInt_t Merit_nEvt = (UInt_t)MT->GetEntries();
  //----------------------------------------------------------
  
  
  //----------------------------------------------------------
  //Warning if Digi and Merit Entries are not the same
  if(Merit_nEvt != Digi_nEvt  ){
    std::cout<<"Different numeber of entries in Digi and Merit File !!!!!!!\n"
    <<"Digi Entries "
    <<Digi_nEvt
    <<"Merit Entries "
    <<Merit_nEvt
    <<std::endl;
    
  }
  //----------------------------------------------------------
  
  
  
  
  //-----------------------------------------------------------
  //Find Tstart and Tstop od the Digi File
  //-----------------------------------------------------------
  printf("---------------------------------------------------\n");
  printf("Check on Tstart and Tstop of the Digi File and M& file \n");
  Digi_i=0;
  Digi_Start=0;
  //Takes the first Digi Element
  //that falls within the M7 time span
  double DeltaT;
  do{
    
    T->GetEntry(Digi_i);
    DigiTime=evt->getTimeStamp();
    //printf("DigiTime=%30.28g\n", DigiTime);
    FT2.Get_FT2_Entry_Index(FT2, DigiTime, Current_FT2_Entry);
    DeltaT=DigiTime-FT2.FT2_T.Tstart[0];
    evt->Clear();
    Digi_i++;
  }while(FT2.Get_OutOfRange(FT2)&&DeltaT>=1.0&& Digi_i<Digi_nEvt-1);
  Digi_i--;
  
  T->GetEntry(Digi_i);
  Tstart_Run=evt->getTimeStamp();
  Digi_Start=Digi_i;
  printf("ID of firtst Digi evt within 1 s gap from second the M7 file %d\n", Digi_Start);
  printf("Time of the firtst Digi evt within 1 s gap from second the M7 file %30.28g\n", DigiTime);
  printf("Tstart of the the M7 file %30.28g\n", FT2.FT2_T.Tstart[0]);
  
  //Takes the Last Digi Element
  //that falls within the M7 time span
  Digi_i=Digi_nEvt-1;
  unsigned int FT2Entries=Get_FT2_Entries(FT2);
  do{
    
    T->GetEntry(Digi_i);
    DigiTime=evt->getTimeStamp();
    //printf("DigiTime=%30.28g Digi_i=%d\n", DigiTime, Digi_i);
    FT2.Get_FT2_Entry_Index(FT2, DigiTime, Current_FT2_Entry);
    DeltaT=FT2.FT2_T.Tstop[FT2Entries]-DigiTime;
    evt->Clear();
    Digi_i--;
  }while(FT2.Get_OutOfRange(FT2) && Digi_i>0 &&DeltaT>=1.0);
  Digi_i++;
  //Digi_i=Digi_nEvt-1;
  T->GetEntry(Digi_i);
  Tstop_Run=evt->getTimeStamp();
  Digi_Stop=Digi_i;
  printf("ID of the last Digi evt within 1 s gap from the M7 file %d\n", Digi_Stop);
  printf("Tstop of the the M7 file %30.28g\n", FT2.FT2_T.Tstop[FT2Entries-1]);
  printf("Time of the last Digi evt within 1 s gap from second the M7 file %30.28g\n", DigiTime);
  printf("Tstart RUN=%30.28g  Tstop RUN=%30.28g\n", Tstart_Run, Tstop_Run);
  printf("---------------------------------------------------\n");
  
  
  
  /*---------------------------------------------------------------------------
   *             MERGE M7 and DIGI ENTRIES
   *Merge The M7 entries with the entries in the Digi
   *corresponding to the first and last event
   *The Sc entries must be filled after M7-Digi Merging, because the
   *merging process will create two new FT2 entries
   *-------------------------------------------------------------------------*/
  printf("Merging ");
  FT2.Merge_M7_Digi_Entries(FT2, Tstart_Run , Tstop_Run);
  
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
  
  
  
  
  /*-------------------M7 issues STOP here ------------------------------------
   *-------------------------------------------------------------------------*/
  
  
  /*--------------------------------------------------------------------------
   *                HRES START LOOP OVER DIGI & MERIT FILE
   *------------------------------------------------------------------------*/
  
  //Digi File Line Numbers
  std::cout<<"DigiFile has "<<   Digi_nEvt <<" events" <<std::endl;
  
  //ROOT LOPPING OVER EVENTS
  out<<"#Digi_I Digi_EvtID Merit Evt_ID TSTAMP            LIVE  s             ELAPS  s           LIVE     ELAPS    DEAD    DeltaEl DeltaDead "<<endl;
  
  //-----------  LOOP OVER DIGI & MERIT FILE ---------------------------
  
  //--- Set FT2.Digi_Time_Size ---
  FT2.DT.Set_DigiTime_Size(FT2.DT, FT2.Get_FT2_Entries(FT2));
  
  //---!!! Merit Starts from First Digi ---
  Merit_i=Digi_Start;
  unsigned int Gap_i(0);
  bool on_gaps=false;
  for (Digi_i = Digi_Start; Digi_i < Digi_nEvt; Digi_i++){
    
    // ---Get the Entries---;
    T->GetEntry(Digi_i);
    MT->GetEntry(Merit_i);
    
    Digi_EvtId=evt->getEventId();
    
    
    //Simulating a Crash!!!
    //if(Digi_EvtId==403612){
    //  Merit_i+=10000;
    //}
    
    //---------- operations over Digi entries -------------------
    currentElapsed=evt->getMetaEvent().scalers().elapsed();
    currentLive=evt->getMetaEvent().scalers().livetime();
    currentDeadTime = currentElapsed - currentLive;
    
    if(Digi_i==Digi_Start){
      previousElapsed=currentElapsed;
      previousDeadTime=currentDeadTime;
    }
    
    deltaElapsed=currentElapsed-previousElapsed;
    deltaDeadTime = currentDeadTime-previousDeadTime;
    
    //---TYPECASTING---
    sprintf(number_string, "%llu", currentLive);
    curr_live=strtod(number_string, NULL);
    //---TYPECASTING---
    sprintf(number_string, "%llu", currentElapsed);
    curr_elapsed=strtod(number_string, NULL);
    
    
    //ROOT TXT OUT FILE
    if ( Digi_i % 1000 == 0 ) {
      //      cout<<"Digi File at " << double(Digi_i)/double(Digi_nEvt)*100.0 <<"% "<<endl;
      out<<(int)Digi_i<<" ";
      out<<Digi_EvtId<<" " ;
      out<<Merit_EvtId<<" " ;
      out<<setprecision(20) <<evt->getTimeStamp()<<" ";
      out<<curr_live*conv<<" ";
      out<<curr_elapsed*conv<<" ";
      out<<currentElapsed<<" ";
      out<<currentDeadTime<<" " ;
      out<<deltaElapsed<<" ";
      out<<deltaDeadTime;
      out<<endl;
    }
    
    //--- SWAPPING ROOT VARIABLES ---
    previousElapsed=currentElapsed;
    previousDeadTime=currentDeadTime;
    
    /*-----------------------------------------------------------------------
     *                   WORK ON FT2 Live Time
     *!!!!! ASSUMING THAT THE ENTRIES IN DIGI FILE ARE TIME ORDERED !!!!!
     * IF DigiEvtId==MeritEvtId -> Increse Live time && Increase
     * both Digi(that is done by for loop) and Merit index (manually )
     * later (at the end of Digi loop)
     *---------------------------------------------------------------------*/
    DigiTime=evt->getTimeStamp();
    
    FT2.Get_FT2_Entry_Index(FT2, DigiTime, Current_FT2_Entry);
    
    ///!!!!TIME CALIBRATION
    if((first_bin)||(New_FT2_Entry)){
      // Verify that the two TimeTones are OK:
      if (!(evt->metaEvent.time().current().incomplete()) &&
              !(evt->(metaEvent.time().current().flywheeling()) &&
              !(evt->metaEvent.time().current().missingCpuPps()) &&
              !(evt->metaEvent.time().current().missingLatPps()) &&
              !(evt->metaEvent.time().current().missingTimeTone()) &&
              !(evt->metaEvent.time().previous().incomplete()) &&
              !(evt->metaEvent.time().previous().flywheeling()) &&
              !(evt->metaEvent.time().previous().missingCpuPps()) &&
              !(evt->metaEvent.time().previous().missingLatPps()) &&
              !(evt->metaEvent.time().previous().missingTimeTone()) &&
              // Avoid 1/0 error:
              (evt->metaEvent.time().current().timeHack().ticks() !=
              evt->metaEvent.time().previous().timeHack().ticks()) &&
              // If there is more than a second between 1-PPS I can
              // only use the nominal value for the LAT clock anyway!
              ((evt->metaEvent.time().current().timeSecs() -
              evt->metaEvent.time().previous().timeSecs()) == 1)) {
        
        // Number of ticks between the current and the previous time tone:
        double clockTicksDelta1PPS =
        double (evt->metaEvent.time().current().timeHack().ticks()) -
        double (evt->metaEvent.time().previous().timeHack().ticks());
        
        // Rollover?
        if (clockTicksDelta1PPS < 0) {
          clockTicksDelta1PPS = clockTicksDelta1PPS + RollOver;
        }
        
        // This is what you want:
        conv = clockTicksDelta1PPS;
      } else {
        conv=50.0/1e9;
      }
    }
    
    Current_LiveTime=curr_live*conv;
    
    
    //----Find Tstart and Tstop for each Gap-----------
    if(FT2.DigiGAPS){
      if(Digi_EvtId==FT2.GP.GemStart[Gap_i]){
        FT2.GP.Tstart[Gap_i]=DigiTime;
        FT2.GP.LiveTstart[Gap_i]=Current_LiveTime;
        on_gaps=true;
      }
      if(Digi_EvtId==FT2.GP.GemStop[Gap_i]){
        FT2.GP.Tstop[Gap_i]=DigiTime;
        FT2.GP.LiveTstop[Gap_i]=Current_LiveTime;
        Gap_i++;
        on_gaps=false;
      }
    }
    if(on_gaps) FT2.DT.gap[Current_FT2_Entry]=true;
    else FT2.DT.gap[Current_FT2_Entry]=false;
    //-------------------------------------------------
    
    
    if(Digi_i==Digi_Start){
      //check-up
      First_Live_Time=Current_LiveTime;
      Old_LiveTime = Current_LiveTime;
      first_bin=true;
    } 
    
    //--- Increase dead time   ------------------------
    //---Correction for the dead time given by crush---
    if(Digi_EvtId!=Merit_EvtId){
      printf("Recon Crash\n");
      double RecDead=Current_LiveTime-Old_LiveTime;
      double fraction=0.0;
      
      //Split Dead Time if a New Entry starts
      if(New_FT2_Entry){
        fraction= FT2.FT2_T.Tstop[Current_FT2_Entry-1]-FT2.DT.Tstop[Current_FT2_Entry-1];
        fraction*=1.0/(DigiTime-FT2.DT.Tstop[Current_FT2_Entry-1]);
        FT2.DT.ReconDeadTime[Current_FT2_Entry-1]+=RecDead*(fraction);
      }
      FT2.DT.ReconDeadTime[Current_FT2_Entry]+=RecDead*(1.0-fraction);
      
      
    }
    
    if(Current_FT2_Entry!=Old_FT2_Entry && !first_bin ){
      New_FT2_Entry=1;
      
      //FT2.FT2_T.LiveTime[Current_FT2_Entry-1]=
      //(FT2.DT.Tstop_LiveTime[Current_FT2_Entry-1] -  FT2.DT.Tstart_LiveTime[Current_FT2_Entry-1]);
      
      //--- Print last Entry Values ----
      if(FT2.verbose){
        printf("Start a New Entry\n");
        printf("Current %d  Old Entry %d\n", Current_FT2_Entry, Current_FT2_Entry-1);
        printf("Old Entry Results\n");
        printf("DigiTime=%20.18e \n", DigiTime);
        printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",
                FT2.DT.Tstart_LiveTime[Current_FT2_Entry-1], FT2.DT.Tstop_LiveTime[Current_FT2_Entry-1]);
        std::cout<<setprecision(20)
        <<"DT.Tstart "
        <<FT2.DT.Tstart[Current_FT2_Entry-1]
        <<" DT.Tstop "
        <<FT2.DT.Tstop[Current_FT2_Entry-1]
        <<" Delta T "
        <<FT2.DT.Tstop[Current_FT2_Entry-1] - FT2.DT.Tstart[Current_FT2_Entry-1]
        <<"\nFT.Tstart "
        <<FT2.FT2_T.Tstart[Current_FT2_Entry-1]
        <<" FT.Tstop "
        <<FT2.FT2_T.Tstop[Current_FT2_Entry-1]
        <<" Live Time "
        <<FT2.FT2_T.LiveTime[Current_FT2_Entry-1]
        <<" Dead Time "
        <<FT2.DT.ReconDeadTime[Current_FT2_Entry-1]
        <<endl
        <<"EvID_Tstart_Live "
        <<EvID_Tstart_Live
        <<" EvID_Tstop_Live "
        <<EvID_Tstop_Live
        <<std::endl;
      }
    }
    
    /*---------------------------------------------
     *Operations when A NEW BIN STARTS
     *
     *--------------------------------------------*/
    if((first_bin)||(New_FT2_Entry)){
      
      first_bin=false;
      
      printf("New Entry %d\n", Current_FT2_Entry);
      cout<<"Digi File at " << double(Digi_i)/double(Digi_nEvt)*100.0 <<"% "<<endl;
      
      //---Set true update value---
      FT2.DT.update[Current_FT2_Entry]=true;
      
      //---Update DigiTstart and others---
      FT2.DT.Tstart[Current_FT2_Entry]=DigiTime;
      
      //------- Update LiveTime ----------------------------
      FT2.DT.Tstart_LiveTime[Current_FT2_Entry]=Current_LiveTime;;
      LiveTime_Tstart = Current_LiveTime;
      
      
      EvID_Tstart_Live=Digi_EvtId;
      
      New_FT2_Entry=0;
      //Total_Dead_Time+=DeadTime;
      //DeadTime=0;
      
      if(FT2.verbose){
        printf("TstartLiveTime %20.18e CurrentLive %20.18e \n", FT2.DT.Tstart_LiveTime[Current_FT2_Entry], Current_LiveTime);
        std::cout<<"New FT2 Entry, #="
        <<Current_FT2_Entry
        <<" DigiTime="
        <<DigiTime
        <<"FT2 Id="
        <<FT2.Get_FT2_Time_Bin(DigiTime, Tstart_Run)
        <<"Digi EvtId"
        <<Digi_EvtId
        <<std::endl
        <<"--------------------------------------------------------"
        <<std::endl
        <<std::endl;
      }
    }
    
    //UPDATE TSTOP
    FT2.DT.Tstop[Current_FT2_Entry]=DigiTime;
    
    //UPDATE LIVE TIME
    LiveTime_Tstop = Current_LiveTime;
    //FT2.FT2_T.LiveTime[Current_FT2_Entry]=(LiveTime_Tstop -  LiveTime_Tstart)-DeadTime;
    FT2.DT.Tstop_LiveTime[Current_FT2_Entry] = Current_LiveTime;
    
    //check-up
    LiveTime_Tstop=Current_LiveTime;
    EvID_Tstop_Live=Digi_EvtId;
    
    
    //--- Increase Merit Index only id the index is aligned with Digi one ----
    if(Digi_EvtId==Merit_EvtId){
      Merit_i++;
    }
    
    //SWAPPING
    Old_FT2_Entry=Current_FT2_Entry;
    Old_LiveTime=Current_LiveTime;
    evt->Clear();
    
    //check-up
    Last_Live_Time=Current_LiveTime;
    
    /*----------------------------------------------------------------------
     *                  END OF LOOPING DIGI FILE
     *--------------------------------------------------------------------*/
  }
  
  
  /*------Operations ON LAST ENTRY -----------------------------------------*/
  //---Print last Entry Values---
  printf("Old Entry %d\n", Current_FT2_Entry);
  printf("Old Entry Results\n");
  printf("DigiTime=%20.18e \n", DigiTime);
  printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",
          FT2.DT.Tstart_LiveTime[Current_FT2_Entry], FT2.DT.Tstop_LiveTime[Current_FT2_Entry]);
  std::cout<<setprecision(20)
  <<"DT.Tstart "
  <<FT2.DT.Tstart[Current_FT2_Entry]
  <<" DT.Tstop "
  <<FT2.DT.Tstop[Current_FT2_Entry]
  <<" Delta T "
  <<FT2.DT.Tstop[Current_FT2_Entry] - FT2.DT.Tstart[Current_FT2_Entry]
  <<"\nFT.Tstart "
  <<FT2.FT2_T.Tstart[Current_FT2_Entry]
  <<" FT.Tstop "
  <<FT2.FT2_T.Tstop[Current_FT2_Entry]
  <<" Live Time "
  <<FT2.FT2_T.LiveTime[Current_FT2_Entry]
  <<" Dead Time "
  <<FT2.DT.ReconDeadTime[Current_FT2_Entry]
  <<endl
  <<"EvID_Tstart_Live "
  <<EvID_Tstart_Live
  <<" EvID_Tstop_Live "
  <<EvID_Tstop_Live
  <<std::endl;
  
  
  /*-------------------- VERBOSE OUTPUT -----------------------------------*/
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  std::cout<<"======================================================"<<std::endl;
  if(FT2.verbose){
    for(unsigned int i=0;i<FT2_ENTR;i++){
      printf("Current Entry %d\n", i);
      printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",
              FT2.DT.Tstart_LiveTime[i], FT2.DT.Tstop_LiveTime[i]);
      std::cout<<setprecision(20)
      <<"Tstart "
      <<FT2.DT.Tstart[i]
      <<" Tstop "
      <<FT2.DT.Tstop[i]
      <<" Delta T "
      <<FT2.DT.Tstop[i] - FT2.DT.Tstart[i]
      <<" Live Time "
      <<FT2.FT2_T.LiveTime[i]
      <<" Dead Time "
      <<FT2.DT.ReconDeadTime[i]
      <<"\n--------------------------------------------------------"
      <<std::endl;
    }
    std::cout<<"======================================================"<<std::endl;
  }
  
  /*--------------------------------------------------------------------------
   *
   *Evaluate LIVE TIME tacking into account edge effects
   *DEAD TIME form Recon Crash, Gaps and Fake Gaps
   *
   *------------------------------------------------------------------------*/
  for(unsigned int i=0;i<FT2_ENTR;i++){
    Total_Dead_Time+=FT2.DT.ReconDeadTime[i];
  }
  printf("Total Recon crushes Dead Time=%30.28e\n", Total_Dead_Time);
  
  //---------------LIVE TIME CALCULATIONS ---------------------------
  if(FT2.DigiGAPS)FT2.Set_GAPS_DeadTime(FT2);
  FT2.Evaluate_Live_Time(FT2);
  FT2.Fix_Fake_GAPS(FT2);
  //-----------------------------------------------------------------
  
  
  /*-------------------------------------------------------------------------*/
  
  /*-------------------- VERBOSE CHECK ON LIVETIME--------------------------*/
  FT2_Live_Time=0;
  Total_Dead_Time=0;
  for(unsigned int i=0;i<FT2_ENTR;i++){
    FT2_Live_Time+=FT2.FT2_T.LiveTime[i];
    Total_Dead_Time+=FT2.DT.ReconDeadTime[i];
  }
  printf("dead=%e\n", Total_Dead_Time);
  for(unsigned int i = 0; i < FT2.GP.Tstart.size(); ++i){
    Total_Dead_Time+=FT2.GP.LiveTstop[i]-FT2.GP.LiveTstart[i];
    
  }
  printf("dead=%e\n", Total_Dead_Time);
  FT2_Live_Time1=Last_Live_Time-First_Live_Time;
  printf("----------------------------------------------------------------------------------------------------\n");
  printf("Last_Live_Time=%30.28e First_Live_Time=%30.28e \n", Last_Live_Time, First_Live_Time);
  cout<<setprecision(30)
  <<"FT2_live_Time(sum over LiveTime[i]) "
  <<FT2_Live_Time
  <<endl
  <<"Last-First LiveTime "
  <<FT2_Live_Time1
  <<endl
  <<"Last Live Time "
  <<Last_Live_Time
  <<endl
  <<"First Live_Time "
  <<First_Live_Time
  <<endl
  <<"FT2_live_Time -  (Last-First LiveTime) - DeadTime = "
  <<FT2_Live_Time1 - FT2_Live_Time -   Total_Dead_Time
  <<endl
  <<"Total_Dead_Time "
  <<Total_Dead_Time
  <<endl;
  //------------------------------------------------------------
  
  
  /*-------------------------------------------------------------------------
   *                   GENERATING FITS AND TXT FT2 FILE
   *!!!Because M7 file is per-Down-Link (DL) and Digi per-RUN (1DL = 2RUNS)
   *We MUST CUT from FITS and TXT FT2 files, entries outside of DIGI BOUNDARIES
   *-----------------------------------------------------------------------*/
  
  //---!!!VALUES NECESSARY TO CUT FT2 ENTRIES OUTSIDE OF DIGI FILE
  FT2.DT.RunStart=Tstart_Run;
  FT2.DT.RunStop=Tstop_Run;
  
  //--------------Write FT2 fits file---------------------------
  FT2.WriteTextFile(FT2);
  
  //--------------Write FT2 fits file---------------------------
  FT2.WriteFitsFile(FT2);
  
  //return(0);
}
