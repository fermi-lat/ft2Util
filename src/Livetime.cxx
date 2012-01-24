#include "ft2Util/Livetime.h"
#include "facilities/Util.h"
#include "Configuration.h"

#include "digiRootData/DigiEvent.h"

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TEntryList.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iostream>

//This class implements the livetime
#include "util.h"

using namespace ft2Util;

//Here the constructor open the ROOT files (digi and merit) and set up the branches and evts
Livetime::Livetime(const std::string& digiFile, const std::string& meritFile) :
                m_lastEvtRow(0), m_lastRequestedStop(0)
{
  //Open and read the Digi file, saving useful informations
  this->openAndSetupDigiFile(digiFile);
  //Open and read the Merit file, saving useful informations
  this->openAndSetupMeritFile(meritFile);
  
  //Check for consistency between the Merit and the Digi file
  
  //Now check for the presence of sweep events
  this->checkSweepEvents();
  
  //Print a summary
  std::cout.precision(14);
  std::cout << "Livetime input:" << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "N. of Digi events                     :" << m_nDigiEvts << std::endl;
  std::cout << "Time of the first Digi event (MET)    :" << m_digiStart << std::endl;
  std::cout << "ID of the first Digi event            :" << m_digiStartID << std::endl;
  std::cout << "Time of the last Digi event (MET)     :" << m_digiStop << std::endl;
  std::cout << "ID of the last Digi event             :" << m_digiStopID << std::endl;
  std::cout << "N. of Merit events                    :" << m_nMeritEvts << std::endl;
  std::cout << "Time of the first Merit event (MET)   :" << m_meritStart << std::endl; 
  std::cout << "Time of the last Merit event (MET)    :" << m_meritStop << std::endl;
  std::cout << "Sweep events at the beginning:        :" << m_hasSweepEvtBeginning << std::endl;
  std::cout << "Sweep events at the end:              :" << m_hasSweepEvtEnd << std::endl;
  std::cout.precision(5);
  std::cout << std::endl;
}

Livetime::~Livetime()
{
  delete m_digiFile;
  delete m_meritFile;
}


void Livetime::checkSweepEvents()
{
  m_digiTree->GetEntry(1);
  Long64_t thisEventID = m_digiEvt->getEventId();
  if(thisEventID==m_digiStartID) 
  {
    //There is a sweep event at the beginning
    m_hasSweepEvtBeginning = true;
  } else 
  {
    //This should mean that the Digi file is not complete: this could be correct if the user gave us a Digi file which is the result
    //of a cut on a complete Digi file. Let's issue just a warning
    std::cout << "Livetime::Livetime(): the input Digi file lacks a sweep event at the beginning. This means it does" 
    << " not span an entire run." << std::endl;
    m_hasSweepEvtBeginning = false;
  }
  m_digiTree->GetEntry(m_nDigiEvts-2);
  thisEventID = m_digiEvt->getEventId();
  if(thisEventID==m_digiStopID) 
  {
    //There is a sweep event at the end
    m_hasSweepEvtEnd = true;
  } else 
  {
    //This could happen for a lot of reasons, so accept this quietly
    m_hasSweepEvtEnd = false;
  }
}

void Livetime::openAndSetupDigiFile(const std::string& digiFile)
{
  //Open up the digi file
  m_digiFile = new TFile(digiFile.c_str());
  m_digiTree = (TTree*) m_digiFile->Get("Digi");
  //Disable all branches and reenable only the needed ones
  m_digiTree->SetBranchStatus("*", 0);
  m_digiTree->SetBranchStatus("m_eventId", 1);
  m_digiTree->SetBranchStatus("m_runId", 1);
  m_digiTree->SetBranchStatus("m_liveTime", 1);
  m_digiTree->SetBranchStatus("m_timeStamp", 1);
  m_digiTree->SetBranchStatus("m_metaEvent", 1);
  m_digiTree->SetBranchStatus("m_gem", 1);
  m_digiTree->SetBranchStatus("DigiEvent", 1);
  //Set up the access to the tree 
  m_digiEvt = 0;
  m_digiTree->SetBranchAddress("DigiEvent", &m_digiEvt);
  Long64_t nDigiEvts = (Long64_t) m_digiTree->GetEntries();
  m_nDigiEvts = nDigiEvts; 
  
  //Now get the start and stop 
  m_digiTree->GetEntry(0);
  m_digiStart = m_digiEvt->getTimeStamp();
  m_digiStartID = m_digiEvt->getEventId();

  m_digiTree->GetEntry(nDigiEvts-1);
  m_digiStop = m_digiEvt->getTimeStamp();
  m_digiStopID = m_digiEvt->getEventId();

  m_runNumber = m_digiEvt->getRunId();
  
  //Now build the index:
  //it makes the search for events much faster
  //Actually a complete Digi file already has one, but if we are dealing with a file produced by
  //selecting a part of a complete digi file, we need to rebuild it. Note that rebuilding an index for a 
  //complete Digi file take few seconds at most  
  std::cerr << "Rebuilding Digi index...";
  m_digiTree->BuildIndex("m_runId","m_eventId");
  std::cerr << " done" << std::endl;
}

