/** @file FT2_Time.hpp
 * @brief
 *
 * @author Andrea Tramacere <tramacer@slac.stanford.edu>
 *
 */


#ifndef __FT2_Time_Class__
#define __FT2_Time_Class__


#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"


/**
 * @class FT2_Time
 *
 * @brief This class handles time quantities for the ft2 file
 * like the time start and stop for each bin
 * the livetime and the bin ID
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 *
 */

class FT2_Time {
public:
    ///time start of the ft2 file time bin
    std::vector<double> Tstart,
            ///time stop of the ft2 file time bin
            Tstop,
            ///live time for the ft2 file time bin
            LiveTime;
    ///Id for the time bin in ft2 file
    std::vector<unsigned int> bin;
    ///default constructor
    FT2_Time();
    
    /**
     *@brief Set_FT2Time_Size set the size of all the array members of this class
     *@param FT2_T address of the FT2_Time object
     *@param size to set the FT2_Time arrays dimension
     */
    void Set_FT2Time_Size(FT2_Time &FT2_T, unsigned int size);
    
};

/**
 * @class FT2_SpaceCraft
 * @brief This class hold the members useful to store the non time
 * entries in the ft2 file ie position attitude and geomagnetic quantities
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 *
 */
class FT2_SpaceCraft {
public:
    /// SC RA of X axis
    std::vector<double> RA_SCX,
            /// SC RA of Z axis
            RA_SCZ,
            /// SC DEC of X axis
            DEC_SCX,
            /// SC DEC of Z axis
            DEC_SCZ;
    
    /// RA of the Zenith
    std::vector<double> RA_ZENITH,
            /// DEC of the Zenith
            DEC_ZENITH;
    
    /// IEF coordinate for Latitude
    std::vector<double> LAT_GEO,
            /// IEF coordinate for Longitude
            LON_GEO,
            /// SC altitude
            RAD_GEO;
    ///Geomagnetic: B MCILWAIN
    std::vector<double> B_MCILWAIN,
            ///Geomagnetic: L MCILWAIN
            L_MCILWAIN,
            ///Geomagnetic: Latitude
            GEOMAG_LAT;
    
    ///Rocking anlge and Orbital Pole
    std::vector<double> ROCKING_ANGLE, ORBITAL_POLE_RA, ORBITAL_POLE_DEC;
    
    
    void EvalRockingAngle(FT2_SpaceCraft &FT2_SC , unsigned int i);
    void EvalOrbitalPole(Hep3Vector pos1, Hep3Vector pos2, FT2_SpaceCraft &FT2_SC , unsigned int i);
    
    
    ///Quaternion: X component
    std::vector<double> QS_J1,
            ///Quaternion: Y component
            QS_J2,
            ///Quaternion: Z component
            QS_J3,
            ///Quaternion: Sacalar component
            QS_J4;
    
    ///default constructor
    FT2_SpaceCraft();
    
    /**
     *@brief Set_FT2SC_Size set the size of all the array members of this class
     *@param FT2_SC address of the FT2_SpaceCraft object
     *@param size to set the  FT2_SpaceCraft  arrays dimension
     */
    void Set_FT2SC_Size(FT2_SpaceCraft &FT2_SC, unsigned int size);
};



/**
 * @class  DigiTime
 *
 * @brief This class hold the members useful to store all the quantities from
 * the Digi file
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 *
 */
class DigiTime {
public:
    ///Time start of the Digi event within a ginve ft2 time bin
    std::vector<double> Tstart,
            ///Time stop of the Digi event within a ginve ft2 time bin
            Tstop,
            ///Live elapsed time in a give ft2 time bin
            LiveTime,
            ///Livetime stamp of the first digi evt in a given ft2 time bin
            Tstart_LiveTime,
            ///Livetime stamp of the last digi evt in a given ft2 time bin
            Tstop_LiveTime;
    
    ///Dead time in a give time bin
    std::vector<double> DeadTime,
            ///Dead time from gaps in the Digi file
            GapsDeadTime,
            ///Dead time from recon crash
            ReconDeadTime,
            ///Helper variable to evaluate the actual dead time
            RealGapsDeadTime;
    
    ///variable to decide the updating of entries for dead time correction
    std::vector<bool> update;
    
    ///varible flagging entries with gaps
    std::vector<bool> gap;
    
    ///Time start of the Run
    double RunStart,
            ///Time stop of the Run
            RunStop;
    ///default constructor
    DigiTime();
    
