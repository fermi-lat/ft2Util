#ifndef __FT2_Time_Class__
#define __FT2_Time_Class__



//This class handles some time fields
//for the FT2 file, all the time here refers
//to the time for the final FT2 fil
class FT2_Time {
 public:
  std::vector<double> Tstart, Tstop,LiveTime;
  std::vector<unsigned int> bin;  
  FT2_Time();
  void Set_FT2Time_Size(FT2_Time &FT2_T,unsigned int size);
  double FT2_Time::Get_Tstart( unsigned int  timebin );
  double FT2_Time::Get_Tstop( unsigned int  timebin );
};


//This Class stores information about
//Time from the digi file, all times here
//are to be compared with the time in the
//class FT2_Time
class DigiTime {
 public:
  std::vector<double> Tstart, Tstop, LiveTime,DeadTime,Tstart_LiveTime,Tstop_LiveTime; 
  std::vector<int> update;  
  DigiTime();
  void Set_DigiTime_Size(DigiTime &DigiT, unsigned long size);
};


//This Class stores information about
//Time from Attitude in M-7 file
class ATTITUDE{
 public:
  ATTITUDE();
  void Set_ATT_Size(ATTITUDE &Att, unsigned int size);
  std::vector<double> x,y,z,w,vx,vy,vz;
  std::vector<int>  entr;
};


//This Class stores information about
//Time from Orbit in M-7 file
class ORBIT{
public:
  ORBIT();
  void Set_ORB_Size(ORBIT &Orb,unsigned int size);
 

  std::vector<double> x,y,z,vx,vy,vz;
  std::vector<int>  entr,CM,SAA; 
};



//This Class handles and stores the information
//coming from the DIGI and M-7 file, i.e.
//the FT2 informations within a RUN
//These informations wiil populate a database
//that will be queried to build the FT2 fits file.
class FT2{
public:
  FT2();
  std::string MeritFile;
  std::string DigiFile;
  std::string M7File;
  std::string FT2_txt_File;
  
  DigiTime DT;
  ATTITUDE ATT;
  ORBIT ORB;
  FT2_Time FT2_T;

  //File Names 
  void getFileNames(int iargc, char * argv[]);
  unsigned int LineNumberCount(const std::string & infile); 
  void Get_DigiFileLineNumber(const std::string & infile );
 
 
  //M-7
  double Get_M7_Time(const std::string &Time, const std::string &Frac_Time);
  void Handle_M7_Entries(FT2 &FT2);
  void Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens, unsigned int entry);
  void Update_ORB(ORBIT &Orb, const std::vector<std::string> &tokens, unsigned int entry);
  unsigned int M7_Entries;
 
  //FT2_Time
  void Update_FT2_Time(FT2 &FT2, unsigned int Current_FT2_Entries, unsigned int bin);
 
  //DIGI
  unsigned int DigiEntries;
  // void Update_Digi(FT2 &FT2);

  

  //FT2
  int Get_FT2_Entry_Index(FT2 &FT2 ,double time);
  void Update_FT2_Entries(FT2 &FT2, int i);
  unsigned int Get_FT2_Entries(FT2 &FT2);
  unsigned int DigiFileLineNumber;
  //unsigned long FT2_Entries;


  //FT2
  int Get_FT2_Time_Bin(double time);  
  void Evaluate_Live_Time(FT2 &FT2);
 

  
private:
  unsigned int CurrentEntry;
  unsigned int Entries; 
  
};



#endif