void Livetime::openAndSetupMeritFile(const std::string & meritFile)
{
  //Open up the Merit file
  m_meritFile= new TFile(meritFile.c_str(), "READ");
  m_meritTree = (TTree*) m_meritFile->Get("MeritTuple");
  //Disable all branches and reenable only the needed ones
  m_meritTree->SetBranchStatus("*", 0);
  m_meritTree->SetBranchStatus("EvtEventId", 1);
  m_meritTree->SetBranchStatus("EvtRun",1);
  m_meritTree->SetBranchStatus("EvtElapsedTime",1);

  Long64_t nMeritEvts = (Long64_t) m_meritTree->GetEntries();
  m_nMeritEvts = nMeritEvts;
  
  //Get some information about the beginning and the end of the Merit file
  m_meritTree->GetEntry(0);
  m_meritStart = (double) m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();
  m_meritStartID = (Long64_t) m_meritTree->GetLeaf("EvtEventId")->GetValue();
  m_meritTree->GetEntry(nMeritEvts-1);
  m_meritStop = (double) m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();;
  m_meritStopID = (Long64_t) m_meritTree->GetLeaf("EvtEventId")->GetValue();
  
  //Now build the index:
  //it makes the search for events much faster
  //Note that building an index for a 
  //complete Merit file take few seconds at most  
  
  std::cerr << "Building Merit index...";
  m_meritTree->BuildIndex("EvtRun","EvtEventId");
  std::cerr << " done" << std::endl << std::endl;
  
}

bool Livetime::isCovered(double start, double stop)
{
  bool covered;
  if(start >= m_digiStart && stop <= m_digiStop) 
  {
    covered = true;
  } else 
  {
    covered = false;
  }
  return covered;
}