    /**
     *@brief Set_DigiTime_Size set the size of all the array members of this class
     *@param DigiT address of the DigiTime object
     *@param size to set the  DigiTime  arrays dimension
     */
    void Set_DigiTime_Size(DigiTime &DigiT, unsigned long size);
};



/**
 * @class  GAPS
 *
 * @brief This class handles gaps in the Digi file
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 */
class GAPS{
public:
    ///Default constructor
    GAPS();
    
    ///LiveTime of the first evt before the gap
    std::vector<double> LiveTstart,
            ///LiveTime of the firs evt after the gap
            LiveTstop,
            ///Time of the the first evt before the gap
            Tstart,
            ///Time of the the first evt after the gap
            Tstop;
    
    ///GemID of the first evt before the gap
    std::vector<unsigned long>  GemStart,
            ///GemID of the first evt after the gap
            GemStop;
    /**
     *@brief Set_GAPS_Size set the size of all the array members of this class
     *@param Gap  address of the GAPS object
     *@param size to set the  GAPS class arrays dimension
     */
    void Set_GAPS_Size(GAPS &Gap, unsigned long size );
};



/**
 * @class  ATTITUDE
 *
 * @brief This class handles ATTITUDE information from M7 file
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 */
class ATTITUDE{
public:
    ///Default constructor
    ATTITUDE();
    
    /**
     *@brief Set_ATT_Size  set the size of all the array members of this class
     *@param Att address of the ATTITUDE object
     *@param size to set the ATTITUDE class arrays dimension
     */
    void Set_ATT_Size(ATTITUDE &Att, unsigned int size);
    
    /**
     *@brief Print_ATT_Entries print all the information of all the ATT entries
     *@param Att address of the ATTITUDE object
     */
    void Print_ATT_Entries(ATTITUDE &ATT);
    
    ///Tstamp of the first M7 ATT entry falling in a given ft2 bin
    std::vector<double> Tstart;
    
    ///M7 x quaternion component
    std::vector<double> x,
            ///M7 Y quaternion component
            y,
            ///M7 z quaternion component
            z,
            ///M7 scalar quaternion component
            w,
            ///M7 x angular velocity
            vx,
            ///M7 y angular velocity
            vy,
            ///M7 z angular velocity
            vz;
    
    ///variable used to count M7 entries falling within a ft2 time bin
    std::vector<unsigned int>  entr;
    
    ///varible to flag M7 ATT entries gaps
    std::vector<bool> gap;
    /**
     * @brief  Interpolation FLAGS description :
     * -1 means that the interpolation failed
     *  1 means interpolation made by the best method (SLERP)
     *  2 means extrapolation  via velocities (both for ATT and ORB)
     *  3 means extrapolation via velocities (very inaccurae)
     *  4 means vec_norm significantly larger than 1 and W *FORCED* to 0
     *  5 means vec_norm significantly larger than 1 and W *NOT FORCED* to 0
     */
    std::vector<int> interp_flag;
    
    /**
     *@brief  Eval_w decide the strategy to eval the quaternion scalar component
     *@param Att ATTITUDE object address
     *@param i entry of the ft2 time bin
     */
    void Eval_w(ATTITUDE &Att, unsigned int i);
    
    /**
     *@brief Eval_VecNorm evaluate the sum x^2+y^2+z^2 components of the quternion
     *@param  Att ATTITUDE object address
     *@return i entry of the ft2 time bin
     */
    double Eval_VecNorm(ATTITUDE &Att, unsigned int i);
    
    /**
     *@brief CorrectAndEval_w check and correct and eval the scalar component
     *@param Att ATTITUDE object address
     *@return i entry of the ft2 time bin
     */
    void CorrectAndEval_w(ATTITUDE &Att, unsigned int i);
    
    /**
     *@brief CheckAndEval_w check but does not correc the scalar component
     *@param Att ATTITUDE object address
     *@return i entry of the ft2 time bin
     */
    void CheckAndEval_w(ATTITUDE &Att, unsigned int i);
    
    ///Value of the tolerance to correct for the scalar quaternion component
    double NomrTolerance;
    
    ///Value of the tolerance to move entry to acutual ft2 tstart
    double DeltaT_TstatTolerance;
    
    ///bool var to test and correct the scalar quaternion component
    bool TestQ, TestQ_all;
};



/**
 * @class  ORBIT
 *
 * @brief This class handles ORBIT information from M7 file
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 */

class ORBIT{
public:
    
    ///Default constructor
    ORBIT();
    
