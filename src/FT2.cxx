
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
      std::cout<<"live "<<FT2.FT2_T.LiveTime[i]<<std::endl;;
    
    
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
  
  //double diff=0;
  //for(unsigned int i=1;i<FT2_ENTR-1;i++){
 
  //    if(FT2.DT.update[i]){
  //   printf("Entry %d \n",i);
  //   diff+=FT2.DT.Tstart_LiveTime[i]-FT2.DT.Tstop_LiveTime[i-1];  
  // }
  // }
  //printf("diff=%30.28e fraction_tot=%30.28e\n",diff,fraction_tot);
  printf("FT2.DT.Tstop_LiveTime[10]=%30.28e FT2.DT.Tstart_LiveTime[0]=%30.28e \n",FT2.DT.Tstop_LiveTime[10],FT2.DT.Tstart_LiveTime[0]);
  printf("FT2.DT.Tstop_LiveTime[10]-FT2.DT.Tstart_LiveTime[0] live=%30.28e\n",FT2.DT.Tstop_LiveTime[10]-FT2.DT.Tstart_LiveTime[0]);
}
//-------------------------------------------------------------



//-------------------Files -------------------------
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

void FT2::Get_DigiFileLineNumber(const std::string & infile){
  //std::cout<<infile<<std::endl ;
  DigiFileLineNumber=LineNumberCount(infile);
}



 
//------------------------------  FT2_CLASS FT2 --------------------------------------------

//Update the private filed Entries
void FT2::Update_FT2_Entries(FT2 &FT2, int i){
  FT2.Entries=i;
}

unsigned int FT2::Get_FT2_Entries(FT2 &FT2){
  return FT2.Entries;
}

//Update the fields of the FT2_Time Class
void FT2::Update_FT2_Time(FT2 &FT2, unsigned int Current_FT2_Entries ,unsigned int TimeBin){
  
  FT2.FT2_T.Tstart[Current_FT2_Entries-1]=FT2.FT2_T.Get_Tstart(TimeBin);
  FT2.FT2_T.Tstop[Current_FT2_Entries-1]=FT2.FT2_T.Get_Tstop(TimeBin);
  FT2.FT2_T.bin[Current_FT2_Entries-1]=TimeBin;
}




int FT2::Get_FT2_Entry_Index(FT2 &FT2 ,double time){
  unsigned int i, Entries ;
  Entries=Get_FT2_Entries(FT2);
  for(i=0;i<Entries;i++){
    if((time<=FT2.FT2_T.Tstop[i])&&(time>=FT2.FT2_T.Tstart[i])){
      return i;
    }  
  }
  std::cout<<"M7 data out of any Bin time interval "  
	   << i
	   <<"time "
	   <<std::setprecision(20)
	   <<time
	 <<std::endl;
 exit(1);
}

int FT2::Get_FT2_Time_Bin(double time){
  unsigned int i;
  
  i=int(time/30.0);
  //std::cout<<"bin "
  //    <<i
  //    <<std::endl;
    
  return i;
}




//------------------------------- FT2_CLASS M-7 -------------------------------------------

double  FT2::Get_M7_Time(const std::string &Time, const std::string &Frac_Time){
  double time ,f_time;
  
  time=atof(Time.c_str());
  f_time=atof(Frac_Time.c_str());
  f_time*=1e-6;
  
  return (time+f_time);
}


//Handle M7 Entries in M7 File

void FT2::Handle_M7_Entries(FT2 &FT2){

  double time;
  //int new_entry(1);
  unsigned int M7LineCounter(0);
  int  TimeBin, NewTimeBin(0),OldTimeBin(0);
  unsigned int Current_FT2_Entries(0);

  //File Handlign
  std::string buf,comment; //buffer string
  std::string line;
  std::ifstream M7F(FT2.M7File.c_str());
    

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

    //Absolute FT2 time bin Id
    TimeBin=FT2.Get_FT2_Time_Bin(time);
    
    //std::cout<<"TimeBin"
    //	   <<TimeBin
    //	   <<"OldTimeBin"
    //	   <<OldTimeBin
    //	   <<std::endl;
    
    //IF a new TimeBin has started and
    //it is not the first!!!!!
    //print information about the previously 
    //completed TimeBin
    if(TimeBin!=OldTimeBin && M7LineCounter>0){
      NewTimeBin=1;
      std::cout<<"New Time bin "
	       <<Current_FT2_Entries
	       <<" Abd Time Id "
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
      std::cout<<"New Bin  " 
	       <<"Current FT2 Entries "
	       <<Current_FT2_Entries
	       <<"M7 line "
	       <<M7LineCounter
	       <<std::endl; 
      
      
    
      //Resize FT2_Time class 
      FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,Current_FT2_Entries);
      
      //Update FT2_Time class
      FT2.Update_FT2_Time(FT2,Current_FT2_Entries,TimeBin);
      
      //Live Time set to zero!!!
      FT2.FT2_T.LiveTime[Current_FT2_Entries-1]=0;
      

      
      
      //Resize ATT & ORB
      FT2.ATT.Set_ATT_Size(FT2.ATT,Current_FT2_Entries);
      FT2.ORB.Set_ORB_Size(FT2.ORB,Current_FT2_Entries);
      
      NewTimeBin=0;
    }
    
    
    
    if (tokens[2]=="ATT"){
      FT2.Update_ATT_Quaternions(FT2.ATT,tokens,Current_FT2_Entries-1);
    }
    if (tokens[2]=="ORB"){ 
    FT2.Update_ORB(FT2.ORB,tokens,Current_FT2_Entries-1);
    }
    
    
    //SWAPPING
    OldTimeBin=TimeBin; 
    
    
    M7LineCounter++;

    }
  }

  M7F.close();
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




void FT2::Update_ORB(ORBIT &Orb,const std::vector<std::string> &tokens, unsigned int entry){

  
  Orb.entr[entry]++;
  Orb.x[entry]+=atof(tokens[5].c_str());
  Orb.y[entry]+=atof(tokens[6].c_str());
  Orb.z[entry]+=atof(tokens[7].c_str());
  Orb.vx[entry]+=atof(tokens[9].c_str());
  Orb.vy[entry]+=atof(tokens[9].c_str());
  Orb.vz[entry]+=atof(tokens[10].c_str());
  Orb.CM[entry]+=atoi(tokens[11].c_str());
  Orb.SAA[entry]+=atoi(tokens[12].c_str());

}




//----------------------------------------------------------------------
//----------------------------------------------------------------------