//This is to get the livetime between start and stop
double Livetime::operator() (double start, double stop)
{    
    //First of all check for legal input
    if(!this->isCovered(start,stop))
    {
      throw std::runtime_error("Livetime::operator(): Illegal request: you requested a time interval not contained in the Digi file.");
    }
    if(start==stop) 
    {
      std::cerr << "Livetime::operator(): requested time interval lasts 0 seconds. Livetime is zero. Check for errors." << std::endl;
      return 0;
    }
    
    Long64_t startingRow=0;
    if (start >= m_lastRequestedStop && m_lastRequestedStop!=0)
    {
        //The requested time interval is after the interval requested
        //in the previous call to operator(), so we can start to look at events
        //starting from the last position processed in the previous call
        //This saves a HUGE amount of time when calling operator() on
        //several consecutive time intervals.
        //NOTE: obviously this relys on the fact that the Digi file is time ordered
        startingRow = m_lastEvtRow;
    } else
    {
        startingRow = 0;
    }
    
    
    //Get the events contained in the requested time interval:
    std::vector<Long64_t> digiEvtIDs;
    //Reserve a certain amount of memory to avoid the resizing of the vector digiEvtIDs when calling push_back()
    digiEvtIDs.reserve(10000);
    //Initialize some variables to be used in the following loop
    Long64_t thisEvtID,  nSelected = 0, nTotal = 0, i=startingRow;
    for (; i <m_nDigiEvts; ++i)
    {
        //Get the current entry
        m_digiTree->GetEntry(i);
        thisEvtID = m_digiEvt->getEventId();       
        
        //Increment the total number of considered events
        nTotal++;
        
        //Check if the current event is contained between start and stop
        double thisTime = m_digiEvt->getTimeStamp();                
        //If this event is after the stop time, we can stop looping
        if (thisTime > stop)
        {
          //Save the last entry for the next call to operator() and break
          m_lastEvtRow = i;
          m_lastRequestedStop = stop;           
          break;
        }

        //If we are here, the event is before stop: if it is after or at tstart,
        //let's select it
        if (thisTime >= start) 
        {
          digiEvtIDs.push_back(m_digiEvt->getEventId());
          nSelected++;
        }        
    }
    
    //Handle the case where there are zero events in the time interval start-stop
    if(digiEvtIDs.size()<1) 
    {
      //Print a warning
      std::cerr << "WARNING: Livetime::operator(): there are no events between " << start << " and " << stop 
      << "The livetime will be computed by distributing uniformly the livetime between the two closest events." << std::endl;
      std::cerr << "This could be wrong if the livetime has been zero because of gaps in the data (or the instrument was off)."
      << " Check for gaps, especially if the interval you requested is greater than a fraction of a second." << std::endl;
      //Get the closest event after stop, which is the last event considered in the previous loop
      m_digiTree->GetEntry(m_lastEvtRow);
      Long64_t eventAfterID = m_digiEvt->getEventId();
      double eventAfterTime = this->getEvtTimeStamp(m_runNumber,eventAfterID);
      
      Long64_t eventBeforeID = this->getPreviousEventID(m_runNumber,eventAfterID);
      double eventBeforeTime = this->getEvtTimeStamp(m_runNumber,eventBeforeID);
      
      double fraction = (stop-start)/(eventAfterTime-eventBeforeTime);
      if(fraction < 0) fraction=0;
      double livetime = this->getDigiEvtLivetime(m_runNumber,eventBeforeID, eventAfterID);
      
      return fraction*livetime;
    }
    
    //************ Compensate for the time between the first(last) event  *************
    //************ and the start(stop) of the requested interval                  **************    
    Long64_t evtBeforeID = this->getPreviousEventID(m_runNumber,digiEvtIDs.front());
    double evtBeforeTime = this->getEvtTimeStamp(m_runNumber,evtBeforeID);
    double firstEvtTime = this->getEvtTimeStamp(m_runNumber,digiEvtIDs.front());
    double fractionStart=0, totalLivetimeStart=0, compensationStart=0;
    if(evtBeforeID!=m_digiStartID)
    {
      totalLivetimeStart = this->getDigiEvtLivetime(m_runNumber,evtBeforeID,digiEvtIDs.front());
      fractionStart = (firstEvtTime-start)/(firstEvtTime-evtBeforeTime);
      if(fractionStart<0) fractionStart=0;
      compensationStart = fractionStart*totalLivetimeStart;
//       std::cerr.precision(25);
//       std::cerr << "evtBeforeID = " << evtBeforeID << ",evtBeforeTime = " << evtBeforeTime << ", firstEvtTime = " << firstEvtTime
//       << ", fractionStart = " << fractionStart << ", totalLivetimeStart = " << totalLivetimeStart 
//       << ", compensationStart = " << compensationStart << std::endl;
    } else 
    {
      //Here we are or at the beginning of a run (so there are no events before the current one)
      fractionStart=0;
      totalLivetimeStart=0;
      compensationStart=0;
    }
    
    Long64_t evtAfterID = this->getNextEventID(m_runNumber,digiEvtIDs.back());
    double evtAfterTime = this->getEvtTimeStamp(m_runNumber,evtAfterID);
    double lastEvtTime = this->getEvtTimeStamp(m_runNumber,digiEvtIDs.back());
    double fractionStop=0,totalLivetimeStop=0,compensationStop=0;
    if(evtAfterID!=m_digiStopID) 
    {
      totalLivetimeStop = this->getDigiEvtLivetime(m_runNumber,digiEvtIDs.back(),evtAfterID); 
      fractionStop = (stop-lastEvtTime)/(evtAfterTime-lastEvtTime);
      if(fractionStop <0) fractionStop = 0;
      compensationStop = fractionStop*totalLivetimeStop;
//       std::cerr << "evtAfterID = " << evtAfterID << ",evtAfterTime = " << evtAfterTime << ", lastEvtTime = " << lastEvtTime
//       << ", fractionStop = " << fractionStop << ", totalLivetimeStop = " << totalLivetimeStop 
//       << ", compensationStop = " << compensationStop << std::endl;
    } else
    {
      //Here we are or at the end of a run (so there are no events after the current one)
      fractionStop=0;
      totalLivetimeStop=0;
      compensationStop=0;
    }
    
    double compensation = compensationStart+compensationStop;
//     std::cerr << "Compensation = " << compensation << std::endl;
    
    //************ Get the corrected livetime  *************
    double livetimeBetweenEvents = this->getDigiEvtLivetime(m_runNumber,digiEvtIDs.front(),digiEvtIDs.back());
    //This is the livetime if no problems occurred:
    double livetime = livetimeBetweenEvents+compensation;
//     std::cerr << "Livetime = " << livetime << std::endl;
    if(livetime < 0) throw std::runtime_error("FATAL: Livetime::operator(): Livetime < 0! This is a bug.");
    return livetime;
}