    /**
     *@brief Set_ORB_Size  set the size of all the array members of this class
     *@param Att address of the ORBTI object
     *@param size to set the ORBIT class arrays dimension
     */
    void Set_ORB_Size(ORBIT &ORB, unsigned int size);
    
    /**
     *@brief Print_ORB_Entries print all the information of all the ORB entries
     *@param ORB address of the ORBIT object
     */
    void Print_ORB_Entries(ORBIT &ORB);
    
    ///Tstamp of the first M7 ORB entry falling in a given ft2 bin
    std::vector<double> Tstart;
    
    ///ORBIT (position) x component
    std::vector<double> x,
            ///ORBIT (postion) y component
            y,
            ///ORBIT (postion) z component
            z,
            ///ORBIT vx component
            vx,
            ///ORBIT vy component
            vy,
            ///ORBIT vz component
            vz;
    
    ///varible to flag M7 ORBIT entries gaps
    std::vector<bool> gap;
    
    /**
     * @brief  Interpolation FLAGS description :
     * Interpolation FLAGS description:
     * -1 means that the interpolation failed
     *  1 means interpolation made by the best method (gtbary)
     *  2 means extrapolation via parabolic fit (accurate for shor gaps ~ 10 sec)
     *  3 means extrapolation via velocities  (very inaccurate)
     */
    std::vector<int> interp_flag;
    
    ///variable used to count M7 entries falling within a ft2 time bin
    std::vector<unsigned int>  entr,
            ///M7 SAA flag
            SAA;
    ///M7 observation mode flag
    std::vector<int> CM;
    
    ///Value of the tolerance to move entry to acutual ft2 tstart
    double DeltaT_TstatTolerance;
};



/**
 * @class  FT2
 *
 * @brief This is the main class used to build the ft2 util
 * @this class aggregates the other classes
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 */
class FT2{
    
public:
    
    /// constructor of the FT2 class
    FT2();
    
    /// Merit file string
    std::string MeritFile;
    
    /// Digi file string
    std::string DigiFile;
    
    /// M7 file string
    std::string M7File;
    
    /// FT2 txt file string
    std::string FT2_txt_File;
    
    /// FT2 fits file string
    std::string FT2_fits_File;
    
    /// Digi Gaps file string
    std::string Gaps_File;
    
    /// Version string
    std::string Version ;

    /// Path to ft2 tpl file
    std::string path;

    /// Digi Time Class Aggregate object
    DigiTime DT;
    
    /// GAPS Class Aggregate object
    GAPS GP;
    
    /// ATTITUDE Class Aggregate object
    ATTITUDE ATT;
    
    /// ORBIT Class Aggregate object
    ORBIT ORB;
    
    /// FT2_Time Class Aggregate object
    FT2_Time FT2_T;
    
    /// FT2_SpaceCraft Aggregate object
    FT2_SpaceCraft FT2_SC;
    
    /// Width of the ft2 bin for non merged file
    double FT2_BIN_WIDTH;
    
    /// Width of the ft2 bin for merged file
    double FT2_MERGED_BIN_WIDTH;
    
    ///Line skipped in M7 file to have ATT and ORB with the same Time stamp
    unsigned int M7ShiftStart;
    
    ///ID of the Run
    unsigned long RunID;
    
    ///LatConfig  flag  for the run intent
    int LAT_CONFIG;
    
    ///DataQual flag for the data quality
    int DATA_QUAL;

    /// varible to set verbose output
    bool verbose;

    /// varible to set ft2 template
    bool new_tpl;

    /**
     *@brief  getFileNames parse the command line and get file names and parameters
     *@param  argc
     *@param  argv
     *@param  FT2 FT2 object address
     */
    void getFileNames(int iargc, char * argv[], FT2 &FT2);
    
    /**
     *@brief LineNumberCoun counts the lines number of a file
     *@param infile file name string
     */
    unsigned int LineNumberCount(const std::string & infile);
    
    
    
    /**
     *@brief  Get_DigiFileLineNumber (? No more used)
     *@param
     */
    void Get_DigiFileLineNumber(FT2 &FT2, const std::string & infile );
    
    
    //------------------------- M-7-------------------------------------------
    /**
     *@brief  Set_M7_Entries set the ft2 file entries according to M7 file
     *@param FT2 address of the FT2 object
     *@param Tstart_RUN time start of the Run
     *@param Tstop_RUN time stop of the Run
     */
    void Set_M7_Entries(FT2 &FT2, double Tstart_RUN, double Tstop_RUN);
    
