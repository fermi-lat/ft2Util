#include "TTree.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TEntryList.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iostream>

void selectDigiEvents(double met1=327356689.88581, double met2=327356838.64326,
                      const char* input="r0327356667_v001_digi.root", 
                      const char* output="r0327356667_digi_gaps.root") {   
   //Get old file, old tree and set top branch address
   TFile *oldfile = new TFile(input, "READ");
   TTree *oldtree = (TTree*)oldfile->Get("Digi");   
   
   //Disable all branches and reenable only the needed ones
   oldtree->SetBranchStatus("*", 0);
   oldtree->SetBranchStatus("m_eventId", 1);
   oldtree->SetBranchStatus("m_runId", 1);
   oldtree->SetBranchStatus("m_liveTime", 1);
   oldtree->SetBranchStatus("DigiEvent", 1);
   oldtree->SetBranchStatus("m_timeStamp", 1);
   oldtree->SetBranchStatus("m_metaEvent", 1);
   oldtree->SetBranchStatus("m_gem", 1);

   DigiEvent *evt=0;
   oldtree->SetBranchAddress("DigiEvent", &evt);
   Int_t nentries = (Int_t)oldtree->GetEntries();
   
   //Create a new file + a clone of old tree in new file
   TFile *newfile = new TFile(output,"recreate");
   TTree *newtree = oldtree->CloneTree(0);
   int nSelected=0;
   
   //Get the offset for the livetime counter
   oldtree->GetEntry(0);
   ULong64_t livetimeOffset = evt->getMetaEvent().scalers().livetime();
   //Get the offset for the elapsed time counter
   ULong64_t elapsedOffset = evt->getMetaEvent().scalers().elapsed();
   
   ULong64_t thisDelta, zeroDeadtime;
   double timeStamp;
   
   for (Int_t i=0;i<nentries; i++) {
      oldtree->GetEntry(i);
      //newtree->Fill();
      timeStamp = evt->getTimeStamp();
      if (timeStamp >= met1 && timeStamp <= met2 ) {
        std::cerr << ".";
        //Set the livetime to the elapsed time (so that every bin in the FT2 file will have LIVETIME=1)
        //thisDelta = evt->getMetaEvent().scalers().elapsed()-elapsedOffset;
        //evt->getMetaEvent().scalers().setLivetime((thisDelta)+livetimeOffset);
        evt->getMetaEvent().scalers().setLivetime(evt->getMetaEvent().scalers().elapsed());
        
        //Deliberately create a hole 3.3 seconds long
        if(timeStamp>=met1+81.3 && timeStamp<=met1+84.6) continue;      
        
        newtree->Fill();
        nSelected++;
        }
      if(timeStamp >= met2) break;
   }
   std::cerr << std::endl;
   std::cerr << "Selected " << nSelected << " events out of " << nentries << std::endl;
   newtree->Print();
   newtree->AutoSave();
   delete oldfile;
   delete newfile;
}