double Livetime::getDeadTimeFromDigiGaps(double start, double stop, std::vector <Long64_t >& digiEvtIDs)
{
  //************ Handle possible gaps in the Digi file **************
  
  //Get the ID of the first and last selected events
  Long64_t firstEvtId = digiEvtIDs.front();
  Long64_t lastEvtId = digiEvtIDs.back(); 
  
  
  //     std::cerr << "Start: " << start << ", Stop: " << stop << std::endl;
  //     std::cerr << "Selected " << nSelected << " events between " << firstEvtId << " and " << lastEvtId <<  std::endl;
  
  int nRelevantGaps = 0;
  double  gapDeadtime=0;
  for (std::set<Long64_t>::iterator startGapIDItor=m_gapStartIDs.begin(), stopGapIDItor=m_gapStopIDs.begin();
  startGapIDItor != m_gapStartIDs.end() && stopGapIDItor != m_gapStopIDs.end();
  ++startGapIDItor,++stopGapIDItor)
  {
    //Compute the start and stop time for the current gap
    double startGapTime = this->getEvtTimeStamp(m_runNumber,*startGapIDItor);
    double stopGapTime = this->getEvtTimeStamp(m_runNumber,*stopGapIDItor);
    
    //         std::cerr << "         Id of the first Digi event in this interval      = " << firstEvtId << std::endl;
    //         std::cerr << "         Id of the last Digi event in this interval       = " << lastEvtId << std::endl;
    //         std::cerr << "         Id of the last good Digi event before the gap    = " << *startGapIDItor << std::endl;
    //         std::cerr << "         Time of the last good Digi event before the gap  = " << startGapTime << std::endl;
    //         std::cerr << "         Id of the first good Digi event after the gap    = " << *stopGapIDItor << std::endl;
    //         std::cerr << "         Time of the first good Digi event after the gap  = " << stopGapTime << std::endl;
    
    if (stopGapTime <= start) continue; //Gap before the requested interval, let's consider the following
      
      if (startGapTime >= stop) break; //No more gaps can be relevant for this interval
        
        //The current gap is relevant to the current time inteval, thus compute the corresponding deadtime
        //Note that at this point, because of the two "if" clause before, we are in this condition:
        //-stopGapTime > tstart, i.e., *stopGapItor > firstEvtId
        //-startGapTime < stop
        
        double thisGapDeadtime=0;
      //Deadtime due to the gap
      if ( *startGapIDItor != *stopGapIDItor)
      {
        //get the fraction of deadtime due to the gaps in this interval
        double totalGapDeadtime = this->getDigiEvtLivetime(m_runNumber, *startGapIDItor, *stopGapIDItor);
        double gapDuration = stopGapTime-startGapTime;
        double relevantStart = std::max(startGapTime,start);
        double relevantStop = std::min(stopGapTime,stop);
        double fraction = (relevantStop-relevantStart)/(stopGapTime-startGapTime);
        if(fraction<0) fraction=0;
        thisGapDeadtime = fraction * totalGapDeadtime;
        std::cerr.precision(14);
        std::cerr << "WARNING: Gap affecting the current interval (" << start << ", " << stop << ")" << std::endl;
        std::cerr << "         Id of the first Digi event in this interval      = " << firstEvtId << std::endl;
        std::cerr << "         Id of the last Digi event in this interval       = " << lastEvtId << std::endl;
        std::cerr << "         Id of the last good Digi event before the gap    = " << *startGapIDItor << std::endl;
        std::cerr << "         Time of the last good Digi event before the gap  = " << startGapTime << std::endl;
        std::cerr << "         Id of the first good Digi event after the gap    = " << *stopGapIDItor << std::endl;
        std::cerr << "         Time of the first good Digi event after the gap  = " << stopGapTime << std::endl;
        std::cerr << "         Total duration of the gap                        = " << gapDuration << std::endl;
        std::cerr << "         Total livetime comprised in the gap (lost)       = " << totalGapDeadtime << std::endl;
        std::cerr << "         Fraction of the gap relevant for this interval   = " << fraction << std::endl;
        std::cerr << "                            ---> Deadtime due to this gap = " << thisGapDeadtime << std::endl;
      } else
      {
        //This should never happen!
        throw std::runtime_error("Livetime::operator(): current gap begins and ends at the same event ID, which is impossible!");
      }
      if (thisGapDeadtime < 0)
      {
        //This should never happen!
        throw std::runtime_error("Livetime::operator(): negative gap deadtime, probably a bug in the FT2 code.");
      }
      ++nRelevantGaps;
      gapDeadtime += thisGapDeadtime;
      
  }
  if (nRelevantGaps > 0)
  {
    std::cerr << std::endl << "Found " << nRelevantGaps << " gap(s) affecting time interval (" << start << ", " << stop << ")" << std::endl;
    std::cerr << "Total deadtime due to gap(s): " << gapDeadtime << std::endl << std::endl;
  }
  return gapDeadtime;
}