    /**
     *@brief Get_M7_Time convert time from M7 format to seconds
     *@param  Time string storing the integer time from M7
     *@param  Time string storing the fractional time form M7 (microseconds )
     *@return time in seconds
     */
    double Get_M7_Time(const std::string &Time, const std::string &Frac_Time);
    
    /**
     *@brief  Fill_M7_Entries fill ATT and ORB entries from the M7 file
     *@param FT2 address of the FT2 object
     *@param Tstart_RUN time start of the Run
     *@param Tstop_RUN time stop of the Run
     */
    void Fill_M7_Entries(FT2 &FT2, double Tstart_RUN, double Tstop_RUN);
    
    /**
     *@brief   Average_M7_Entries (No More Used!!!)
     *@param
     */
    void Average_M7_Entries(FT2 &FT2);
    
    /**
     *@brief Update_ATT_Quaternions update the ATT entries from values in M7 file
     *@param Att address of the ATT object
     *@param tokens strings of the M7 file with ATT values
     *@param time of the ATT M7 lie
     *@param Id of the correspondig ft2 bin
     */
    void Update_ATT_Quaternions(ATTITUDE &Att, const std::vector<std::string> &tokens , double time, unsigned int entry);
    
    /**
     *@brief Update_ORB update the ORBIT entries from values in M7 file
     *@param ORB address of the ORBIT object
     *@param tokens strings of the M7 file with ORBIT values
     *@param time of the ATT M7 lie
     *@param Id of the correspondig ft2 bin
     */
    void Update_ORB(ORBIT &Orb, const std::vector<std::string> &tokens, double time, unsigned int entry);
    
    /**
     *@brief Clean_ATT_Quaternions set all zero
     *@param Att address of the ATT object
     *@param entry Id of the correspondig ft2 bin
     */
    void Clean_ATT_Quaternions(ATTITUDE &Att, unsigned int entry);
    
    /**
     *@brief Clean_ORB  set all zero in that entry
     *@param ORB address of the ORBIT object
     *@param entry Id of the correspondig ft2 bin
     */
    void Clean_ORB(ORBIT &Orb, unsigned int entry);
    
    /**
     *@brief Interp_ORB_Entries fills  the ORB entries for gaps in the M7 file
     *the interpolation method is the same used in gtbary, the extrpolation is
     *obtained by means of parabolic approximation or in the worst case by velocities
     *@param FT2 address of the FT2 object
     */
    void Interp_ORB_Entries(FT2 &FT2);
    
    /**
     *@brief Interp_ORB_Entries_PARAB_FW etrapolate forward OBR entries by parabolic
     *approximation
     *@param FT2 address of the FT2 object
     *@param i ft2 entry
     *@param bool handles interpolation failure
     */
    void Interp_ORB_Entries_PARAB_FW(FT2 &FT2, unsigned int i, bool &failed);
    
    /**
     *@brief Interp_ORB_Entries_PARAB_BW etrapolate backward OBR entries by parabolic
     *approximation
     *@param FT2 address of the FT2 object
     *@param i ft2 entry
     *@param bool handles interpolation failure
     */
    void Interp_ORB_Entries_PARAB_BW(FT2 &FT2, unsigned int i, bool &failed);
    
    /**
     *@brief Interp_ATT_Entries fills  the ATTITUDE entries for gaps in the M7 file
     *the interpolation method is SLERP, the extrpolation is made by velocities
     *@param FT2 address of the FT2 object
     */
    void Interp_ATT_Entries(FT2 &FT2);
    
    /**
     *@brief Interp_ORB_Tstart move ORB entries from M7 tstamp to actual ft2 bin TSTART
     *@param FT2 address of the FT2 object
     */
    void Interp_ORB_Tstart(FT2 &FT2);
    
    /**
     *@brief Interp_ATT_Tstart move ATT entries from M7 tstamp to actual ft2 bin TSTART
     *@param FT2 address of the FT2 object
     */
    void Interp_ATT_Tstart(FT2 &FT2);
    
    /**
     *@brief Interp_ORB_Vel_Entries extrapolate ORB  velocities (!!! NO MORE USED)
     *@param FT2 address of the FT2 object
     */
    void Interp_ORB_Vel_Entries(FT2 &FT2);
    
    ///!!!UNSUDE
    unsigned int M7_Entries;
    
    //--------------------------- GAPS -------------------------------------------
    
    ///varible to flag Digi entries GAPS
    bool DigiGAPS;
    
