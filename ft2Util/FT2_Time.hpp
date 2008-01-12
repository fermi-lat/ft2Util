/** @file FT2_Time.cxx
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

*/


#ifndef __FT2_Time_Class__
#define __FT2_Time_Class__

const double FT2_BIN_WIDTH=1.0;

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


class FT2_SpaceCraft {
 public:
  std::vector<double> RA_SCX,RA_SCZ,DEC_SCX,DEC_SCZ;
  std::vector<double> RA_ZENITH,DEC_ZENITH;
  std::vector<double> LAT_GEO,LON_GEO,RAD_GEO;
  std::vector<double> B_MCILWAIN,L_MCILWAIN,GEOMAG_LAT;
  FT2_SpaceCraft();

  void Set_FT2SC_Size(FT2_SpaceCraft &FT2_SC,unsigned int size);
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
  void Print_ATT_Entries(ATTITUDE &ATT);
  std::vector<double> Tstart;
  std::vector<double> x,y,z,w,vx,vy,vz;
  std::vector<int>  entr;
};


//This Class stores information about
//Time from Orbit in M-7 file
class ORBIT{
public:
  ORBIT();
  void Set_ORB_Size(ORBIT &ORB,unsigned int size);
  void Print_ORB_Entries(ORBIT &ORB);
  std::vector<double> Tstart;  
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
  std::string FT2_fits_File;
  
  DigiTime DT;
  ATTITUDE ATT;
  ORBIT ORB;
  FT2_Time FT2_T;
  FT2_SpaceCraft FT2_SC;

  //File Names 
  bool verbose;
  void getFileNames(int iargc, char * argv[], FT2 &FT2);
  unsigned int LineNumberCount(const std::string & infile); 
  void Get_DigiFileLineNumber(FT2 &FT2, const std::string & infile );
 
 
  //M-7
  void Set_M7_Entries(FT2 &FT2);
  double Get_M7_Time(const std::string &Time, const std::string &Frac_Time);
  void Fill_M7_Entries(FT2 &FT2);
  void Average_M7_Entries(FT2 &FT2);
  void Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens, unsigned int entry);
  void Update_ORB(ORBIT &Orb, const std::vector<std::string> &tokens, unsigned int entry);
  void Clean_ATT_Quaternions(ATTITUDE &Att, unsigned int entry);
  void Clean_ORB(ORBIT &Orb, unsigned int entry);
  void Interp_ORB_Entries(FT2 &FT2);
  void Interp_ATT_Entries(FT2 &FT2);
  unsigned int M7_Entries;
 
  //FT2_Time
  //void Update_FT2_Time(FT2 &FT2, unsigned int Current_FT2_Entries, unsigned int bin);
 
  //DIGI
  unsigned int DigiEntries;
  // void Update_Digi(FT2 &FT2);

  

  //FT2
  void Get_FT2_Entry_Index(FT2 &FT2, double time, unsigned int &i);
  void Update_FT2_Entries(FT2 &FT2, int i);
  unsigned int Get_FT2_Entries(FT2 &FT2);
  void Merge_M7_Digi_Entries(FT2 &FT2,double Tstart_Run ,double Tstop_Run);
  unsigned int DigiFileLineNumber;
  void Set_OutOfRange_TRUE(FT2 &FT2);
  void Set_OutOfRange_FALSE(FT2 &FT2);
  bool Get_OutOfRange(FT2 &FT2);

  //unsigned long FT2_Entries;


  //FT2
  int Get_FT2_Time_Bin(double time,double Tstart);  
  void Evaluate_Live_Time(FT2 &FT2);
 


  //FT2_SpaceCraft
  void Fill_SC_Entries(FT2 &FT2);


  //Digi
  void Digi_FT2(FT2 &FT2);
  
  //Gleam
  void Gleam_FT2(FT2 &FT2);
  bool Gleam;
   
  //MonteCarlo
  void DigiMC_FT2(FT2 &FT2);
  bool MC;
  
  //Fits 
  void WriteFitsFile(FT2 &FT2);
  
  //Text
  void WriteTextFile(FT2 &FT2);

private:
  bool OutOfRange;
  unsigned int CurrentEntry;
  unsigned int Entries; 
  
};



#endif