double Livetime::getDeadTimeFromReconCrashes(double start, double stop, std::vector <Long64_t >& digiEvtIDs)
{
  //************ Handle possible recon crashes **************    
  double reconCrashDeadtime=0;
  Long64_t nReconCrash=0;
  for(std::vector<Long64_t>::iterator itor=digiEvtIDs.begin(); itor != digiEvtIDs.end(); ++itor)
  {
    //Check if the current event is contained in the Merit file
    Int_t meritBytesRead = GetMeritEntryWithIndex(m_runNumber,*itor);
    
    if(meritBytesRead < 0) 
    {
      //The current event is contained in the Digi, but not in the Merit: recon crash!
      std::cerr << "Recon crash: event " << *itor << " is not present in the Merit file" << std::endl;
      ++nReconCrash;
      //Compute the deadtime associated with this event, which is the difference in the livetime counter
      //between this event and the event before it. If the event before it is not contained in this interval,
      //consider only the fraction of deadtime pertinent to this interval
      Long64_t eventBeforeID = this->getPreviousEventID(m_runNumber,*itor);
      double thisDeadtime = 0;
      //Check if the event before is contained in the requested interval
      if(eventBeforeID >= digiEvtIDs.front())
      {
        //Yes, it is: all the deadtime is relevant for the requested interval
        thisDeadtime = this->getDigiEvtLivetime(m_runNumber, eventBeforeID,*itor);
        //           std::cerr << "eventBeforeID = " << eventBeforeID << " thisDeadtime = " << thisDeadtime << std::endl;
      } else
      {
        //The event before is not in the requested interval. 
        //We have to take into account only the fraction of deadtime in the requested interval
        double totalDeadTime = this->getDigiEvtLivetime(m_runNumber,eventBeforeID,*itor);
        //Get the two time stamps
        double evtBeforeTime = this->getEvtTimeStamp(m_runNumber,eventBeforeID);
        double thisTime = this->getEvtTimeStamp(m_runNumber,*itor);
        //Get the fraction of time
        double fraction = (thisTime-start)/(thisTime-evtBeforeTime);
        if(fraction<0) fraction=0;
        //Get the fraction of dead time to take into account
        thisDeadtime = fraction*totalDeadTime;
      } 
      reconCrashDeadtime += thisDeadtime;
    }   
  }
  
  if(reconCrashDeadtime > 0 && nReconCrash >0) 
  {
    std::cerr.precision(14);
    std::cerr << "Found " << nReconCrash << " Recon. crashes in time interval (" << start << ", " << stop << ")" << std::endl;
    std::cerr << "Total deadtime due to Recon crashes: " <<  reconCrashDeadtime << std::endl << std::endl;
  }
  
  return reconCrashDeadtime;
  
}
double Livetime::getEvtTimeStamp(Long64_t runID=0,Long64_t evtID=0)
{
  if (evtID!=0 && runID!=0)
  {
    GetDigiEntryWithIndex(runID,evtID); //m_digiTree->GetEntryWithIndex(runID, evtID);
  }
  double thisTime = (double) m_digiEvt->getTimeStamp();
  return thisTime;
}

