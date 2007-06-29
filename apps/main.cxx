/** @file FT2_Time.cxx
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

*/

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








int main(int argc, char **argv){

  //--------------------------- ROOT DECLARATIONS ---------------------------------
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
  //--------------------------------------------------------------------------------





  //------------------------ FT2 DECLARATIONS ---------------------------------
  //unsigned int DigiLineCounter(0), M7LineCounter(0);
  double DigiTime, Current_LiveTime,Old_LiveTime, LiveTime_Tstart, LiveTime_Tstop;
  double DeadTime(0);
  unsigned int New_FT2_Entry(0),Current_FT2_Entry,Old_FT2_Entry;
  unsigned int Digi_i,Merit_i;

  double begin_edge_correction(0),end_edge_correction(0),bin_edge_live_fraction(0);

  std::string buf,comment; //buffer string
  std::string line;
  
  //------ MAKE FT2 FILES ----------
  FT2 FT2;
  
  //--- strings for files ---
  std::string MeritFile;
  std::string DigiFile;
  std::string M7File;
  std::string FT2_txt_File;
  
  //--- Get File Names ---
  FT2.getFileNames(argc, argv);
  
  std::cout<<"Digi File "
	   <<FT2.DigiFile<<std::endl
	   <<"MeritFile"
	   <<FT2.MeritFile<<std::endl
	   <<"M7 File "
	   <<FT2.M7File<<std::endl
	   <<"OUT FILE"
	   <<FT2.FT2_txt_File<<std::endl;
  

  std::ofstream FT2F(FT2.FT2_txt_File.c_str());
  std::ofstream out("Digi-out.txt");

  //-----------------------------------------------------------------------------------





  //---------------------------------------------------------------------------------------------------------
  //-------- WORK ON  M7 FILE -------------------------------------------------------------------------------
  
  out<<"M-7 file"<<endl;
  //SET ATT AND ORB SIZE TO FT2_ENTRIES
  //FT2.ATT.Set_ATT_Size(FT2.ATT,FT2.Get_FT2_Entries(FT2));
  //FT2.ORB.Set_ORB_Size(FT2.ORB,FT2.Get_FT2_Entries(FT2));
   
  
  //Read M-7 File 
  
  FT2.Handle_M7_Entries(FT2);
   
  std::cout<<"======================================================"<<std::endl;
  
  //---------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------




  //---------------------------------  STAT HANDLING ROOT DIGI & MERIT FILE ---------------------------------
  //---------------------------------------------------------------------------------------------------------


  
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


  //----------------------------------------------------------
  //---------------- ROOT Start TTree Merit ------------------
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


  if(Merit_nEvt != Digi_nEvt  ){
    std::cout<<"Different numeber of entries in Digi and Merit File !!!!!!!"<<std::endl;
  }
  

  //Digi File Line Numbers
  std::cout<<"DigiFile has "<<   Digi_nEvt <<" events" <<std::endl;
  
  //ROOT LOPPING OVER EVENTS
  out<<"#Digi_I Digi_EvtID Merit Evt_ID TSTAMP            LIVE  s             ELAPS  s           LIVE     ELAPS    DEAD    DeltaEl DeltaDead "<<endl;
  


  //-------------------------------------------  LOOP OVER DIGI & MERIT FILE ------------------------------------------------

  //Check-up variables
  double First_Live_Time, Last_Live_Time;
  double Digi_Live_Time,FT2_Live_Time,FT2_Live_Time1;
  double Total_Dead_Time(0);
  unsigned int EvID_Tstart_Live, EvID_Tstop_Live;
  
  Merit_i=0;
  //Digi_nEvt=150000;
 
  //Set FT2.Digi_Time_Size
   FT2.DT.Set_DigiTime_Size(FT2.DT,FT2.Get_FT2_Entries(FT2)+1);
  
  
  for (Digi_i = 0; Digi_i < Digi_nEvt ; Digi_i++){
    
    // Get the Entries;
    T->GetEntry(Digi_i);
    MT->GetEntry(Merit_i);
    
    Digi_EvtId=evt->getEventId(); 
    // Merit_EvtId


    //Simulating a Crash!!!
    //if(Digi_EvtId==473612){
    //  Merit_i+=10000; 
    //}
    
   

    
    currentElapsed=evt->getMetaEvent().scalers().elapsed();
    currentLive=evt->getMetaEvent().scalers().livetime(); 
    currentDeadTime = currentElapsed - currentLive;
    
    if(Digi_i==0){
      previousElapsed=currentElapsed;
      previousDeadTime=currentDeadTime;
    }
  
    
    deltaElapsed=currentElapsed-previousElapsed;
    deltaDeadTime = currentDeadTime-previousDeadTime;
    
    
    
    //TRIKY TYPECASTING
    sprintf(number_string,"%llu",currentLive);
    curr_live=strtod(number_string,NULL);
    
  
    //TRIKY TYPECASTING
    sprintf(number_string,"%llu",currentElapsed);
    curr_elapsed=strtod(number_string,NULL);
    
    
    //ROOT TXT OUT FILE
    if ( Digi_i % 10000 == 0 ) {
      cout<<"Digi File at " << double(Digi_i)/double(Digi_nEvt)*100.0 <<"% "<<endl;
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
    
    
    
    
    
   
  
    //---------------------   WORK ON FT2 Live Time  -------------------------------------

    //--------!!!!! ASSUMING THAT THE ENTRIES IN DIGI FILE ARE TIME ORDERED !!!!!---------

    //-------- IF DigiEvtId==MeritEvtId -> Increse Live time && Increase both Digi(that is done by for loop) and Merit index (manually )
    
    DigiTime=evt->getTimeStamp();
    Current_LiveTime=curr_live*conv;
    Current_FT2_Entry=FT2.Get_FT2_Entry_Index(FT2,DigiTime);
    
    if(Digi_i==0){
 
      //check-up
      First_Live_Time=Current_LiveTime;
      Old_LiveTime = Current_LiveTime;
    }
 
   
    
    if(Digi_EvtId!=Merit_EvtId){
      //DeadTime+=Current_LiveTime-Old_LiveTime;
      FT2.DT.DeadTime[Current_FT2_Entry]+=Current_LiveTime-Old_LiveTime;			
     }
    
    
    if(Current_FT2_Entry!=Old_FT2_Entry && Digi_i>0){

      New_FT2_Entry=1;
 

      //Correction for the dead time given by crush
      FT2.FT2_T.LiveTime[Current_FT2_Entry-1]=(FT2.DT.Tstop_LiveTime[Current_FT2_Entry-1] -  FT2.DT.Tstart_LiveTime[Current_FT2_Entry-1]);
    

      

      //Print last Entry Values
      printf("Start a New Entry\n");
      printf("Current %d  Old Entry %d\n",Current_FT2_Entry,Current_FT2_Entry-1);
      printf("Old Entry Results\n");
      printf("TstartLiveTime %20.18e TstopLiveTime%20.18e\n",FT2.DT.Tstart_LiveTime[Current_FT2_Entry-1],FT2.DT.Tstop_LiveTime[Current_FT2_Entry-1]);
      std::cout<<setprecision(20)
	       <<"Tstart "
	       <<FT2.DT.Tstart[Current_FT2_Entry-1]
	       <<" Tstop "
	       <<FT2.DT.Tstop[Current_FT2_Entry-1]
	       <<" Delta T "
	       <<FT2.DT.Tstop[Current_FT2_Entry-1] - FT2.DT.Tstart[Current_FT2_Entry-1]
	       <<" Live Time "
	       <<FT2.FT2_T.LiveTime[Current_FT2_Entry-1]
	       <<" Dead Time "
	       <<DeadTime
	       <<endl
	       <<"EvID_Tstart_Live "
	       <<EvID_Tstart_Live
	       <<" EvID_Tstop_Live "
	       <<EvID_Tstop_Live	
	       <<std::endl;
	}
    
    
    if((Digi_i==0)||(New_FT2_Entry)){
      printf("New Entry %d\n",Current_FT2_Entry);
      
      //Set true update value
      FT2.DT.update[Current_FT2_Entry]=1;

      //Update DigiTstart and others
      FT2.DT.Tstart[Current_FT2_Entry]=DigiTime; 
     
      //check-up
      //LiveTime_Tstart = Current_LiveTime;
 
      //Update LiveTime
      //The live time must be evaluated starting from
      //The live time in the las element in the previous bin

      FT2.DT.Tstart_LiveTime[Current_FT2_Entry]=Current_LiveTime;;
      LiveTime_Tstart = Current_LiveTime;
      printf("TstartLiveTime %20.18e CurrentLive %20.18e \n",FT2.DT.Tstart_LiveTime[Current_FT2_Entry],Current_LiveTime);
      
     
      //bin_edge_live_fraction=0;
      cout<<"fraction correction"
	  <<bin_edge_live_fraction
	  <<endl;


      //bin_edge_live_fraction =(FT2.DT.Tstart[Current_FT2_Entry] - FT2.FT2_T.Tstart[Current_FT2_Entry])/(FT2.DT.Tstart[Current_FT2_Entry]-FT2.DT.Tstop[Current_FT2_Entry-1]);
      //begin_edge_correction = - (1.0 - bin_edge_live_fraction)*(Current_LiveTime - Old_LiveTime);


      EvID_Tstart_Live=Digi_EvtId;




      
      
      New_FT2_Entry=0;
      Total_Dead_Time+=	DeadTime;	
      DeadTime=0;
      
      
      std::cout<<"New FT2 Entry, #="
	       <<Current_FT2_Entry
	       <<" DigiTime="
	       <<DigiTime
	       <<" Absolute FT2 Id="
	       <<FT2.Get_FT2_Time_Bin(DigiTime)
	       <<"Digi EvtId"
	       <<Digi_EvtId
	       <<std::endl
	       <<"--------------------------------------------------------"
	       <<std::endl
	       <<std::endl;
    }
    
    
    else{
      //UPDATE TSTOP
      FT2.DT.Tstop[Current_FT2_Entry]=DigiTime;
      
      //UPDATE LIVE TIME
      LiveTime_Tstop = Current_LiveTime; 
      //FT2.FT2_T.LiveTime[Current_FT2_Entry]=(LiveTime_Tstop -  LiveTime_Tstart)-DeadTime;
      FT2.DT.Tstop_LiveTime[Current_FT2_Entry] = Current_LiveTime; 

      //check-up
      LiveTime_Tstop=Current_LiveTime;
      EvID_Tstop_Live=Digi_EvtId;
    }
       
    
    if(Digi_EvtId==Merit_EvtId){
      Merit_i++; 
    }  
    
    
    //-------- IF DigiEvtId!=MeritEvtId -> Increase DeadTime && Increase only Digi Index (that is done by for loop)
    
    
    //SWAPPING
    Old_FT2_Entry=Current_FT2_Entry;  
    Old_LiveTime=Current_LiveTime;
    evt->Clear();  
  }

  //check-up 
  Last_Live_Time=Current_LiveTime;
  
  
  // exit(0);
  //--------  END OF LOOPING DIGI FILE ----------------------------------------------------------
  
  
  
  FT2.Evaluate_Live_Time(FT2);
  
  //----------- WRITE FT2 TXT FILE -----------------------------------------------------------------
  FT2F<<"#Abs_FT2_ID FT2Tstart FT2tstop SC.x SY.y Sc.z lat_geo lon_geo rad_geo ra_zenith dec_zenith b_mcilwain geom_lat is_saa ra_scz dec_scz lat_mode livetime"<<endl;
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);

  //check-up
  FT2_Live_Time=0;

  for(unsigned int i=0;i<FT2_ENTR;i++){
    
    FT2F<<FT2.FT2_T.bin[i]
	<<","
	<<setprecision(20)
	<<scientific
	<<FT2.FT2_T.Tstart[i]
	<<","
	<<FT2.FT2_T.Tstop[i]
	<<","
	<<FT2.ORB.x[i]/double(FT2.ORB.entr[i])
	<<","
	<<FT2.ORB.y[i]/double(FT2.ORB.entr[i])
	<<","
	<<FT2.ORB.z[i]/double(FT2.ORB.entr[i])
	<<","
	<<0
	<<","
	<<0
	<<","
	<<0
	<<","
	<<0
	<<","
	<<0
	<<","
	<<0
	<<","
	<<0
	<<","
	<<FT2.ORB.SAA[i]
	<<","
	<<0
	<<","
	<<0
	<<","
	<<FT2.ORB.CM[i]
	<<","
	<<FT2.FT2_T.LiveTime[i]
	<<std::endl;
    
    FT2_Live_Time+=FT2.FT2_T.LiveTime[i];
    Total_Dead_Time+=FT2.DT.DeadTime[i];
  }
  
  FT2_Live_Time1=Last_Live_Time-First_Live_Time;
  FT2F.close();


  //check-up
  printf("FT2.DT.Tstop_LiveTime[10]=%30.28e FT2.DT.Tstart_LiveTime[0]=%30.28e \n",FT2.DT.Tstop_LiveTime[10],FT2.DT.Tstart_LiveTime[0]);
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
      <<"Last Live Time "
      <<FT2.DT.Tstop_LiveTime[10]
      <<endl
      <<"First Live_Time "
      <<First_Live_Time
      <<endl
      <<"First Live_Time "
      <<FT2.DT.Tstart_LiveTime[0]
      <<endl
      <<"FT2_live_Time -  (Last-First LiveTime) - DeadTime = "
      <<FT2_Live_Time1 - FT2_Live_Time -   Total_Dead_Time   
      <<endl
      <<"Total_Dead_Time "
      <<Total_Dead_Time
      <<endl;  
  return(0);

}