    /**
     *@brief Set_GAPS fills the arrays storing the DigiGaps informations
     *@param FT2 address of the FT2 object
     */
    void Set_GAPS(FT2 &FT2);
    
    /**
     *@brief Set_GAPS_DeadTime Evaluate the Dead Time from the Digi Gaps
     *@param FT2 address of the FT2 object
     */
    void Set_GAPS_DeadTime(FT2 &FT2);
    
    /**
     *@brief Fix_Fake_GAPS handles livetime correction for bin with zero evt but
     *that are not in the middle of a gap
     *@param FT2 address of the FT2 object
     */
    void Fix_Fake_GAPS(FT2 &FT2);
    
    /**
     *@brief Get_Run_ID get the RunID from the GAPS file
     *@param string GapsRun file name
     */
    unsigned long Get_Run_ID(std::string  GapsRun);
    
    
    
    //-------------------------------- DIGI --------------------------------------
    
    /// (?!) NO MORE USED
    unsigned int DigiEntries;
    
    /// Tstart of the Run for the Gleam Option
    double GleamDigiTstart,
            /// Tstop of the Run for the Gleam Option
            GleamDigiTstop;
    ///ExtGemID counter of firs and last Dig_evt
    unsigned long Digi_EvtId_start, Digi_EvtId_stop;
    
    //------------------------------ FT2 -----------------------------------------
    /**
     *@brief Get_FT2_Entry_Index gives the ft2 entry index for a given time
     *@param FT2 address of the FT2 object
     *@param time time
     *@return i entry index
     */
    void Get_FT2_Entry_Index(FT2 &FT2, double time, unsigned int &i);
    
    /**
     *@brief Update_FT2_Entries update  the value of the variable storing
     *the size of the ft2 entries
     *@param FT2 address of the FT2 object
     *@param i new size value
     */
    void Update_FT2_Entries(FT2 &FT2, int i);
    
    /**
     *@brief Get_FT2_Entries return the  value of the variable storing
     *the size of the ft2 entries
     *@param FT2 address of the FT2 object
     */
    unsigned int Get_FT2_Entries(FT2 &FT2);
    
    /**
     *@brief Merge_M7_Digi_Entries merge the DigiEntries with the M7 entries generating
     *the ft2 time binning, and correct for mismatch between M7 and Digi
     *@param FT2 address of the FT2 object
     *@param Tstart_Run time start of the Run (here we had a padding)
     *@param Tstop_Run  time start of the Run (here we had a padding)
     *@param
     */
    void Merge_M7_Digi_Entries(FT2 &FT2, double Tstart_Run , double Tstop_Run, bool & redo);
    
    /**
     *@brief RunCut (!?) Nomore used
     *@param
     */
    void FT2::RunCut(FT2 &FT2, double Tstart, double Tstop);
    
    /// DigiFileLineNumber
    unsigned int DigiFileLineNumber;
    
    /**
     *@brief Set_OutOfRange_TRUE set TRUE the value for events falling out of the
     *ft2 time span
     *@param FT2 address of the FT2 object
     */
    void Set_OutOfRange_TRUE(FT2 &FT2);
    
    /**
     *@briefbrief Set_OutOfRange_FALSE set FALSE the value for events falling out of the
     *ft2 time span
     *@param FT2 address of the FT2 object
     */
    void Set_OutOfRange_FALSE(FT2 &FT2);
    
    /**
     *@brief Get_OutOfRange return the boolean value of the OutRange variable
     *@param FT2 address of the FT2 object
     */
    bool Get_OutOfRange(FT2 &FT2);
    
    /**
     *@brief Get_FT2_Time_Bin, gives the time bin of the ft2 time span, it is not
     *used to build the binning ID of the ft2 file, but it is used just to realize
     *bin changing when reading the M7 file. !!!(Rivedere)
     *@param FT2 address of the FT2 object
     */
    int Get_FT2_Time_Bin(double time, double Tstart);
    
    /**
     *@brief Evaluate_Live_Time is the fucntion that evaluate the livetime putting
     *togheter effects from gaps and crachses and also spit the live time according
     *to the edge bin
     *@param  FT2 address of the FT2 object
     */
    void Evaluate_Live_Time(FT2 &FT2);
    
    //--------------------- FT2_SpaceCraft ---------------------------------------
    /**
     *@brief Fill_SC_Entries fills the SC entries from the ATT ORB ones
     *@param FT2 address of the FT2 object
     */
    void Fill_SC_Entries(FT2 &FT2);
    