Long64_t Livetime::getPreviousEventID(Long64_t runID=0,Long64_t evtID=0)
{
   //If the current event is the first one, there is nothing before: I return the ID of the first event in the file
    if(evtID==m_digiStartID) return m_digiStartID;
  
    if (evtID!=0 && runID!=0)
    {
        GetDigiEntryWithIndex(runID, evtID);//m_digiTree->GetEntryWithIndex(runID, evtID);
    } else
    {
        //Get current evt id
        evtID = m_digiEvt->getEventId();
        runID = m_digiEvt->getRunId();
    }
    
    //Get the ordinal number of the current event
    Long64_t thisEventNumber = m_digiTree->GetEntryNumberWithIndex(runID, evtID);
    //Move to the previous event
    Int_t bytesRead = m_digiTree->GetEntry(thisEventNumber-1);
    if(bytesRead <= 0) 
    {
      throw std::runtime_error("Livetime::getPreviousEventID(): Input/Output error while reading the digi file.");
    }
    //Get the event ID of the current event, which is the event before evtID
    Long64_t  eventBeforeID = m_digiEvt->getEventId();
    return eventBeforeID;
}

Long64_t Livetime::getNextEventID(Long64_t runID=0,Long64_t evtID=0)
{
  //If the current event is the last one, there is nothing after that: I return the ID of the last event in the file
  if(evtID==m_digiStopID) return m_digiStopID;
  
  if (evtID!=0 && runID!=0)
  {
    GetDigiEntryWithIndex(runID, evtID); //m_digiTree->GetEntryWithIndex(runID, evtID);
  } else
  {
    //Get current evt id
    evtID = m_digiEvt->getEventId();
    runID = m_digiEvt->getRunId();
  }
  
  //Get the ordinal number of the current event 
  Long64_t thisEventNumber = m_digiTree->GetEntryNumberWithIndex(runID, evtID);
  //Move to the next event
  Int_t bytesRead = m_digiTree->GetEntry(thisEventNumber+1);
  if(bytesRead <= 0) 
  {
    throw std::runtime_error("Livetime::getNextEventID(): Input/Output error while reading the digi file.");
  }
  //Get the event ID and return it
  Long64_t  eventAfterID = m_digiEvt->getEventId();
  return eventAfterID;
}

