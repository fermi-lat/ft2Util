/** @file Digi.cxx
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

*/


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

  double conv=50.0/1e9; 
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
  double Tstart_Run,Tstop_Run;
  double Digi_Live_Time,FT2_Live_Time,FT2_Live_Time1;
  double Total_Dead_Time(0);
  unsigned int EvID_Tstart_Live, EvID_Tstop_Live;
  //-------------------------------------------------------------------------------
  
  
  //------------------------ FT2 DECLARATIONS ------------------------------------- 
  //Declarations of variable used for the Digi File and Live Time
  //unsigned int DigiLineCounter(0), M7LineCounter(0);
  double DigiTime, Current_LiveTime,Old_LiveTime, LiveTime_Tstart, LiveTime_Tstop;
  double DeadTime(0);
  bool first_bin(true);
  unsigned int New_FT2_Entry(0),Current_FT2_Entry,Old_FT2_Entry;
  unsigned int Digi_i,Merit_i,Digi_Start;
  //----------------------------------------------------------------------------
  
  std::string buf,comment; //buffer string
  std::string line;
  std::ofstream out("Digi-out.txt");
  
  //-----------------------------------------------------------------------------------
  
  
  
  //-------- WORK ON  M7 FILE ---------------------------------------------------------
  std::cout<<"M-7 file"<<std::endl;
  //Read M-7 File and Set FT2 Entries
  FT2.Set_M7_Entries(FT2); 
  std::cout<<"======================================================"<<std::endl;
  //-----------------------------------------------------------------------------------

    
  
  
  //------------------ START HANDLING ROOT DIGI & MERIT FILE --------------------------
  //-----------------------------------------------------------------------------------
  
  
  
  //----------------------------------------------------------
  //------------------ ROOT Start TTree DIGI -----------------
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
  

    
  //---------------------------------------------------------
  //---------------- ROOT Start TTree Merit -----------------
  TFile *fMerit=new TFile(FT2.MeritFile.c_str());
  TTree *MT= (TTree*)fMerit->Get("MeritTuple");
  //--- ROOT Disable all branches MERIT ---
  MT->SetBranchStatus("*", 0);
  //--- ROOT Enable only needed branch MRIT ---
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
  
  

  
  
  //-------------MERGE M7 and DIGI ENTRIES--------------------
  //   Merge The M7 entries with the entries in the Digi
  //   corresponding to the first and last event
  
  printf("Merging ");
  Digi_i=0;
  Digi_Start=0;
  //Takes the first Digi Element
  //that falls within the M7 time span
  do{
    T->GetEntry(Digi_i);
    DigiTime=evt->getTimeStamp();
    printf("DigiTime=%30.28g\n", DigiTime);
    FT2.Get_FT2_Entry_Index(FT2,DigiTime,Current_FT2_Entry);
    evt->Clear(); 
    Digi_i++;
  }while(FT2.Get_OutOfRange(FT2));
  Digi_i--;
    
  T->GetEntry(Digi_i);
  Tstart_Run=evt->getTimeStamp();
  Digi_Start=Digi_i;
  

  //!!!!Here does not check that the last element
  //of the Digi file fall within the M7 time span
  Digi_i=Digi_nEvt-1;
  T->GetEntry(Digi_i);
  Tstop_Run=evt->getTimeStamp();
  printf("Tstart RUN=%30.28g  Tstop RUN=%30.28g\n",Tstart_Run,Tstop_Run);
  
  FT2.Merge_M7_Digi_Entries(FT2,Tstart_Run ,Tstop_Run);
  
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
  
  
  
  //Digi File Line Numbers
  std::cout<<"DigiFile has "<<   Digi_nEvt <<" events" <<std::endl;
  
  //ROOT LOPPING OVER EVENTS
  out<<"#Digi_I Digi_EvtID Merit Evt_ID TSTAMP            LIVE  s             ELAPS  s           LIVE     ELAPS    DEAD    DeltaEl DeltaDead "<<endl;
    
    
    
  //-----------  LOOP OVER DIGI & MERIT FILE ---------------------------
  
  //Digi_nEvt=50000;
    
  //Set FT2.Digi_Time_Size
  FT2.DT.Set_DigiTime_Size(FT2.DT,FT2.Get_FT2_Entries(FT2));
  
  Merit_i=Digi_Start;
  for (Digi_i = Digi_Start; Digi_i < Digi_nEvt ; Digi_i++){
    
    // Get the Entries;
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
    
    //TYPECASTING
    sprintf(number_string,"%llu",currentLive);
    curr_live=strtod(number_string,NULL);
    //TYPECASTING
    sprintf(number_string,"%llu",currentElapsed);
    curr_elapsed=strtod(number_string,NULL);
    
    
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
    
    //SWAPPING ROOT VARIABLES
    previousElapsed=currentElapsed;
    previousDeadTime=currentDeadTime;
    
    
    //---------------------   WORK ON FT2 Live Time  ------------------------------------
    
    //--------!!!!! ASSUMING THAT THE ENTRIES IN DIGI FILE ARE TIME ORDERED !!!!!-------
    // IF DigiEvtId==MeritEvtId -> Increse Live time && Increase 
    // oth Digi(that is done by for loop) and Merit index (manually )
    
    DigiTime=evt->getTimeStamp();
    Current_LiveTime=curr_live*conv;
    FT2.Get_FT2_Entry_Index(FT2,DigiTime,Current_FT2_Entry);
    
    if(Digi_i==Digi_Start){
      //check-up
      First_Live_Time=Current_LiveTime;
      Old_LiveTime = Current_LiveTime;
      first_bin=true;
    }
    
    
    //--- Increase dead time  ------
    if(Digi_EvtId!=Merit_EvtId){
      FT2.DT.DeadTime[Current_FT2_Entry]+=Current_LiveTime-Old_LiveTime;			
    }
    
    
    if(Current_FT2_Entry!=Old_FT2_Entry && !first_bin ){
      
      New_FT2_Entry=1;
      //Correction for the dead time given by crush
      FT2.FT2_T.LiveTime[Current_FT2_Entry-1]=
	  (FT2.DT.Tstop_LiveTime[Current_FT2_Entry-1] -  FT2.DT.Tstart_LiveTime[Current_FT2_Entry-1]);
      
      
      //Print last Entry Values
      if(FT2.verbose){ 
	printf("Start a New Entry\n");
	printf("Current %d  Old Entry %d\n",Current_FT2_Entry,Current_FT2_Entry-1);
	printf("Old Entry Results\n");
	printf("DigiTime=%20.18e \n",DigiTime);
	printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",
	       FT2.DT.Tstart_LiveTime[Current_FT2_Entry-1],FT2.DT.Tstop_LiveTime[Current_FT2_Entry-1]);
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
		 <<FT2.DT.DeadTime[Current_FT2_Entry-1]
		 <<endl
		 <<"EvID_Tstart_Live "
		 <<EvID_Tstart_Live
		 <<" EvID_Tstop_Live "
		 <<EvID_Tstop_Live	
		 <<std::endl;
      }
    }
    
    //--- if a new bin start ------
    if((first_bin)||(New_FT2_Entry)){
      
      first_bin=false;
      
      printf("New Entry %d\n",Current_FT2_Entry);
      cout<<"Digi File at " << double(Digi_i)/double(Digi_nEvt)*100.0 <<"% "<<endl;
      
      //Set true update value
      FT2.DT.update[Current_FT2_Entry]=1;
      
      //Update DigiTstart and others
      FT2.DT.Tstart[Current_FT2_Entry]=DigiTime; 
      
	
      
      //------- Update LiveTime ----------------------------
      FT2.DT.Tstart_LiveTime[Current_FT2_Entry]=Current_LiveTime;;
      LiveTime_Tstart = Current_LiveTime;
     
      
      EvID_Tstart_Live=Digi_EvtId;
      
      New_FT2_Entry=0;
      Total_Dead_Time+=DeadTime;	
      DeadTime=0;
      
      if(FT2.verbose){ 
	printf("TstartLiveTime %20.18e CurrentLive %20.18e \n",FT2.DT.Tstart_LiveTime[Current_FT2_Entry],Current_LiveTime);
	std::cout<<"New FT2 Entry, #="
		 <<Current_FT2_Entry
		 <<" DigiTime="
		 <<DigiTime
		 <<"FT2 Id="
		 <<FT2.Get_FT2_Time_Bin(DigiTime,Tstart_Run)
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
    
  }
  //--------  END OF LOOPING DIGI FILE -------------------------------------------------
  
  //Print last Entry Values
  printf("Old Entry %d\n",Current_FT2_Entry);
  printf("Old Entry Results\n");
  printf("DigiTime=%20.18e \n",DigiTime);
  printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",
	 FT2.DT.Tstart_LiveTime[Current_FT2_Entry],FT2.DT.Tstop_LiveTime[Current_FT2_Entry]);
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
	   <<FT2.DT.DeadTime[Current_FT2_Entry]
	   <<endl
	   <<"EvID_Tstart_Live "
	   <<EvID_Tstart_Live
	   <<" EvID_Tstop_Live "
	   <<EvID_Tstop_Live	
	   <<std::endl;
    
    
  
    
    unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
    std::cout<<"======================================================"<<std::endl;
    if(FT2.verbose){ 
      for(unsigned int i=0;i<FT2_ENTR;i++){
	printf("Current Entry %d\n",i);
	printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",
	       FT2.DT.Tstart_LiveTime[i],FT2.DT.Tstop_LiveTime[i]);
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
		 <<FT2.DT.DeadTime[i]
		 <<"\n--------------------------------------------------------"
		 <<std::endl;
      }
      std::cout<<"======================================================"<<std::endl;
    }
    
    
    //----- Evaluate Live Time and tacking into account edge effects --------
    FT2.Evaluate_Live_Time(FT2);
    //-----------------------------------------------------------------------
    
    
    //------------------check-up------------------------------------
    FT2_Live_Time=0;
    for(unsigned int i=0;i<FT2_ENTR;i++){
       FT2_Live_Time+=FT2.FT2_T.LiveTime[i];
       Total_Dead_Time+=FT2.DT.DeadTime[i];
    }
    FT2_Live_Time1=Last_Live_Time-First_Live_Time;
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("Last_Live_Time=%30.28e First_Live_Time=%30.28e \n",Last_Live_Time,First_Live_Time);
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



    //------------------------------------------------------------
    
    //FT2.RunCut(FT2,Tstart_Run ,Tstop_Run);

    FT2.DT.RunStart=Tstart_Run;
    FT2.DT.RunStop=Tstop_Run;
    
    
    //--------------Write FT2 fits file---------------------------
    FT2.WriteTextFile(FT2);


    //--------------Write FT2 fits file---------------------------
    FT2.WriteFitsFile(FT2);
    
    //return(0);
} 