    //-------------------------  Digi Option--------------------------------------
    /**
     *@brief Digi_FT2 is the method that handles the Digi Option. It has performs
     *a lot of tascks: Read M7 file and Fill ATT ORB form M7 entries,generates ft2
     *time span by Merging M7 and Digi entries, and looping over Digi and Merit file
     *fills DigiGaps informations, livetime informations, and recon crash inforamtions
     *than it calls the methods to evaluate the live time for all the time bins
     *and eventaully export ft2 fits and txt file.
     *@param FT2 address of the FT2 object
     */
    void Digi_FT2(FT2 &FT2);
    
    //--------------------------- Gleam Option -----------------------------------
    /**
     *@brief is the anlogous of Digi_FT2, but it does not handles Digi and Merit file
     *just generate ft2 file with zero live time and reads SC information from M7 file
     *@param FT2 address of the FT2 object
     */
    void Gleam_FT2(FT2 &FT2);
    
    /// variable to handle the Gleam option
    bool Gleam;
    
    //------------------------ MonteCarlo Option ---------------------------------
    /**
     *@brief s the anlogous of Digi_FT2, but for the MonteCarlo case, so livetime
     *is evaluated in a different way due to roll-overe effects
     *@param FT2 address of the FT2 object
     *
     */
    void DigiMC_FT2(FT2 &FT2);
    
    ///  variable to handle the MonteCarlo option
    bool MC;
    
    //---------------------------- Fits  File-------------------------------------
    /**
     *@brief WriteFitsFile write the ft2fits file from SC and FT2_Time entries
     *@param FT2 address of the FT2 object
     */
    void WriteFitsFile(FT2 &FT2);
    
    /**
     *@brief WriteFitsFileMerged write the ft2fits Merged file from SC and FT2_Time entries
     *@param FT2 address of the FT2 object
     */
    void FT2::WriteFitsFileMerged(FT2 &FT2);
    
    //---------------------------- Text File -------------------------------------
    /**
     *@brief WriteFitsFile write the ft2txt file from SC and FT2_Time entries
     *@param FT2 address of the FT2 object
     */
    void WriteTextFile(FT2 &FT2);
    
    
    /**
     *@brief WriteFitsFileMerged write the ft2txt Merged file from SC and FT2_Time entries
     *@param FT2 address of the FT2 object
     */
    void WriteMergedTextFile(FT2 &FT2);
    
    
    double lininterp(double x1, double x2, double t1, double t2, double t);
    
    
    
private:
    /// variable for out of range events
    bool OutOfRange;
    
    /// varible for the current ft2 entry
    unsigned int CurrentEntry;
    
    /// variable storing the number of entries
    unsigned int Entries;
    
    /// Initialize the FT2 Class
    void init();
    
};

//----------------------Parabolic Interpolation Class---------------------------
/**
 * @class ParabInterp
 *
 * @brief this class makes the prabolic interpolation
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 */
class ParabInterp{
public:
    /**
     *@brief generates the parabolic fits over the x and y array
     *@param x array
     *@param y array
     */
    void   Interp(std::vector<double> &x, std::vector<double> &y);
    
    /**
     *@brief returns the coefficient from the parabolic fit y=a+bx+cx^2
     *@param a zero order coeff
     *@param b coeff of the linear term
     *@param c coeff of the quadratic term
     */
    void   GetCoeff(ParabInterp p, double &a, double &b, double &c);
    
    /**
     *@brief returns the values of the y given x and the prab fit coefficients
     *@param p PrabInterp object
     *@param x array
     *@param y array
     */
    void   GetInterp(ParabInterp p, double x, double &y);
private:
    
    ///zero order term of the parabola y=a+bx+cx^2
    double  pa,
            ///coefficient of the linear term of the parabola
            pb,
            ///coefficient of the quadratic term of the parabola
            pc;
};


/**
 * @class OrbInterp
 *
 * @brief this class makes the ORB interpolation using the method from gtbary
 * @author Andrea Tramacere <tramacer@slac.stanford.edu
 *
 */
class OrbInterp{
public:
    /**
     *@brief Interpolates ORB using the gtbary method
     *@param time time
     *@param ORB object adress
     *@param interp ID of the ORB entry to interpolate
     */
    void   Interp(double time, ORBIT &Orb, unsigned int i1, unsigned int i2, unsigned int interp);
    
    double inner_product(double vect_x[], double vect_y[]);
    
    double outer_product(double vect_x[], double vect_y[], double vect_z[]) ;
};

double Rad2Deg(double Angle);
double Deg2Rad(double Angle);
#endif