double Livetime::getDigiEvtLivetime(Long64_t runID,const Long64_t evtID1, const Long64_t evtID2)
{
  
  if(evtID1==evtID2) return 0;
  
  //Loop over all the events between evtID1 and evtID2, and sum the livetimes. This is probably an overkill,
  //since we could just take the first and last event, and take the difference between the livetime counter.
  //But, going event-by-event does not slow down much, and naturally take into account possible clock drifts,
  //especially if there is a lot of time between evtID1 and evtID2.
  
  //Get the starting entry number
  Long64_t event1Number = m_digiTree->GetEntryNumberWithIndex(runID, evtID1); 
  //Get the entry number of the second event
  Long64_t event2Number = m_digiTree->GetEntryNumberWithIndex(runID, evtID2);
  
  //Get the livetime ticks for the first event
  this->GetDigiEntryWithIndex(runID,evtID1);
  //Get the livetime counter value for the first event
  ULong64_t livetime1Ticks =  m_digiEvt->getMetaEvent().scalers().livetime();
  
  //Now sum the livetimes. Given two events, the livetime (in seconds) is the number of ticks
  //between the two events, times the conversion factor between ticks and seconds
  double totalLivetime=0;
  double prevLivetimeTicks = livetime1Ticks;
  for(Long64_t thisEntry=event1Number+1; thisEntry <= event2Number;++thisEntry)
  {
    m_digiTree->GetEntry(thisEntry);
    ULong64_t thisLivetimeTicks =  m_digiEvt->getMetaEvent().scalers().livetime();
    //Here we are converting from a Long64_t to a double, which is potentially dangerous. So, I convert only
    //the difference between the two livetime counters. Given the event rate in the LAT, it will certainly fit into a double.
    double thisLivetime = ((double)(thisLivetimeTicks-prevLivetimeTicks))*this->ticksToSeconds();
    
    if(thisLivetimeTicks-prevLivetimeTicks < 0) 
    {
      std::stringstream str;
      str << "Livetime::getDigiEvtLivetime(): the livetime counter went backward between entry " << thisEntry-1 << " and entry " 
      << thisEntry;
      throw std::runtime_error(str.str());
    }
    totalLivetime += thisLivetime;
    prevLivetimeTicks = thisLivetimeTicks;
  }
  
  //Now let's correct the livetime if there were sweep events involved
  if(m_hasSweepEvtBeginning && evtID1==m_digiStartID)
  {
    //At the beginning of a run, there are three things happening:
    //A) Sweep event issued (solicited trigger)
    //B) Window open for physics
    //C) End of the read out of the sweep event
    //D) First physics event of the run
    //The opening of the window (B) is independent from the actual reading of the sweep event. So, if the reading of the sweep
    //event takes longer than the time between A and B, the livetime between the sweep event and the first real event is just
    //the difference in the livetime counters (as usual):
    // A ----------B --- C --- D
    //If, on the contrary, the reading of the sweep event is faster than the time between A and B, there is an additional 
    //deadtime to take into account, which is the time occurring from the end of the 
    //reading of the sweep event (C) and the opening of the window (B):
    // A ------C ---- B ------ D
    // The time between A and B is always 10 x ticksPerCommand (currently 10 x 196 = 1960 ticks). Thus, to understand
    // if C is before B, we can take the deadtime between A and D: if this deadtime is > 1960 ticks, then we are in the first case
    // otherwise we are in the second case
    this->GetDigiEntryWithIndex(m_runNumber,m_digiStartID);
    ULong64_t sweepElapsed = m_digiEvt->getMetaEvent().scalers().elapsed();
    ULong64_t sweepLivetime = m_digiEvt->getMetaEvent().scalers().livetime();
    
    Long64_t firstEvtID = this->getNextEventID(m_runNumber,m_digiStartID);
    this->GetDigiEntryWithIndex(m_runNumber,firstEvtID);
    ULong64_t firstEvtElapsed = m_digiEvt->getMetaEvent().scalers().elapsed();
    ULong64_t firstEvtLivetime = m_digiEvt->getMetaEvent().scalers().livetime();
    
    ULong64_t elapsedTimeTicks = firstEvtElapsed-sweepElapsed;
    ULong64_t livetimeTicks  = firstEvtLivetime-sweepLivetime; 
    ULong64_t deadtimeTicks = elapsedTimeTicks-livetimeTicks;
    
    if(deadtimeTicks <= 10 * ft2Util::Configuration::Instance()->ticksPerCommand) 
    {
      totalLivetime -= (10 * ft2Util::Configuration::Instance()->ticksPerCommand *this->ticksToSeconds() );
    }
    
  }
  if(m_hasSweepEvtEnd && evtID2==m_digiStopID)
  {
    //Here the situation is very similar to the other one. The three things happening are:
    //A) Last physics event of the run
    //B) Window close for physics
    //C) End of the read out of the last physics event
    //D) Sweep event issued
    //The issue of the sweep event (D) will always happen sweepEventsDelayEnd + (10 x ticksPerCommand) ticks
    //after the closing of the window (B). Currently this is 20000 + (10 x 196) = 21960 ticks. The read out time for the last 
    //physics event can determine two situations:
    //A------B----C------D
    //or:
    //A----C----B----D
    //In the first case we don't need any correction, while in the second case we have to subtract the time between 
    //B and D
    this->GetDigiEntryWithIndex(m_runNumber,m_digiStopID);
    ULong64_t sweepElapsed = m_digiEvt->getMetaEvent().scalers().elapsed();
    ULong64_t sweepLivetime = m_digiEvt->getMetaEvent().scalers().livetime();
    
    Long64_t lastEvtID = this->getPreviousEventID(m_runNumber,m_digiStopID);
    this->GetDigiEntryWithIndex(m_runNumber,lastEvtID);
    ULong64_t lastEvtElapsed = m_digiEvt->getMetaEvent().scalers().elapsed();
    ULong64_t lastEvtLivetime = m_digiEvt->getMetaEvent().scalers().livetime();
    
    ULong64_t elapsedTimeTicks = sweepElapsed-lastEvtElapsed;
    ULong64_t livetimeTicks  = sweepLivetime-lastEvtLivetime; 
    ULong64_t deadtimeTicks = elapsedTimeTicks-livetimeTicks;
    
    double delayTicks = ft2Util::Configuration::Instance()->sweepEventsDelayEnd + 10*ft2Util::Configuration::Instance()->ticksPerCommand;
    
    if(deadtimeTicks <= (delayTicks))
    {
      totalLivetime -= delayTicks*this->ticksToSeconds();
    }
  }
  
  return totalLivetime;
}

