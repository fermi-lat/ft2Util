#include "ft2Util/GapHandler.h"
#include "ft2Util/TimeInterval.h"
#include "facilities/Util.h"
#include "Configuration.h"

#include "digiRootData/DigiEvent.h"


#include "Ttypes.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TTree.h"

#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <fstream>

namespace ft2Util
{
  GapHandler::GapHandler(const std::string& digiFile, const std::string& meritFile, 
                         const std::string& digiGapFile, std::vector<TimeInterval> &timeIntervals) :
        m_prevDigiID(0), m_prevDigiEvtLivetime(0),m_prevMeritID(0)
{
    //This method check the Digi and the Merit file for lacking events/gaps. If one or more event are in the Digi file but not in the
    //Merit file (or the other way around) it means there has been some problems either in the merging process or in the recon
    //process (hopefully temporary). In both cases this method add one time interval to timeIntervals, covering the time of each gap.
    //Such time intervals will have livetime = 0, so that they will be excluded from the GTIs by downstream software.
    //The same thing is performed when there are digi gaps.

    this->openDigiFile(digiFile);
    this->openMeritFile(meritFile);

    //Check that the digi and the Merit cover the requested time span
    m_requestedStartTime = timeIntervals.front().start();
    m_requestedStopTime = timeIntervals.back().stop();
    if (m_digiStart > m_requestedStartTime || m_meritStart > m_requestedStartTime ||
            m_digiStop < m_requestedStopTime || m_meritStop < m_requestedStopTime)
    {
        std::cerr.precision(40);
        std::cerr << "Digi time span:            " << m_digiStart << " - " << m_digiStop << std::endl;
        std::cerr << "Merit time span:           " << m_meritStart << " - " << m_meritStop << std::endl;
        std::cerr << "Requested time span:       " << m_requestedStartTime << " - " << m_requestedStopTime << std::endl;
        throw std::runtime_error("GapHandler: the Digi and/or the Merit do not cover the requested time span. Check start and stop time.");
    }
    
    //parse digiGap file and setup the corresponding bad time intervals (if any)
    this->parseGapFile(digiGapFile);
    
    //Verify that Digi and Merit are ok, and in synch
    this->verifyDigiAndMerit();
    
    //Now insert the bad time intervals in timeIntervals
    this->mergeGoodAndBadTimeIntervals(timeIntervals);    
}

GapHandler::~GapHandler()
{
    delete m_digiFile;
    delete m_meritFile;
}

void GapHandler::mergeGoodAndBadTimeIntervals(std::vector<TimeInterval> &timeIntervals)
{
  //Now insert the bad time intervals (if any) in the timeIntervals vector
  if (!m_badTimeIntervals.empty())
  {
    //We have some bad time intervals
    std::cerr << "\nWARNING: There are gaps either in the Merit either in the Digi file (or both)." << std::endl << std::endl;
    
    //In the following we will use the insert method of the vector class, which could generate
    //a reallocation of the vector if we run out of capacity. Reallocation makes invalid every iterator
    //previously obtained, which can be dangerous. Thus, we reserve memory enough to avoid
    //reallocation
    timeIntervals.reserve(timeIntervals.size()+2*m_badTimeIntervals.size());
    
    //Now loop on the bad time intervals and add them in the right position:
    // GTIs:     |      |      |      |    |    |     |        |          |           |
    //  BTIs:              |xxxx|    |xxxxx|        |xxxxxxxxxxxxxx|
    for (std::vector<TimeInterval>::iterator badItor=m_badTimeIntervals.begin();badItor !=m_badTimeIntervals.end();++badItor)
    {
      //Find the first element in timeIntervals with a start time greater than the start time of the current bad time
      //interval
      for (std::vector<TimeInterval>::iterator itor=timeIntervals.begin();itor !=timeIntervals.end();++itor)
      {
        if (itor->start() >= badItor->start())
        {
          //Remove all the intervals entirely contained in the bad time interval (if any)
          while (itor->stop() <= badItor->stop())
          {
            //Yes! Remove this time interval
            itor = timeIntervals.erase(itor);
          }
          if (badItor->contains(itor->start()))
          {
            //If the bad time interval contains the start time of the current interval, we are in a situation like this:
            // |          |                    |
            //       |xxxxxxx|
            //which has to become:
            // |     |xxxxxxx|          |
            
            //The previous time interval (which used to have itor->start() as stop time) has to be modified to have
            //badItor->start() as stop time
            if (itor!=timeIntervals.begin())
            {
              std::vector<TimeInterval>::iterator prevItor = itor-1;
              prevItor->setStop(badItor->start());
              //Note that the previous interval keeps its status: good if it was good, bad if it was bad
              
            } else
            {
              //The bad time interval begin before the beginning of the first interval,
              //thus cut it to that time
              badItor->setStart(itor->start());
            }
            
            //The current interval has to be modified to have badItor->stop() as start time
            itor->setStart(badItor->stop());
            
            //Now insert the bad time interval before the current one
            timeIntervals.insert(itor,*badItor);
          } else
          {
            //We are in a situation like this:
            // |              |                |
            //    |xxxx|
            //which has to become:
            // |  |xxxx|   |              |
            
            //The previous time interval (which used to have itor->start() as stop time) has to be modified to have
            //badItor->start() as stop time
            std::vector<TimeInterval>::iterator prevItor = itor-1;
            prevItor->setStop(badItor->start());
            //Note that the previous interval keeps its status: good if it was good, bad if it was bad
            
            //Now we have to insert two new intervals: the bad time interval itself, and the interval going
            //from the end of the bad time interval to the start of the current interval
            //Let's create the new interval, which has the status of the old "big interval"
            bool newStatus=true;
            if (!prevItor->isGood()) newStatus=false;
            std::vector<TimeInterval> twoIntervalsToInsert;
            twoIntervalsToInsert.push_back(*badItor);
            twoIntervalsToInsert.push_back(TimeInterval(badItor->stop(),itor->start(),newStatus));
            
            //Insert both the intervals
            timeIntervals.insert(itor,twoIntervalsToInsert.begin(),twoIntervalsToInsert.end());
          }
          //Search no more for the place for the current bad time interval
          break;
        }
        
      }
    }
    //Print the time intervals
    std::cout << std::endl;
    std::cout << "Bad time intervals:" << std::endl;
    for (std::vector<TimeInterval>::iterator itor=m_badTimeIntervals.begin();itor !=m_badTimeIntervals.end();++itor)
    {
      std::cout.precision(25);
      std::cout << itor->start() << " - " << itor->stop() << std::endl;
    }
    std::cout << std::endl;
  }
  
}

void GapHandler::parseGapFile(const std::string &gapFile)
{
    //Open the gap file, which have a structure like:
    //"Run ID"  "Last event before the gap" "First event after the gap"
    //r0xxxxxxxxx 79194 83166

    //open the text file
    std::ifstream input_file;
    input_file.open(gapFile.c_str(), std::ifstream::in);
    if (!input_file.is_open()) {
        std::cerr << "FATAL ERROR:  Unable to open Gap file :  " << gapFile << std::endl;
        throw std::runtime_error("GapHandler::parseGapFile(): could not open Gap file");
    }
    int linePos=0;
    
    int numberOfGaps = 0;
    
    //Infinite loop, will exit with break
    while (true) {
        linePos++;
        std::string line;
        std::getline(input_file, line);

        if (input_file.eof())
        {
            //End of file
            break;
        }
        std::vector<std::string> tokens;
        facilities::Util::stringTokenize(line, " ",  tokens);
        //Remove the leading "r" in the run number
        tokens[0].erase(0,1);
        //Insert in the current list of gaps only the gaps relevant for the current run:
        if (facilities::Util::atoi(tokens[0])==static_cast<int>( m_runNumber ))
        {
            //Note that these are event Ids, NOT times
            Long64_t gapStartId = (Long64_t) facilities::Util::stringToUnsigned(tokens[1]);
            Long64_t gapStopId = (Long64_t) facilities::Util::stringToUnsigned(tokens[2]);
            
            if( gapStartId >= gapStopId ) 
            {
              std::cerr << "The ID of the event before the gap (" << gapStartId 
                        << ") is equal or greater than the ID of the event after the gap (" << gapStopId << "), which is impossible!" << std::endl;
              throw std::runtime_error("GapHandler::parseGapFile(): fatal error in the gap file, the start and stop eventID are not sequential.");
            }
            
            double padding = ft2Util::Configuration::Instance()->deadPad*1E-6;

            //Now get the corresponding time stamps
            m_digiTree->GetEntryWithIndex(m_runNumber,gapStartId);
            double gapStartTime = m_digiEvt->getTimeStamp()+padding;

            m_digiTree->GetEntryWithIndex(m_runNumber,gapStopId);
            double gapStopTime = m_digiEvt->getTimeStamp()-padding;

            //Set up the time interval
            try {
              TimeInterval gap(gapStartTime,gapStopTime);
              gap.setAsBad();
              m_badTimeIntervals.push_back(gap);
              numberOfGaps++;
            } catch (...) {
              std::cerr << "GapHandler::parseGapFile(): could not create a bad time interval between event "
                        << gapStartId << " (at time " << gapStartTime << ") and event "
                        << gapStopId  << " (at time " << gapStopTime  << ")." << std::endl;
              throw std::runtime_error("GapHandler::parseGapFile(): fatal error in the gap file and/or Digi file, gap with zero or negative duration!");                      
            }            
        }
    }
    input_file.close();
    //summarize
    std::cout << std::endl;
    std::cout << "Gap file " << gapFile << " parsed:" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "N. of gaps for this run               :" << numberOfGaps << std::endl;
    if (m_badTimeIntervals.size()>0)
    {
        std::cout << "gaps:" << std::endl;
        for (std::vector<TimeInterval>::iterator itor=m_badTimeIntervals.begin();itor !=m_badTimeIntervals.end();++itor)
        {
            std::cout.precision(25);
            std::cout << itor->start() << " - " << itor->stop() << std::endl;
        }
    }
    std::cout << std::endl;
}

void GapHandler::openDigiFile(const std::string& digiFile)
{
    //Open Digi file
    m_digiFile = new TFile(digiFile.c_str());
    m_digiTree = (TTree*) m_digiFile->Get("Digi");
    //Disable all branches and reenable only the needed ones
    m_digiTree->SetBranchStatus("*", 0);
    m_digiTree->SetBranchStatus("m_eventId", 1);
    m_digiTree->SetBranchStatus("m_runId", 1);
    m_digiTree->SetBranchStatus("DigiEvent", 1);
    m_digiTree->SetBranchStatus("m_timeStamp", 1);
    m_digiTree->SetBranchStatus("m_metaEvent", 1);
    m_digiTree->SetBranchStatus("m_gem", 1);

    m_nDigiEvts = (Long64_t) m_digiTree->GetEntries();

    m_digiEvt = 0;
    m_digiTree->SetBranchAddress("DigiEvent",&m_digiEvt);

    //Now build the index:
    //it makes the search for events much faster
    //Actually a complete Digi file already has one, but if we are dealing with a file produced by
    //selecting a part of a complete digi file, we need to rebuild it. Note that rebuilding an index for a
    //complete Digi file take few seconds at most
    std::cerr << "Rebuilding Digi index...";
    m_digiTree->BuildIndex("m_runId","m_eventId");
    std::cerr << " done" << std::endl;

    if (! this->digiSpansOneRun())
    {
        throw std::runtime_error("GapHandler::the digi file spans more than one run. Can't handle it.");
    }    
}

bool GapHandler::digiSpansOneRun()
{
    //Verify that Digi spans only one run

    //Now get the start and stop
    m_digiTree->GetEntry(0);
    m_digiStart = m_digiEvt->getTimeStamp();
    m_digiStartID = m_digiEvt->getEventId();
    Long64_t runNumberOfTheFirstEvent = m_digiEvt->getRunId();
    m_digiTree->GetEntry(m_nDigiEvts-1);
    m_digiStop = m_digiEvt->getTimeStamp();
    m_digiStopID = m_digiEvt->getEventId();
    Long64_t runNumberOfTheLastEvent = m_digiEvt->getRunId();

    if (runNumberOfTheFirstEvent==runNumberOfTheLastEvent)
    {
        m_runNumber = runNumberOfTheFirstEvent;
        return true;
    } else
    {
        return false;
    }
}

void GapHandler::openMeritFile(const std::string& meritFile)
{
    //Open Merit file
    m_meritFile= new TFile(meritFile.c_str(), "READ");
    m_meritTree = (TTree*) m_meritFile->Get("MeritTuple");
    //Disable all branches and reenable only the needed ones
    m_meritTree->SetBranchStatus("*", 0);
    m_meritTree->SetBranchStatus("EvtEventId", 1);
    m_meritTree->SetBranchStatus("EvtRun",1);
    m_meritTree->SetBranchStatus("EvtElapsedTime",1);

    //Verify that Digi spans only one run
    m_nMeritEvts = (Long64_t) m_meritTree->GetEntries();

    //Now build the index:
    //it makes the search for events much faster
    //Note that building an index for a
    //complete Merit file take few seconds at most

    std::cerr << "Building Merit index...";
    m_meritTree->BuildIndex("EvtRun","EvtEventId");
    std::cerr << " done" << std::endl << std::endl;

    if (! this->meritSpansOneRun())
    {
        throw std::runtime_error("GapHandler::the merit file spans more than one run. Can't handle it.");
    }
}

bool GapHandler::meritSpansOneRun()
{


    //Now get the start and stop
    //Get some information about the beginning and the end of the Merit file
    m_meritTree->GetEntry(0);
    m_meritStart = (double) m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();
    m_meritStartID = (Long64_t) m_meritTree->GetLeaf("EvtEventId")->GetValue();
    Long64_t runNumberOfTheFirstEvent = (Long64_t) m_meritTree->GetLeaf("EvtRun")->GetValue();
    m_meritTree->GetEntry(m_nMeritEvts-1);
    m_meritStop = (double) m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();;
    m_meritStopID = (Long64_t) m_meritTree->GetLeaf("EvtEventId")->GetValue();
    Long64_t runNumberOfTheLastEvent = (Long64_t) m_meritTree->GetLeaf("EvtRun")->GetValue();

    if (runNumberOfTheFirstEvent==runNumberOfTheLastEvent)
    {
        return true;
    } else
    {
        return false;
    }
}

void GapHandler::verifyDigiAndMerit()
{

    //Check that the Merit file covers the time period covered by the Digi file
    if ( (m_meritStop < m_digiStart) || //merit covers an interval different (and before) the Digi one
            (m_meritStart > m_digiStop) //merit cover an interval different (and after) the Digi one
       )
    {
        std::cerr << "Digi: " << m_digiStart << " " << m_digiStop << "\nMerit: " << m_meritStart << " " << m_meritStop << std::endl;
        throw std::runtime_error("GapHandler(): the Merit file and the Digi file apparently cover two different time spans.");
    }
    
    //Now check the two trees event by event, enforcing eventID order, and time order (i.e., livetime counter increasing)

    //Loop over Digi and Merit looking for lacking events
    bool digiIsSweep = false;
    bool meritIsSweep = false;

    std::cout << std::endl << "Checking Merit and Digi for errors or gaps: " << std::endl << std::endl;

    //First of all go in both trees to the first event included in the requested time interval
    Long64_t digiI=0,meritI=0;
    for (;digiI < m_nDigiEvts;++digiI)
    {
        m_digiTree->GetEntry(digiI);
        double digiTimestamp = m_digiEvt->getTimeStamp();
        if (digiTimestamp >= m_requestedStartTime)
        {
//             std::cerr << "Digi in sync" << std::endl;
            break;
        }
    }
    for (;meritI < m_nMeritEvts;++meritI)
    {
        m_meritTree->GetEntry(meritI);
        double meritEvtLivetime = m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();
        if (meritEvtLivetime >= m_requestedStartTime)
        {
//             std::cerr << "Merit in sync" << std::endl;
            break;
        }
    }

    int outSteps = (int) std::ceil((m_nDigiEvts-digiI-1)/10.0);
    for (;digiI < m_nDigiEvts && meritI < m_nMeritEvts;++digiI,++meritI)
    {
        m_digiTree->GetEntry(digiI);
        m_curDigiEvtId = (Long64_t) m_digiTree->GetLeaf("m_eventId")->GetValue();

        m_meritTree->GetEntry(meritI);
        m_curMeritEvtId = (Long64_t) m_meritTree->GetLeaf("EvtEventId")->GetValue();

        if (digiI % outSteps == 0)
        {
            std::cerr.precision(2);
            std::cerr  << std::setw(5) << 100.0*double(digiI)/m_nDigiEvts  << " % completed" << std::endl;
            std::cerr.precision(14);
        }
//         std::cerr << "Digi : " << m_curDigiEvtId << "(" << m_prevDigiID << ")\nMerit: "
//         << m_curMeritEvtId << "(" << m_prevDigiID << ")" <<  std::endl;

        if (m_prevDigiID==m_curDigiEvtId && (m_prevDigiID==m_digiStartID || m_prevDigiID==m_digiStopID))
        {
            digiIsSweep = true;
        } else {
            digiIsSweep = false;
        }
        if (m_prevMeritID==m_curMeritEvtId && (m_prevMeritID==m_meritStartID || m_prevMeritID==m_meritStopID))
        {
            meritIsSweep = true;
        } else {
            meritIsSweep = false;
        }

        //Enforce ordered event IDs in both trees (taking into account sweep events)
        if (!digiIsSweep) this->enforceOrderedEventID(m_prevDigiID,m_curDigiEvtId,"Digi");

        if (!meritIsSweep) this->enforceOrderedEventID(m_prevMeritID,m_curMeritEvtId,"Merit");

        //Enforce time order in Digi (since the events in Merit are taken from the Digi, and we already know they are ordered by
        //eventID, we don't need to check also the Merit for this)
        ULong64_t digiEvtLivetime = m_digiEvt->getMetaEvent().scalers().livetime();
        /*        std::cerr << "digiEvtLivetime : " << digiEvtLivetime << std::endl;*/
        if (!digiIsSweep && m_prevDigiEvtLivetime!=0) this->enforceTimeOrder(m_prevDigiEvtLivetime,digiEvtLivetime);
        m_prevDigiEvtLivetime=digiEvtLivetime;

        double digiTimestamp = m_digiEvt->getTimeStamp();
        if (digiTimestamp > m_requestedStopTime)
        {
            std::cerr << " Requested stop time is before the end of the data, no need to check further." << std::endl;
            break;
        }

        //Now check if we are dealing with the same events in both Digi both Merit. If not, we probably had a merging problem,
        //or a recon crash (i.e., Recon failed to reconstruct the event)
        if (m_curDigiEvtId != m_curMeritEvtId)
        {
            //We have a gap, either in the Digi or in the Merit
            Long64_t nLost = 0;
            if (m_curDigiEvtId < m_curMeritEvtId)
            {
                std::cerr << "GapHandler: gap in the Merit file between event " << m_prevMeritID << " and event " << m_curMeritEvtId << std::endl;
                //The current Digi event did not make into the Merit (recon crash, or merging problem with the merit)
                this->handleMeritGap();
                //Now get the two trees in sync again
                meritI = m_meritTree->GetEntryNumberWithIndex(m_runNumber,m_curMeritEvtId); //useless
                Long64_t newDigiI = m_digiTree->GetEntryNumberWithIndex(m_runNumber,m_curMeritEvtId);
                nLost = newDigiI-digiI;
                digiI = newDigiI;
            } else
            {
                std::cerr << "GapHandler: gap in the Digi file between event " << m_prevDigiID << " and event " << m_curDigiEvtId << std::endl;
                //digiEvtId > meritEvtId, thus we have an event which is in the Merit, but not in the Digi
                this->handleDigiGap();
                //Now get the two trees in sync again
                Long64_t newMeritI = m_meritTree->GetEntryNumberWithIndex(m_runNumber,m_curDigiEvtId);
                digiI = m_digiTree->GetEntryNumberWithIndex(m_runNumber,m_curDigiEvtId); //useless
                nLost = newMeritI-meritI;
                meritI = newMeritI;
            }
            std::cerr << "You lost " << nLost << " event (these events are contained"
                      << " in a bad time interval, which will be excluded by downstream software)" << std::endl << std::endl;
        }
        m_prevDigiID = m_curDigiEvtId;
        m_prevMeritID=m_curMeritEvtId;
    }
}

void GapHandler::handleMeritGap()
{
    //We have one or more event lacking in the Merit file. The current event is the first event after the gap.
    //Thus, the bad time interval goes from just after the event before the current one, to just before the current
    //event.

    Long64_t meritEventAfterTheGapNumber = m_meritTree->GetEntryNumberWithIndex(m_runNumber,m_curMeritEvtId);
    Long64_t meritEventBeforeTheGapNumber = meritEventAfterTheGapNumber-1;

    //This is the padding to add (subtract) from the time stamp of the event before (after) the gap, to avoid
    //the inclusion of the two good events in the bad time interval
    double padding = ft2Util::Configuration::Instance()->deadPad*1E-6;

    m_meritTree->GetEntry(meritEventBeforeTheGapNumber);
    double timeBeforeGap = m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();

    m_meritTree->GetEntry(meritEventAfterTheGapNumber);
    double timeAfterGap = m_meritTree->GetLeaf("EvtElapsedTime")->GetValue();
//     std::cerr.precision(30);
//     std::cerr << "timeBeforeGap: " << timeBeforeGap << " timeAfterGap: " << timeAfterGap << std::endl;
//     std::cerr << timeBeforeGap+padding << " " << timeAfterGap-padding << std::endl;
    //Add this time interval to the vector of bad time intervals
    TimeInterval newBadTimeInterval(timeBeforeGap+padding,timeAfterGap-padding);
    newBadTimeInterval.setAsBad();
    m_badTimeIntervals.push_back(newBadTimeInterval);
}

void GapHandler::handleDigiGap()
{
    //We have one or more event lacking in the Digi file. The current event is the first event after the gap.
    //Thus, the bad time interval goes from just after the event before the current one, to just before the current
    //event.
    Long64_t digiEventAfterTheGapNumber = m_digiTree->GetEntryNumberWithIndex(m_runNumber,m_curDigiEvtId);
    Long64_t digiEventBeforeTheGapNumber = digiEventAfterTheGapNumber-1;

    double padding = ft2Util::Configuration::Instance()->deadPad*1E-6;
    m_digiTree->GetEntry(digiEventBeforeTheGapNumber);
    double timeBeforeGap = m_digiEvt->getTimeStamp();

    m_digiTree->GetEntry(digiEventAfterTheGapNumber);
    double timeAfterGap = m_digiEvt->getTimeStamp();

    //Add this time interval to the vector of bad time intervals
    TimeInterval newBadTimeInterval(timeBeforeGap+padding,timeAfterGap-padding);
    newBadTimeInterval.setAsBad();
    m_badTimeIntervals.push_back(newBadTimeInterval);
}

inline void GapHandler::enforceTimeOrder(ULong64_t t1, ULong64_t t2)
{
    if (t2 <= t1)
    {
        std::cerr << "t1 : " << t1 << " t2: " << t2 << std::endl;
        std::cerr << "GapHandler::out of order event in the Digi file. Event ID = " << m_curDigiEvtId << std::endl;
        throw std::runtime_error("GapHandler::out-of-order event in the Digi file.");
    }
}

inline void GapHandler::enforceOrderedEventID(Long64_t eventID1, Long64_t eventID2, const std::string tree)
{
    if (eventID2 <= eventID1)
    {
        std::cerr << "GapHandler::event " << eventID1 << " and event " << eventID2 << " have out of order eventId in " << tree << " tree" << std::endl;
        throw std::runtime_error("GapHandler::out-of-ID-order events");
    }
}

}

