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

void ATTITUDE::Set_ATT_Size(ATTITUDE &ATT, unsigned int size){
  ATT.Tstart.resize(size);
  ATT.x.resize(size);
  ATT.y.resize(size);
  ATT.z.resize(size);
  ATT.w.resize(size);
  ATT.vx.resize(size);
  ATT.vy.resize(size);
  ATT.vz.resize(size);
  ATT.entr.resize(size);
}

void ATTITUDE::Print_ATT_Entries(ATTITUDE &ATT){
  std::cout<<"ATT elements --- "<<"\n";
for (unsigned int i = 0; i < ATT.entr.size(); ++i){ 
    std::cout
      << i
      <<" "
      <<ATT.Tstart[i]
      <<" "
      << ATT.x[i]
      <<" "
      << ATT.y[i]
      <<" " 
      << ATT.z[i]
      <<" "
      << ATT.w[i]
      <<" "
      << ATT.vx[i]
      <<" "
      << ATT.vy[i]
      <<" "
      << ATT.vz[i]
      <<"\n";
  }
}





//----------------------------------------------------------
//------------------------------------------------------------




//-------------------- ORBIT CLASS ------------------------------
ORBIT::ORBIT()
{
}
void ORBIT::Set_ORB_Size(ORBIT &ORB, unsigned int size){
  ORB.Tstart.resize(size);
  ORB.x.resize(size);
  ORB.y.resize(size);
  ORB.z.resize(size);
  ORB.vx.resize(size);
  ORB.vy.resize(size);
  ORB.vz.resize(size);
  ORB.CM.resize(size);
  ORB.SAA.resize(size);
  ORB.entr.resize(size);
}





void ORBIT::Print_ORB_Entries(ORBIT &ORB){
  std::cout<<"ORB elements  "<<"\n";
  std::cout<<"Ts x y z vx vy vz CM SAA \n";
for (unsigned int i = 0; i < ORB.entr.size(); ++i){ 
    std::cout
      << i
      <<" "
      << ORB.Tstart[i]
      <<" "
      << ORB.x[i]
      <<" "
      << ORB.y[i]
      <<" " 
      << ORB.z[i]
      <<" "
      << ORB.vx[i]
      <<" "
      << ORB.vy[i]
      <<" "
      << ORB.vz[i]
       <<" "
      << ORB.CM[i]
      <<" "
      << ORB.SAA[i]
      <<"\n";
  }
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
    
    
    
    //FT2.FT2_T.LiveTime[i]=0;

    if(FT2.DT.update[i]){
      
 
      FT2.FT2_T.LiveTime[i]=
	(FT2.DT.Tstop_LiveTime[i] -  FT2.DT.Tstart_LiveTime[i]) - FT2.DT.DeadTime[i]; 

      if(FT2.verbose){
	printf("Entry %d \n",i);
	std::cout<<"upadte "
		 <<i
		 <<std::endl;
	
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
      }

    
      if(FT2.DT.update[i+1] && i<FT2_ENTR-1){
	fraction=(FT2.FT2_T.Tstop[i]-FT2.DT.Tstop[i])/(FT2.DT.Tstart[i+1]-FT2.DT.Tstop[i]);
	fraction*=(FT2.DT.Tstart_LiveTime[i+1]-FT2.DT.Tstop_LiveTime[i]);
	if(FT2.verbose){
	  std::cout<<"fraction fwd "
		   <<fraction
		   <<"\n";
	  //printf("%e %e\n",FT2.DT.Tstart_LiveTime[i+1],FT2.DT.Tstop_LiveTime[i]);
	}
	FT2.FT2_T.LiveTime[i]+= fraction;
	//fraction_tot+=fraction;
      }
    
      if(FT2.DT.update[i-1] && i>0){
	fraction=1.0/(FT2.DT.Tstart[i]-FT2.DT.Tstop[i-1]);
	fraction*=(FT2.DT.Tstart[i]-FT2.FT2_T.Tstart[i]);
	fraction*=(FT2.DT.Tstart_LiveTime[i]-FT2.DT.Tstop_LiveTime[i-1]);
	if(FT2.verbose){
	  std::cout<<"fraction back "
		   <<fraction
		   <<"\n";
	  //printf("%e %e\n",FT2.DT.Tstart_LiveTime[i+1],FT2.DT.Tstop_LiveTime[i]);
	}
	FT2.FT2_T.LiveTime[i]+= fraction;
	//fraction_tot+=fraction;
      }
      
      
    }

    
    if(FT2.verbose){
      std::cout<<"live "
	       <<std::setprecision(20)
	       <<FT2.FT2_T.LiveTime[i]
	       <<std::endl 
	       <<"-------------------------------------------------------------------------"
	       <<std::endl;
    }
    //------ inter bin correction --------
    
  }
  
  
}