Int_t Livetime::GetDigiEntryWithIndex(Long64_t runID, Long64_t evtID)
{
  //This is to save us from the fact that GetEntryWithIndex hangs up if it is requested
  //for and eventID which is duplicated. Since in every Digi and every Merit the first and
  //last event is a "sweep event", which IS ALWAYS duplicated, we need to write this
  //workaround
  if(runID !=m_runNumber) throw std::runtime_error("Livetime::GetDigiEntryWithIndex(): requested for the wrong run ID. This is a bug!");
  
  Int_t bytesRead = -1;
  if(evtID == m_digiStartID) 
  {
    m_digiTree->GetEntry(0);
    bytesRead = 1;
  } else if (evtID == m_digiStopID)
  {
    m_digiTree->GetEntry(m_nDigiEvts-1);
    bytesRead = 1;
  } else
  {
    bytesRead = m_digiTree->GetEntryWithIndex(runID, evtID);
  }
  
  if(bytesRead < 0) 
  {
    throw std::runtime_error("Livetime::GetDigiEntryWithIndex(): Input/Output error while reading the Digi file.");
  }
  
  return bytesRead;
}

Int_t Livetime::GetMeritEntryWithIndex(Long64_t runID, Long64_t evtID)
{
  //This is to save us from the fact that GetEntryWithIndex hangs up if it is requested
  //for and eventID which is duplicated. Since in every Digi and every Merit the first and
  //last event is a "sweep event", which IS ALWAYS duplicated, we need to write this
  //workaround
  if(runID !=m_runNumber) throw std::runtime_error("Livetime::GetMeritEntryWithIndex(): requested for the wrong run ID. This is a bug!");
  
  Int_t bytesRead = -1;
  if(evtID == m_meritStartID) 
  {
    m_meritTree->GetEntry(0);
    bytesRead = 1;
  } else if (evtID == m_meritStopID)
  {
    m_meritTree->GetEntry(m_meritTree->GetEntries()-1);
    bytesRead = 1;
  } else
  {
    bytesRead = m_meritTree->GetEntryWithIndex(runID, evtID);
  }
  
  if(bytesRead < 0) 
  {
    throw std::runtime_error("Livetime::GetMeritEntryWithIndex(): Input/Output error while reading the Digi file.");
  }
  
  return bytesRead;
}

double Livetime::ticksToSeconds()
{
    double conv;
    double RollOver = ft2Util::Configuration::Instance()->RollOver;
    double nominalValue = ft2Util::Configuration::Instance()->nominalTicksToSecond;
    double clockTicksDelta1PPS = 0;
    //If the time tone for the current event, and the previous one, are ok,
    //use them to compute the conversion factor, otherwise use the nominal one:
    if (!(m_digiEvt->getMetaEvent().time().current().missingCpuPps()) &&
        !(m_digiEvt->getMetaEvent().time().current().missingLatPps()) &&
        // That was the current Timetone. Now the previous one. 
        !(m_digiEvt->getMetaEvent().time().previous().missingCpuPps()) &&
        !(m_digiEvt->getMetaEvent().time().previous().missingLatPps()) &&
        // Avoid 1/0 error: This could probably be replaced by using 'RollOver' instead  
        // because of the subsequent requirement.   
        (m_digiEvt->getMetaEvent().time().current().timeHack().ticks() != 
        m_digiEvt->getMetaEvent().time().previous().timeHack().ticks()) &&  
        // If there is more than a second between 1-PPS I can 
        // only use the nominal value for the LAT clock anyway!
        ((m_digiEvt->getMetaEvent().time().current().timeSecs() -
         m_digiEvt->getMetaEvent().time().previous().timeSecs()) == 1)) {

        // Number of ticks between the current and the previous time tone:
        clockTicksDelta1PPS = double (m_digiEvt->getMetaEvent().time().current().timeHack().ticks()) -
                                     double (m_digiEvt->getMetaEvent().time().previous().timeHack().ticks());

        // Rollover?
        if (clockTicksDelta1PPS < 0) {
            clockTicksDelta1PPS = clockTicksDelta1PPS + RollOver;
        }

        conv = (1.0) / clockTicksDelta1PPS;
    } else
    {
        //Problems with the GPS signal, use the nominal value for the LAT clock
        conv = nominalValue;
    }
    return conv;
}
