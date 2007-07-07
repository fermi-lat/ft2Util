/** @file FT2_Time.cxx
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

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
FT2_Time::FT2_Time()
{
}

void FT2_Time::Set_FT2Time_Size(FT2_Time &FT2_T, unsigned int size)
{
  FT2_T.Tstart.resize(size);
  FT2_T.Tstop.resize(size);
  FT2_T.LiveTime.resize(size);
  FT2_T.bin.resize(size);
}


 

double FT2_Time::Get_Tstart( unsigned int  timebin ){
  return double(timebin)*30.0;
}

double FT2_Time::Get_Tstop(  unsigned int  timebin ){
  return double(timebin)*30.0 + 30.0;
}

//------------------------------------------------------------
//------------------------------------------------------------





//---------------- DigiTime CLASS ----------------------------------
DigiTime::DigiTime()
{
  
}

void DigiTime::Set_DigiTime_Size(DigiTime &DigiT, unsigned long size){
  DigiT.Tstart.resize(size);
  DigiT.Tstop.resize(size);
  DigiT.Tstart_LiveTime.resize(size);
  DigiT.Tstop_LiveTime.resize(size);
  DigiT.LiveTime.resize(size);
  DigiT.DeadTime.resize(size);
  DigiT.update.resize(size);
  //DigiT.bin.resize(size);
  //DigiT.Tstart_evt.resize(size);
  //DigiT.Tstop_evt.resize(size); 
}
//------------------------------------------------------------
//------------------------------------------------------------




//---------------- ATTITTUDE CLASS --------------------------------
ATTITUDE::ATTITUDE()
{
}

void ATTITUDE::Set_ATT_Size(ATTITUDE &Att, unsigned int size){
  Att.x.resize(size);
  Att.y.resize(size);
  Att.z.resize(size);
  Att.w.resize(size);
  Att.vx.resize(size);
  Att.vy.resize(size);
  Att.vz.resize(size);
  Att.entr.resize(size);
}






//----------------------------------------------------------
//------------------------------------------------------------




//-------------------- ORBIT CLASS ------------------------------
ORBIT::ORBIT()
{
}
void ORBIT::Set_ORB_Size(ORBIT &Orb, unsigned int size){
  Orb.x.resize(size);
  Orb.y.resize(size);
  Orb.z.resize(size);
  Orb.vx.resize(size);
  Orb.vy.resize(size);
  Orb.vz.resize(size);
  Orb.CM.resize(size);
  Orb.SAA.resize(size);
  Orb.entr.resize(size);
}





//------------------------------------------------------------
//------------------------------------------------------------





//-------------------- FT2_CLASS  ---------------------------------------------------------------------
FT2::FT2()
{
}


//-------------------EVALUATE LIVE TIME -----------------------
void FT2::Evaluate_Live_Time(FT2 &FT2){
  
  unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
  double fraction;//fraction_tot=0;
  
  for(unsigned int i=0;i<FT2_ENTR;i++){
    printf("Entry %d \n",i);
    //FT2.FT2_T.LiveTime[i]=0;

    if(FT2.DT.update[i]){
      std::cout<<"upadte "
	       <<i
	       <<std::endl;
      FT2.FT2_T.LiveTime[i]=
	(FT2.DT.Tstop_LiveTime[i] -  FT2.DT.Tstart_LiveTime[i]) - FT2.DT.DeadTime[i]; 
      std::cout<<"Tstart "
	       <<FT2.FT2_T.Tstart[i]
	       <<"\nTstop "
	       <<FT2.FT2_T.Tstop[i]
	       <<"\nTstart Live " 
	       <<FT2.DT.Tstart_LiveTime[i]
	       <<"\nTstop Live "
	       <<FT2.DT.Tstop_LiveTime[i] 
	       <<"\nlive "<<FT2.FT2_T.LiveTime[i]
	       <<std::endl;;
    
    
      if(FT2.DT.update[i+1] && i<FT2_ENTR-1){
	fraction=(FT2.FT2_T.Tstop[i]-FT2.DT.Tstop[i])/(FT2.DT.Tstart[i+1]-FT2.DT.Tstop[i]);
	fraction*=(FT2.DT.Tstart_LiveTime[i+1]-FT2.DT.Tstop_LiveTime[i]);
	std::cout<<"fraction fwd "
		 <<fraction
		 <<"\n";
	//printf("%e %e\n",FT2.DT.Tstart_LiveTime[i+1],FT2.DT.Tstop_LiveTime[i]);
	FT2.FT2_T.LiveTime[i]+= fraction;
	//fraction_tot+=fraction;
      }
    
      if(FT2.DT.update[i-1] && i>0){
	fraction=1.0/(FT2.DT.Tstart[i]-FT2.DT.Tstop[i-1]);
	fraction*=(FT2.DT.Tstart[i]-FT2.FT2_T.Tstart[i]);
	fraction*=(FT2.DT.Tstart_LiveTime[i]-FT2.DT.Tstop_LiveTime[i-1]);
	std::cout<<"fraction back "
		 <<fraction
		 <<"\n";
	//printf("%e %e\n",FT2.DT.Tstart_LiveTime[i+1],FT2.DT.Tstop_LiveTime[i]);
	FT2.FT2_T.LiveTime[i]+= fraction;
	//fraction_tot+=fraction;
      }
      
      
    }


    std::cout<<"live "
	     <<std::setprecision(20)
	     <<FT2.FT2_T.LiveTime[i]
	     <<std::endl 
	     <<"-------------------------------------------------------------------------"
	     <<std::endl;
    //------ inter bin correction --------
    
  }
  
  
}




//-------------------Get Files Names -------------------------
void FT2::getFileNames(int iargc, char * argv[]) {
  if (iargc < 5) {
      std::cout << "usage: " 
	        << "<DigiFile> " 
	        << "<MeritFile> " 
		<< "<M7File> " 
		<< "<FT2_txt out file> "
		<< std::endl;
      std::exit(0);
   } else {
      DigiFile = std::string(argv[1]);
      MeritFile = std::string(argv[2]);
      M7File = std::string(argv[3]);     
      FT2_txt_File = std::string(argv[4]);
   }
}




//-----------------Line Number Counter-------------------------
unsigned int  FT2::LineNumberCount(const std::string & infile){
  std::ifstream file(infile.c_str());
  //unsigned long Lines;
  unsigned int nlines(0);
  std::string line;
  //std::cout<<infile<<std::endl;
  while (std::getline(file, line, '\n')) {
   nlines++;
  }
 
  file.close();
  //std::cout<< nlines <<std::endl;
  return nlines;
} 

void FT2::Get_DigiFileLineNumber(FT2 &FT2,const std::string & infile){
  //std::cout<<infile<<std::endl ;
  FT2.DigiFileLineNumber=LineNumberCount(infile);
}

 

 
//------------------------------ Merge M7 and Digi Entries --------------------------------------------
void FT2::Merge_M7_Digi_Entries(FT2 &FT2,double Tstart_Run ,double Tstop_Run){
  unsigned int Current_FT2_Entry,FT2_Entries;

  printf("---------------------------------------------------------\n");
  printf("Merge M7 with Digi entries\n");

  //add new entry
  printf("FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2, Tstart_Run,Current_FT2_Entry);
  printf("Entry index=%d \n",Current_FT2_Entry);
  FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
  FT2_Entries= Get_FT2_Entries(FT2);
  FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
  printf("add entry, FT2 entries %d \n",Get_FT2_Entries(FT2));

  FT2.FT2_T.Tstart[Current_FT2_Entry]=FT2.FT2_T.Tstart[Current_FT2_Entry];
  FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entry];
  FT2.FT2_T.Tstop[Current_FT2_Entry]=Tstart_Run ;
  FT2.FT2_T.Tstart[FT2_Entries-1]=Tstart_Run ;


  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());
 

  //add new entry
  printf("FT2 entris %d \n",Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2,Tstop_Run,Current_FT2_Entry);
  printf("Entry index=%d \n",Current_FT2_Entry);
  FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
  FT2_Entries= Get_FT2_Entries(FT2);
  FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
  printf("add entry, FT2 entries %d \n",Get_FT2_Entries(FT2));

  FT2.FT2_T.Tstart[Current_FT2_Entry]=FT2.FT2_T.Tstart[Current_FT2_Entry];
  FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entry];
  FT2.FT2_T.Tstop[Current_FT2_Entry]=Tstop_Run ;
  FT2.FT2_T.Tstart[FT2_Entries-1]=Tstop_Run ;

  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());

  //SORTING
  // 
  printf("---------------------------------------------------------\n");

  printf("---------------------------------------------------------\n");
  for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){
     FT2.FT2_T.LiveTime[i]=0;
     FT2.FT2_T.bin[i]=i;
     printf("Entry Id=%d Tstart=%20.18g  Tstop=%20.18g\n",i,FT2.FT2_T.Tstart[i],FT2.FT2_T.Tstop[i]);
  }
  printf("---------------------------------------------------------\n");
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



//---------------Get FT2 Entry Index    ---------------------------------
void FT2::Get_FT2_Entry_Index(FT2 &FT2 ,double time, unsigned int &i){
  unsigned Entries ;

  Entries=Get_FT2_Entries(FT2);

  FT2.Set_OutOfRange_TRUE(FT2);

  for(unsigned int l=0;l<Entries-1;l++){
    if((time<FT2.FT2_T.Tstop[l])&&(time>=FT2.FT2_T.Tstart[l])){
      Set_OutOfRange_FALSE(FT2);
      i=l;
    }  
  }
  l=Entries-1;
  if((time<=FT2.FT2_T.Tstop[l])&&(time>=FT2.FT2_T.Tstart[l])){
    Set_OutOfRange_FALSE(FT2);
      i=l;
  }

  if (FT2.Get_OutOfRange(FT2)){
  std::cout<<"!!!Warning "
	   <<"data out of any Bin time interval "  
	   <<"time "
	   <<std::setprecision(20)
	   <<time
	   <<" Tstart of First Bin "
	   <<FT2.FT2_T.Tstart[0]
	   <<" Tstop of Last Bin "
	   <<FT2.FT2_T.Tstop[FT2.FT2_T.Tstop.size()-1]
	   <<std::endl;
  }
}



//----------------------------------------------------------------------------
int FT2::Get_FT2_Time_Bin(double time,double Tstart){
  unsigned int i;
  
  i=int((time-Tstart)/30.0);
  //std::cout<<"bin "
  //    <<i
  //    <<std::endl;
    
  return i;
}






//-------------------------------Get M-7 Time  -------------------------------------------

double  FT2::Get_M7_Time(const std::string &Time, const std::string &Frac_Time){
  double time ,f_time;
  
  time=atof(Time.c_str());
  f_time=atof(Frac_Time.c_str());
  f_time*=1e-6;
  
  return (time+f_time);
}


//--------------------------------Set Entries According to M7 file -----------------------
void FT2::Set_M7_Entries(FT2 &FT2){

  double time,Tstart;
  //int new_entry(1);
  unsigned int M7LineCounter(0);
  int  TimeBin, NewTimeBin(0),OldTimeBin(0),MODE(0),OLD_MODE;
  bool first_orb_entry(true);
  unsigned int Current_FT2_Entries(0);
  
  //File Handlign
  std::string buf,comment; //buffer string
  std::string line;
  std::ifstream M7F(FT2.M7File.c_str());
 
  printf("----------- Set FT2 Entries from M7 file -----------\n");
  
  
  //Read M-7 File 
  while (std::getline(M7F, line, '\n')) {
    //A SIMPLE TOKENIZER
    std::vector<std::string> tokens; // Create vector to hold our words
    std::stringstream ss(line); // Insert the string into a stream  
    while (ss >> buf)	tokens.push_back(buf);

    //SKIP lines that start with # character
    comment=line.substr(0,1);
    if(comment.find( "#", 0) == std::string::npos ){

    time=FT2.Get_M7_Time(tokens[3],tokens[4]);

    if (M7LineCounter==0){
      Tstart=time;
    }

    //-------FT2 time bin Id-----------------
    TimeBin=FT2.Get_FT2_Time_Bin(time,Tstart);
    

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
	printf("---------------------------------------------------------\n");
	NewTimeBin=1;
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
    

    //------------ Make a new Entry if Time Bin Changes ----------------------
    if(TimeBin!=OldTimeBin && M7LineCounter>0){
      printf("---------------------------------------------------------\n");
      NewTimeBin=1;
      std::cout<<"Previous Entry Time Id "
	       <<std::setprecision(20)
	       <<FT2.FT2_T.bin[Current_FT2_Entries-1]
	       <<" FT2 Tstart "
	       <<FT2.FT2_T.Tstart[Current_FT2_Entries-1]
	       <<" FT2 Tstop "
 	       <<FT2.FT2_T.Tstop[Current_FT2_Entries-1]
	       <<std::endl;
    } 
    
    
   
    


    //IF we are at the beginning of a new TimeBin or
    //at the start UpadteSomeThing
    if((M7LineCounter==0)||(NewTimeBin)){
      

      
      //UPADTE THE NUMBER OF ENTRIES IN THE FT2 CLASS
      Current_FT2_Entries++;
      FT2.Update_FT2_Entries(FT2,Current_FT2_Entries);

      
      //Resize FT2_Time class 
      FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,Current_FT2_Entries);
      FT2.FT2_T.bin[Current_FT2_Entries-1]=TimeBin;
  
      //Update FT2_Time class
      if(M7LineCounter==0){
	FT2.FT2_T.Tstart[Current_FT2_Entries-1]=time;
      }
      else{ 
	FT2.FT2_T.Tstart[Current_FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entries-2]; 
      }
      
      


      //Live Time set to zero!!!
      FT2.FT2_T.LiveTime[Current_FT2_Entries-1]=0;
      
      NewTimeBin=0;

        std::cout<<"New Entry, Current number of  FT2 Entries "
	       <<Current_FT2_Entries
	       <<" Current Entry Id"
	       <<FT2.FT2_T.bin[Current_FT2_Entries-1]
		 <<" M7 line "
	       <<M7LineCounter
	       <<std::endl; 
	std::cout<<Tstart <<" +"<<time<<std::endl;


      
    }
    else{
      FT2.FT2_T.Tstop[Current_FT2_Entries-1]=time; 
    }
 
    //SWAPPING
    OldTimeBin=TimeBin; 
    OLD_MODE=MODE;
    
    
    M7LineCounter++;
    }
  }
  
  M7F.close();
 
  //for (unsigned int i = 0; i < FT2.FT2_T.Tstart.size(); ++i){ 
  //  printf("%d Tstart=%20.18g  Tstop=%20.18g\n",i,FT2.FT2_T.Tstart[i],FT2.FT2_T.Tstop[i]);
  //}
  
}




//--------------  Fill M7 Entries in M7 File ----------------------------------
void FT2::Fill_M7_Entries(FT2 &FT2){

  double time,Tstart;
  //int new_entry(1);
  unsigned int M7LineCounter(0);
  bool  NewEntry(false);
  unsigned int FT2_Entries(0),Current_FT2_Entry(0),Old_FT2_Entry(0);

  //File Handlign
  std::string buf,comment; //buffer string
  std::string line;
  std::ifstream M7F(FT2.M7File.c_str());
  
  printf("---------------------------------------------------------\n");
  printf("----------- Fill M7 Entries from  M7 file -----------\n");
 
  
  //Resize ATT & ORB
  FT2_Entries=Get_FT2_Entries(FT2);
  FT2.ATT.Set_ATT_Size(FT2.ATT,FT2_Entries);
  FT2.ORB.Set_ORB_Size(FT2.ORB,FT2_Entries);
  

  //Read M-7 File 
  while (std::getline(M7F, line, '\n')) {
    //A SIMPLE TOKENIZER
    std::vector<std::string> tokens; // Create vector to hold our words
    std::stringstream ss(line); // Insert the string into a stream  
    while (ss >> buf)	tokens.push_back(buf);

    //SKIP lines that start with # character
    comment=line.substr(0,1);
    if(comment.find( "#", 0) == std::string::npos ){



    time=FT2.Get_M7_Time(tokens[3],tokens[4]);


    if (M7LineCounter==0){
      Tstart=time;
    }



    //FT2 Entry Index
    Get_FT2_Entry_Index(FT2,time,Current_FT2_Entry);
   
    if(Current_FT2_Entry!=Old_FT2_Entry && M7LineCounter>0){
      NewEntry=true;
      std::cout<<"Current Entry "
	       <<Current_FT2_Entry 
	       <<" Current Entry Id "
	       <<std::setprecision(20)
	       <<FT2.FT2_T.bin[Current_FT2_Entry]
	       <<" FT2 Tstart "
	       <<FT2.FT2_T.Tstart[Current_FT2_Entry]
	       <<" FT2 Tstop "
	       <<FT2.FT2_T.Tstop[Current_FT2_Entry]

	       <<std::endl;
    }    
    //IF we are at the beginning of a new TimeBin or
    //at the start UpadteSomeThing
    if((M7LineCounter==0)||(NewEntry)){
      NewEntry=false;
      FT2.Clean_ATT_Quaternions(FT2.ATT,Current_FT2_Entry);
      FT2.Clean_ORB(FT2.ORB,Current_FT2_Entry);
    }
    
    if (tokens[2]=="ATT"){
      FT2.Update_ATT_Quaternions(FT2.ATT,tokens,Current_FT2_Entry);
    }
    if (tokens[2]=="ORB"){ 
      FT2.Update_ORB(FT2.ORB,tokens,Current_FT2_Entry);
    }
    
    
    //SWAPPING
    Old_FT2_Entry=Current_FT2_Entry; 
    
    
    M7LineCounter++;

    }
  }
  


  M7F.close();
  
  FT2.Average_M7_Entries(FT2);
  printf("---------------------------------------------------------\n");

}



//-------------- Average M7 File  Entries  within a FT2 time bin -----------------------
void FT2::Average_M7_Entries(FT2 &FT2){
  
  for (unsigned int i = 0; i < FT2.ATT.entr.size(); ++i){ 
    std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
    FT2.ATT.x[i]*=1.0/(double( FT2.ATT.entr[i]));
    FT2.ATT.y[i]*=1.0/(double( FT2.ATT.entr[i]));
    FT2.ATT.z[i]*=1.0/(double( FT2.ATT.entr[i]));
    FT2.ATT.w[i]*=1.0/(double( FT2.ATT.entr[i]));
    FT2.ATT.vx[i]*=1.0/(double( FT2.ATT.entr[i]));
    FT2.ATT.vy[i]*=1.0/(double( FT2.ATT.entr[i]));
    FT2.ATT.vz[i]*=1.0/(double( FT2.ATT.entr[i]));
  }

  for (unsigned int i = 0; i < FT2.ORB.entr.size(); ++i){
    std::cout<<"ORB elements in Entry "<<i<<","<<FT2.ORB.entr[i]<<"\n";

    if( FT2.ORB.entr.size()==0){
      //Make interpolation
      //FT2.interpolate(FT2.ORB);
    }
    
    FT2.ORB.x[i]*=1.0/(double(FT2.ORB.entr[i]));
    FT2.ORB.y[i]*=1.0/(double(FT2.ORB.entr[i]));
    FT2.ORB.z[i]*=1.0/(double(FT2.ORB.entr[i]));
    FT2.ORB.vx[i]*=1.0/(double(FT2.ORB.entr[i]));
    FT2.ORB.vy[i]*=1.0/(double(FT2.ORB.entr[i]));
    FT2.ORB.vz[i]*=1.0/(double(FT2.ORB.entr[i]));
  }
}


//-------------- Update and clean M7 fields in the FT2 Entries -----------------------
void FT2::Clean_ATT_Quaternions(ATTITUDE &Att, unsigned int entry){      
  Att.entr[entry]=0;
  Att.x[entry]=0;
  Att.y[entry]=0;
  Att.z[entry]=0;
  Att.w[entry]=0;
  Att.vx[entry]=0;
  Att.vy[entry]=0;
  Att.vz[entry]=0;
    
}

void FT2::Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens, unsigned int entry){     
  Att.entr[entry]++;
  Att.x[entry]+=atof(tokens[5].c_str());
  Att.y[entry]+=atof(tokens[6].c_str());
  Att.z[entry]+=atof(tokens[7].c_str());
  Att.w[entry]+=atof(tokens[8].c_str());
  Att.vx[entry]+=atof(tokens[9].c_str());
  Att.vy[entry]+=atof(tokens[10].c_str());
  Att.vz[entry]+=atof(tokens[11].c_str());
}


void FT2::Clean_ORB(ORBIT &Orb, unsigned int entry){  
  Orb.entr[entry]=0;
  Orb.x[entry]=0;
  Orb.y[entry]=0;
  Orb.z[entry]=0;
  Orb.vx[entry]=0;
  Orb.vy[entry]=0;
  Orb.vz[entry]=0;
  Orb.CM[entry]=0;
  Orb.SAA[entry]=0;
}


void FT2::Update_ORB(ORBIT &Orb,const std::vector<std::string> &tokens, unsigned int entry){  
  Orb.entr[entry]++;
  Orb.x[entry]+=atof(tokens[5].c_str());
  Orb.y[entry]+=atof(tokens[6].c_str());
  Orb.z[entry]+=atof(tokens[7].c_str());
  Orb.vx[entry]+=atof(tokens[9].c_str());
  Orb.vy[entry]+=atof(tokens[9].c_str());
  Orb.vz[entry]+=atof(tokens[10].c_str());

  //These Entries are updated only the first time
  if (Orb.entr[entry]==1){
    Orb.CM[entry]+=atoi(tokens[11].c_str());
    Orb.SAA[entry]+=atoi(tokens[12].c_str());
  }

}