//-------------------Get Files Names -------------------------
void FT2::getFileNames(int iargc, char * argv[], FT2 &FT2) {
  const char usage[] = 
    "usage: makeFT2Entries.exe  [OPTIONS]\n" 
    " -DigiFile <FileName>\n"
    " -MeritFile <FileName>\n" 
    " -M7File <FileName>\n"  
    " -FT2_txt_File <FileName>\n" 
    " -FT2_fits_File <FileName> \n"
    " --Gleam \n"
    " --verbose\n"
    " -h --help\n";

  if (iargc < 8) {
    std::cout << usage;
    std::exit(0);
  }   else  {
    int i;
    std::cout<<"command line"<<std::endl;
    for( i = 1; i < iargc; i++ ){
      std::string par = argv[i];
      if (argv[i][0] == '-'){
	std::cout<<argv[i]<<std::endl;
	if(par=="-DigiFile") FT2.DigiFile = std::string(argv[i+1]);
	if(par=="-MeritFile") FT2.MeritFile = std::string(argv[i+1]);
	if(par=="-M7File") FT2.M7File = std::string(argv[i+1]);
	if(par=="-FT2_txt_File") FT2.FT2_txt_File = std::string(argv[i+1]);
	if(par=="-FT2_fits_File") FT2.FT2_fits_File= std::string(argv[i+1]);
	if(par=="-h"){
	  std::cout << usage;
	  std::exit(0);
	  }
	if(par=="--help"){
	  std::cout << usage;
	  std::exit(0);  
	}
	if(par=="--Gleam"){
	  std::cout<<"Gleam FT2\n";
	  FT2.Gleam=true;
	}
	if(par=="--verbose"){
	  std::cout<<"verbose\n";
	  FT2.verbose=true;
	}
	if(par=="--MC"){
	  std::cout<<"MonteCarlo \n";
	  FT2.MC=true;
	}


      }
    }
    std::cout<<"Digi File " 
	     <<FT2.DigiFile<<std::endl
	     <<"MeritFile"
	     <<FT2.MeritFile<<std::endl
	     <<"M7 File "
	     <<FT2.M7File<<std::endl
	     <<"OUT FILE"
	     <<FT2.FT2_txt_File
	     <<" FITS FILE"
	     <<FT2.FT2_fits_File
	     <<" Gleam=="
	       <<FT2.Gleam
	     <<std::endl;
    
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
  printf("Merge M7 with Digi entries -\n");

  //add new entry
  printf("FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2, Tstart_Run,Current_FT2_Entry);
  if(!FT2.Get_OutOfRange(FT2)){
    printf("Entry index=%d \n",Current_FT2_Entry);
    FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
    printf("add entry, FT2 entries %d \n",Get_FT2_Entries(FT2));

    FT2.FT2_T.Tstart[Current_FT2_Entry]=FT2.FT2_T.Tstart[Current_FT2_Entry];
    FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entry];
    FT2.FT2_T.Tstop[Current_FT2_Entry]=Tstart_Run ;
    FT2.FT2_T.Tstart[FT2_Entries-1]=Tstart_Run ;
  }else{
    printf("Adding entries before that M7 file starts");
    FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
    printf("add entry, FT2 entries %d \n",Get_FT2_Entries(FT2));
    printf("Entry index=0\n");
    FT2.FT2_T.Tstart[FT2_Entries-1]=Tstart_Run;
    FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstart[0];
  }

  
  std::sort(FT2.FT2_T.Tstart.begin(), FT2.FT2_T.Tstart.end());
  std::sort(FT2.FT2_T.Tstop.begin(), FT2.FT2_T.Tstop.end());
 

  //add new entry
  printf("FT2 entries %d \n",Get_FT2_Entries(FT2));
  FT2.Get_FT2_Entry_Index(FT2,Tstop_Run,Current_FT2_Entry);
  if(!FT2.Get_OutOfRange(FT2)){
    printf("Entry index=%d \n",Current_FT2_Entry);
    FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
    printf("add entry, FT2 entries %d \n",Get_FT2_Entries(FT2));
    
    FT2.FT2_T.Tstart[Current_FT2_Entry]=FT2.FT2_T.Tstart[Current_FT2_Entry];
    FT2.FT2_T.Tstop[FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entry];
    FT2.FT2_T.Tstop[Current_FT2_Entry]=Tstop_Run ;
    FT2.FT2_T.Tstart[FT2_Entries-1]=Tstop_Run ;
  }else{
    printf("Adding entries after that M7 file ends");
    FT2.Update_FT2_Entries(FT2,Get_FT2_Entries(FT2)+1);
    FT2_Entries= Get_FT2_Entries(FT2);
    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,FT2_Entries);
    printf("add entry, FT2 entries %d \n",Get_FT2_Entries(FT2));
    printf("Entry index=%d \n",FT2_Entries-1);
    FT2.FT2_T.Tstart[FT2_Entries-1]=FT2.FT2_T.Tstop[FT2_Entries-2] ;
    FT2.FT2_T.Tstop[FT2_Entries-1]=Tstop_Run;
  }
  
  

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

  //FT2 entries Number
  Entries=Get_FT2_Entries(FT2);
  

  //Set true the out of range value
  FT2.Set_OutOfRange_TRUE(FT2);

  //Look for the Entry index
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
  
  i=int((time-Tstart)/FT2_BIN_WIDTH);
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
  f_time*=1.0e-6;
  
  return (time+f_time);
}


