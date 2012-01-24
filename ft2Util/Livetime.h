/** @file Livetime.h
@brief declare class Livetime
*/

//Avoid to include this header more than one time
#ifndef LIVETIME_H
#define LIVETIME_H

#include <set>

class TFile;
class TTree;
class DigiEvent;

#include "Ttypes.h"


namespace ft2Util
{
/**
*
* @brief Represents the livetime.

This class implements the computation of the live time for the LAT.
* @author Giacomo Vianello <giacomov@slac.stanford.edu>
*$Header$
*/
class Livetime
{
public:
  /**
  *@brief Constructor
  
  @param digiFile The input Digi file
  @param meritFile The input Merit file
  @param gapFile The input DigiGAP file
  */   
    Livetime(const std::string& digiFile, const std::string& meritFile);
    ~Livetime();
    /**
    *@brief Get the live time of the instrument between start and stop
    @param start Beginning of the time interval (MET)
    @param stop End of the time interval (MET)
    */ 
    double operator() (double start, double stop);
    /**
    *@brief Return true if the given time interval (start,stop) is covered by the current input Digi file, false otherwise.
    If the time interval is covered, then it is possible to use operator(start, stop) to get the live time.
    @param start Beginning of the time interval (MET)
    @param stop End of the time interval (MET)
    */ 
    bool isCovered(double start, double stop);
    
private:    

    /**
    *@brief Compute the conversion factor between the clock ticks and seconds
    The time counters associated with every Digi event (elapsed time and livetime) count the number of clock ticks 
    between that event and the previous one. Usually the clock ticks at 20 Mhz (nominal value), but it can change due
    to temperature variations. Here we compute the conversion factor, which will hopefully always be close to 1/20 Mhz.
    */ 
    double ticksToSeconds();
    /**
    *@brief Return the livetime (in seconds) between two events
    @param runID The ID of the current run
    @param evtID1 The event ID of the first event
    @param evtID2 The event ID of the second event
    */ 
    double getDigiEvtLivetime(Long64_t runID,const Long64_t evtID1, const Long64_t evtID2);
    /**
    *@brief Given a certain event, it returns the event ID of the previous event in the Digi file.
    @param runID The ID of the current run
    @param evtID The event ID of the current event
    */ 
    Long64_t getPreviousEventID(Long64_t runID,Long64_t evtID);
    /**
    *@brief Given a certain event, it returns the event ID of the next event in the Digi file.
    @param runID The ID of the current run
    @param evtID The event ID of the current event
    */ 
    Long64_t getNextEventID(Long64_t runID,Long64_t evtID);
    /**
    *@brief Get the time stamp (in MET) of a certain event
    @param runID The ID of the current run
    @param evtID The event ID of the event
    */
    double getEvtTimeStamp(Long64_t runID,Long64_t evtID);
    /**
    *@brief Move inside the Digi file to the given entry
    @param runID The ID of the current run
    @param evtID The event ID of the target event
    */
    Int_t GetDigiEntryWithIndex(Long64_t runID, Long64_t evtID);
    /**
    *@brief Move inside the Merit file to the given entry
    @param runID The ID of the current run
    @param evtID The event ID of the target event
    */
    Int_t GetMeritEntryWithIndex(Long64_t runID, Long64_t evtID);
    
    void openAndSetupDigiFile(const std::string& digiFile);
    void openAndSetupMeritFile(const std::string& meritFile);
    void checkSweepEvents();
    double getDeadTimeFromDigiGaps(double start, double stop, std::vector <Long64_t >& digiEvtIDs);
    double getDeadTimeFromReconCrashes(double start, double stop, std::vector <Long64_t >& digiEvtIDs);
    
    Long64_t m_runNumber;
    Long64_t m_lastEvtRow;
    double m_lastRequestedStop;
    //Note that the following sets contains the eventID of the start and stop of the gaps,
    //NOT the time
    std::set<Long64_t> m_gapStartIDs;
    std::set<Long64_t> m_gapStopIDs;
    
    bool m_hasSweepEvtBeginning, m_hasSweepEvtEnd;
    
    //Digi        
    TFile* m_digiFile;
    TTree* m_digiTree;
    DigiEvent* m_digiEvt;
    Long64_t m_nDigiEvts;
    double m_digiStart;
    Long64_t m_digiStartID;
    double m_digiStop;
    Long64_t m_digiStopID;
    
    //Merit
    TFile* m_meritFile;
    TTree* m_meritTree;
    UInt_t* m_meritEvtId;
    Long64_t m_nMeritEvts;
    double m_meritStart;
    Long64_t m_meritStartID;
    double m_meritStop;
    Long64_t m_meritStopID;
};
}
#endif