//--------------------------------Set Entries According to M7 file -----------------------
void FT2::Set_M7_Entries(FT2 &FT2){

  double time,Tstart;
  //int new_entry(1);
  unsigned int M7LineCounter(0);
  int  TimeBin(0), NewTimeBin(0),OldTimeBin(0),MODE(0),OLD_MODE;
  bool first_orb_entry(true);
  unsigned int Current_FT2_Entries(0);
  
  //File Handlign
  std::string buf,comment; //buffer string
  std::string line;
  std::ifstream M7F;

  M7F.open(FT2.M7File.c_str());

  if(M7F.fail())
    {
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
    comment=line.substr(0,1);
    if(comment.find( "#", 0) == std::string::npos ){
      
      time=FT2.Get_M7_Time(tokens[3],tokens[4]);
    

   	if(FT2.verbose){ 
	  std::cout<<"Time=" 
		   <<std::setprecision(20)
		   <<time
		   <<std::endl;

	}    

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
      FT2.Update_FT2_Entries(FT2,Current_FT2_Entries);

      
      //Resize FT2_Time class 
      FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T,Current_FT2_Entries);
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
	if(TimeBin-OldTimeBin<=1){
	  FT2.FT2_T.Tstart[Current_FT2_Entries-1]=FT2.FT2_T.Tstop[Current_FT2_Entries-2];
	}else{
	  FT2.FT2_T.Tstart[Current_FT2_Entries-1]=time;
	}

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
	if(FT2.verbose){
	  std::cout
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
	    <<std::endl;
	}
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
 
  //!!!!!!!!!!! WE DO NOT NEED ANYMORE THIS METHOD
  //BECAUSE THE FT2 TEMPLATE REQUIRES VALUE AT 
  //THE BEGINNING OF THE TIME BIN !!!!!!!!!!!!!!!
  //FT2.Average_M7_Entries(FT2);
  FT2.Interp_ORB_Entries(FT2);
  FT2.Interp_ATT_Entries(FT2);
  if(FT2.verbose){
    printf("---------------------------------------------------------\n");
  }
}

//-------------Interplates ORB if entr=0------------------
void FT2::Interp_ORB_Entries(FT2 &FT2){
  double deltat ;
  
 for (unsigned int i = 0; i < FT2.ORB.entr.size(); ++i){
   if(FT2.verbose){
     std::cout<<"ORB elements in Entry "<<i<<","<<FT2.ORB.entr[i]<<"\n";
   }
   if(FT2.ORB.entr[i]==0 && i>0){
      deltat=FT2_T.Tstop[i-1]-FT2_T.Tstart[i-1];
      std::cout<<"deltat = "<<deltat<<"\n";
      FT2.ORB.Tstart[i]=FT2_T.Tstart[i];
      FT2.ORB.x[i]=ORB.x[i-1]+ORB.vx[i-1]*deltat;
      FT2.ORB.y[i]=ORB.y[i-1]+ORB.vy[i-1]*deltat;
      FT2.ORB.z[i]=ORB.z[i-1]+ORB.vz[i-1]*deltat;
      FT2.ORB.CM[i]= ORB.CM[i-1];
      FT2.ORB.SAA[i]=ORB.SAA[i-1];

   }
   
   if(FT2.ORB.entr[i]==0 && i==0){
      deltat=FT2_T.Tstop[i]-FT2_T.Tstart[i];
      std::cout<<"deltat = "<<deltat<<"\n";
      FT2.ORB.Tstart[i]=FT2_T.Tstart[i];
      FT2.ORB.x[i]=ORB.x[i+1]-ORB.vx[i+1]*deltat;
      FT2.ORB.y[i]=ORB.y[i+1]-ORB.vy[i+1]*deltat;
      FT2.ORB.z[i]=ORB.z[i+1]-ORB.vz[i+1]*deltat;
      FT2.ORB.CM[i]= ORB.CM[i+1];
      FT2.ORB.SAA[i]=ORB.SAA[i+1];
   }

 }
 
}



//-------------Interplates ATT if entr=0------------------
void FT2::Interp_ATT_Entries(FT2 &FT2){
  double deltat ;
  
 for (unsigned int i = 0; i < FT2.ATT.entr.size(); ++i){
   if(FT2.verbose){
     std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
   }
   if(FT2.ATT.entr[i]==0 && i>0){
     deltat=FT2_T.Tstop[i-1]-FT2_T.Tstart[i-1];
     std::cout<<"deltat = "<<deltat<<"\n";
     FT2.ATT.Tstart[i]=FT2_T.Tstart[i];
     FT2.ATT.x[i]=FT2.ATT.x[i-1];
     FT2.ATT.y[i]=FT2.ATT.y[i-1];
     FT2.ATT.z[i]=FT2.ATT.z[i-1];
     FT2.ATT.w[i]=FT2.ATT.w[i-1];
     FT2.ATT.vx[i]=FT2.ATT.vx[i-1];
     FT2.ATT.vy[i]=FT2.ATT.vy[i-1];
     FT2.ATT.vz[i]=FT2.ATT.vz[i-1];
   } 
   if(FT2.ATT.entr[i]==0 && i==0){
     deltat=FT2_T.Tstop[i+1]-FT2_T.Tstart[i+1];
     std::cout<<"deltat = "<<deltat<<"\n";
     FT2.ATT.Tstart[i]=FT2_T.Tstart[i];
     FT2.ATT.x[i]=FT2.ATT.x[i+1];
     FT2.ATT.y[i]=FT2.ATT.y[i+1];
     FT2.ATT.z[i]=FT2.ATT.z[i+1];
     FT2.ATT.w[i]=FT2.ATT.w[i+1];
     FT2.ATT.vx[i]=FT2.ATT.vx[i+1];
     FT2.ATT.vy[i]=FT2.ATT.vy[i+1];
      FT2.ATT.vz[i]=FT2.ATT.vz[i+1];
   }
   
 }
 
}


//-------------- Average M7 File  Entries  within a FT2 time bin -----------------------
//!!!!!!!!!!! WE DO NOT NEED ANYMORE THIS METHOD
//BECAUSE THE FT2 TEMPLATE REQUIRES VALUE AT 
//THE BEGINNING OF THE TIME BIN !!!!!!!!!!!!!!!
//void FT2::Average_M7_Entries(FT2 &FT2){
//  
//  for (unsigned int i = 0; i < FT2.ATT.entr.size(); ++i){ 
//    std::cout<<"ATT elements in Entry "<<i<<","<<FT2.ATT.entr[i]<<"\n";
//    FT2.ATT.x[i]*=1.0/(double( FT2.ATT.entr[i]));
//    FT2.ATT.y[i]*=1.0/(double( FT2.ATT.entr[i]));
//    FT2.ATT.z[i]*=1.0/(double( FT2.ATT.entr[i]));
//    FT2.ATT.w[i]*=1.0/(double( FT2.ATT.entr[i]));
//    FT2.ATT.vx[i]*=1.0/(double( FT2.ATT.entr[i]));
//    FT2.ATT.vy[i]*=1.0/(double( FT2.ATT.entr[i]));
//    FT2.ATT.vz[i]*=1.0/(double( FT2.ATT.entr[i]));
//  }
//
//  for (unsigned int i = 0; i < FT2.ORB.entr.size(); ++i){
//   std::cout<<"ORB elements in Entry "<<i<<","<<FT2.ORB.entr[i]<<"\n";
//
//    if( FT2.ORB.entr.size()==0){
//      //!!!!!!!!!!!Make interpolation
//     //!!!!!!!!!!!FT2.interpolate(FT2.ORB);
//  }
//  
// FT2.ORB.x[i]*=1.0/(double(FT2.ORB.entr[i]));
// FT2.ORB.y[i]*=1.0/(double(FT2.ORB.entr[i]));
// FT2.ORB.z[i]*=1.0/(double(FT2.ORB.entr[i]));
// FT2.ORB.vx[i]*=1.0/(double(FT2.ORB.entr[i]));
// FT2.ORB.vy[i]*=1.0/(double(FT2.ORB.entr[i]));
// FT2.ORB.vz[i]*=1.0/(double(FT2.ORB.entr[i]));
//  }
//}


//-------------- Update and clean M7 fields in the FT2 Entries -----------------------
void FT2::Clean_ATT_Quaternions(ATTITUDE &Att, unsigned int entry){      
  Att.entr[entry]=0;
  Att.Tstart[entry]=0;
  Att.y[entry]=0;
  Att.z[entry]=0;
  Att.w[entry]=0;
  Att.vx[entry]=0;
  Att.vy[entry]=0;
  Att.vz[entry]=0;
    
}


void FT2::Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens, unsigned int entry){     
  Att.entr[entry]++;
  //Update only if the entry is the first for the time bin
  if( Att.entr[entry]==1){
    Att.Tstart[entry]=FT2::Get_M7_Time(tokens[3],tokens[4]);
    Att.x[entry]=atof(tokens[5].c_str());
    Att.y[entry]=atof(tokens[6].c_str());
    Att.z[entry]=atof(tokens[7].c_str());
    Att.w[entry]=atof(tokens[8].c_str());
    Att.vx[entry]=atof(tokens[9].c_str());
    Att.vy[entry]=atof(tokens[10].c_str());
    Att.vz[entry]=atof(tokens[11].c_str());
  }
}

void FT2::Clean_ORB(ORBIT &Orb, unsigned int entry){  
  Orb.entr[entry]=0;
  Orb.Tstart[entry]=0;
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
  //Update only if the entry is the first for the time bin
  if (Orb.entr[entry]==1){
    //printf("--\n");
    Orb.Tstart[entry]=FT2::Get_M7_Time(tokens[3],tokens[4]);
    Orb.x[entry]=atof(tokens[5].c_str());
    Orb.y[entry]=atof(tokens[6].c_str());
    Orb.z[entry]=atof(tokens[7].c_str());
    Orb.vx[entry]=atof(tokens[8].c_str());
    Orb.vy[entry]=atof(tokens[9].c_str());
    Orb.vz[entry]=atof(tokens[10].c_str());
    Orb.CM[entry]=atoi(tokens[11].c_str());
    Orb.SAA[entry]=atoi(tokens[12].c_str());
  }

}











