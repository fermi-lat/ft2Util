#include "ft2Util/Ft2.h"
#include "ft2Util/Livetime.h"
#include "ft2Util/Magic7.h"
#include "ft2Util/TimeInterval.h"
#include "ft2Util/GapHandler.h"
#include "Configuration.h"
#include "fitsGen/Ft2File.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "facilities/Util.h"
#include "util.h"
#include <algorithm>

#include "TClass.h"
#include "TTree.h"
#include "TFile.h"
#include "digiRootData/DigiEvent.h"
#include <iomanip>

namespace ft2Util {

Ft2::Ft2(std::string m7File, double tstart, double tstop, int latconfig, int dataquality,
         std::string digiFile, std::string meritFile, std::string gapFile) {
  //Open and parse the Magic 7 file
  Magic7 magic7(m7File, tstart, tstop);
  //Get time intervals from magic 7 file
  std::vector<TimeInterval> timeIntervals = magic7.timeIntervals();

  //Now, if needed, let's pad the time intervals from the Magic 7 to include the requested tstart and tstop
  this->padTimeIntervals(tstart, tstop, timeIntervals);

  //Now, if the digi, merit and gap file have been provided, instance a Livetime class
  bool todoLivetime = false;
  Livetime *livetime = 0;
  if (digiFile != "" && meritFile != "" && gapFile != "") {
    livetime = new Livetime(digiFile, meritFile);
    todoLivetime = true;

    //Verifty that tstart-tstop is covered by the Digi file
    bool isCovered = livetime->isCovered(tstart, tstop);
    if (!isCovered) {
      std::string msg;
      msg =
          "FATAL: the provided Digi file does not cover the requested time interval. No livetime computation is possible ";
      msg +=
          "for time intervals not covered by the Digi file. Consider restrincting the interval, or getting more data in the Digi file.";
      throw std::runtime_error(msg);
    }

    //Check for gaps in the Digi or Merit file: this will update the vector timeIntervals with the bad time intervals (if any)
    if (Configuration::Instance()->verify != "no") {
      GapHandler gapHandler(digiFile, meritFile, gapFile, timeIntervals);
    }
  }

  //Save the stop time (we'll need it when writing the FT2 file)
  m_stopTime = tstop;

  std::cout << "Now parse the file and compute the quantities to be saved in the FT2 file:" << std::endl;
  std::cout << "---------------------------------------------------------------------------" << std::endl;

  //Fill 3 maps: the map of the status (see Status class), the map of the insideSAA flag, and the map of livetimes
  long int nIntervals = (long int) timeIntervals.size();
  long int counter = 0;
  long int outSteps = (long int) std::ceil(nIntervals / 10.0);
  for (std::vector<TimeInterval>::iterator timeIntervalItor = timeIntervals.begin();
       timeIntervalItor != timeIntervals.end();
       ++timeIntervalItor, ++counter) {
    double startTime = timeIntervalItor->start();
    double stopTime = timeIntervalItor->stop();
    //Print the percentage processed every once in a while
    if (counter % outSteps == 0) {
      std::cerr.precision(2);
      std::cerr << std::setw(5) << 100.0 * double(counter) / nIntervals << " % completed" << std::endl;
      std::cerr.precision(14);
    }
    //Instantiate a status class for the current time
    CLHEP::Hep3Vector curPosition = magic7.getPosition(startTime);
    CLHEP::Hep3Vector curVelocity = magic7.getVelocity(startTime);
    astro::Quaternion curQuaternion = magic7.getQuaternion(startTime);
    LatCondition curCondition = LatCondition(magic7.getMode(startTime), latconfig, dataquality);
    astro::EarthCoordinate curEarthCoordinate(curPosition, startTime);
    Status curStatus(curPosition,
                     curQuaternion,
                     curEarthCoordinate,
                     curCondition,
                     util::getSunPosition(startTime)
    );

    double thisLivetime = 0;
    if (todoLivetime && timeIntervalItor->isGood()) {
      thisLivetime = (*livetime)(startTime, stopTime);
    } else {
      //The fake FT2 file has no livetime computation, so livetime=0 for every entry
      //Also, if the time interval is BAD, livetime is zero
      thisLivetime = 0.0;
    }

    //Fill the three maps
    m_statusMap.insert(std::pair<double, ft2Util::Status>(startTime, curStatus));
    m_insideSAA.insert(std::pair<double, int>(startTime, magic7.getInSAA(startTime)));
    m_livetimes.insert(std::pair<double, double>(startTime, thisLivetime));
    m_velocityMap.insert(std::pair<double, CLHEP::Hep3Vector>(startTime, curVelocity));
  }
  //Summarize
  std::cout.precision(14);
  std::cout << "FT2 time bins:" << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "N. of bins                            :" << m_statusMap.size() << std::endl;
  std::cout << "Beginning (MET)                       :" << m_statusMap.begin()->first << std::endl;
  std::cout << "End (MET)                             :" << m_stopTime << std::endl;
  std::cout.precision(5);
  delete livetime;
}

Ft2::~Ft2() {
}

void Ft2::padTimeIntervals(double &tstart, double &tstop, std::vector<TimeInterval> &timeIntervals) {
  //We want to add 1s time intervals until we surpass tstart, then cut back to tstart.
  //This happens when the Magic7 file cover less time than what requested
  if (timeIntervals.front().start() > tstart) {
    //We need to add intervals to reach tstart
    int nBeforePad = timeIntervals.size();

    while (timeIntervals.front().start() - tstart > ft2Util::Configuration::Instance()->NULL_TIME_DIFFERENCE) {
      TimeInterval newTimeInterval(timeIntervals.front().start() - 1.0, timeIntervals.front().start());
      timeIntervals.insert(timeIntervals.begin(), newTimeInterval);
    }
    //Now cut if we have surpassed tstart
    if (timeIntervals.front().start() < tstart) timeIntervals.front().setStart(tstart);

    int addedBefore = timeIntervals.size() - nBeforePad;

    if (addedBefore > ft2Util::Configuration::Instance()->extrapolationLimit) {
      std::string msg;
      msg = "FATAL: the provided Magic 7 file does not cover the requested time interval.";
      msg +=
          " To cover the requested interval we would need to extrapolate position and attitude (backward) more than ";
      msg += " what permitted by the current configuration (see the parameter 'extrapolationLimit'). ";
      throw std::runtime_error(msg);
    }

    if (addedBefore > 0)
      std::cout << "INFO: Ft2::Ft2():: Added " << addedBefore
                << " entry/ies before start time of Magic 7 ORB sequence to cover"
                << " requested Tstart " << tstart << std::endl;
  }
  if (timeIntervals.back().stop() < tstop) {
    //We need to add intervals to reach tstop
    int nBeforePad = timeIntervals.size();

    while (timeIntervals.back().stop() - tstop < ft2Util::Configuration::Instance()->NULL_TIME_DIFFERENCE) {
      TimeInterval newTimeInterval(timeIntervals.back().stop(), timeIntervals.back().stop() + 1);
      timeIntervals.push_back(newTimeInterval);
    }
    //Now cut if we have surpassed tstop
    if (timeIntervals.back().stop() > tstop) timeIntervals.back().setStop(tstop);

    int addedAfter = timeIntervals.size() - nBeforePad;

    if (addedAfter > ft2Util::Configuration::Instance()->extrapolationLimit) {
      std::string msg;
      msg = "FATAL: the provided Magic 7 file does not cover the requested time interval.";
      msg += " To cover the requested interval we would need to extrapolate position and attitude (forward) more than ";
      msg += " what permitted by the current configuration (see the parameter 'extrapolationLimit'). ";
      throw std::runtime_error(msg);
    }

    if (addedAfter > 0)
      std::cout << "INFO: Ft2::Ft2():: Added " << addedAfter
                << " entry/ies after stop time of Magic 7 ORB sequence to cover"
                << " requested Tstop " << tstop << std::endl;
  }

  //Now remove the useless time intervals before and after the time covered by the Digi file (if any). This can happen
  //if the Magic7 file covers more time than what requested
  if (timeIntervals.front().start() < tstart) {
    //we need to REMOVE intervals before tstart
    int nBeforeCut = timeIntervals.size();

    //Remove every interval starting before tstart
    while (timeIntervals.front().start() < tstart) timeIntervals.erase(timeIntervals.begin());

    //Add the time interval between tstart and the beginning of timeIntervals
    if (timeIntervals.front().start() > tstart) {
      TimeInterval newTimeInterval(tstart, timeIntervals.front().start());
      timeIntervals.insert(timeIntervals.begin(), newTimeInterval);
    }
    int nCutted = std::abs((int) (timeIntervals.size() - nBeforeCut));
    if (nCutted > 0)
      std::cout << "INFO: Ft2::Ft2():: There are " << nCutted
                << " time bins before the requested start covered by the Magic7 file which are not needed" << std::endl;
  }
  if (timeIntervals.back().stop() > tstop) {
    //we need to REMOVE intervals after tstop
    int nBeforeCut = timeIntervals.size();

    //Remove every interval starting after tstop
    while (timeIntervals.back().stop() > tstop) timeIntervals.pop_back();

    //Add the time interval between the current end of timeIntervals and tstop
    if (timeIntervals.back().stop() < tstop) {
      TimeInterval newTimeInterval(timeIntervals.back().stop(), tstop);
      timeIntervals.push_back(newTimeInterval);
    }

    int nCutted = timeIntervals.size() - nBeforeCut;
    if (nCutted > 0)
      std::cout << "INFO: Ft2::Ft2():: There are " << nCutted
                << " time bins after the requested stop covered by the Magic7 file which are not needed" << std::endl;
  }

  std::cout << std::endl;

}


void Ft2::writeFT2file(const std::string filename,
                       const int version,
                       const int processingVersion,
                       const std::string ft2Template) {
  //Create and open the FT2 file
  fitsGen::Ft2File *SC_DATA = NULL;
  if (ft2Template == "") {
    //Use default template
    SC_DATA = new fitsGen::Ft2File(filename, m_statusMap.size(), "SC_DATA");
  } else {
    //Use provided template
    SC_DATA = new fitsGen::Ft2File(filename, m_statusMap.size(), "SC_DATA", ft2Template);
  }
  //Here is the list of the known columns: lowercase since the fitsGen package
  //(and the tip package used by fitsGen) return lowercase
  std::set<std::string> knownColumns;
  knownColumns.insert("start");
  knownColumns.insert("stop");
  knownColumns.insert("sc_position");
  knownColumns.insert("lat_geo");
  knownColumns.insert("lon_geo");
  knownColumns.insert("rad_geo");
  knownColumns.insert("ra_zenith");
  knownColumns.insert("dec_zenith");
  knownColumns.insert("b_mcilwain");
  knownColumns.insert("l_mcilwain");
  knownColumns.insert("geomag_lat");
  knownColumns.insert("lambda");
  knownColumns.insert("in_saa");
  knownColumns.insert("ra_scz");
  knownColumns.insert("dec_scz");
  knownColumns.insert("ra_scx");
  knownColumns.insert("dec_scx");
  knownColumns.insert("ra_npole");
  knownColumns.insert("dec_npole");
  knownColumns.insert("rock_angle");
  knownColumns.insert("lat_mode");
  knownColumns.insert("lat_config");
  knownColumns.insert("data_qual");
  knownColumns.insert("livetime");
  knownColumns.insert("qsj_1");
  knownColumns.insert("qsj_2");
  knownColumns.insert("qsj_3");
  knownColumns.insert("qsj_4");
  knownColumns.insert("ra_sun");
  knownColumns.insert("dec_sun");
  knownColumns.insert("sc_velocity");


  //now get the columns contained in the template
  std::vector<std::string> definedColumnsV = SC_DATA->getFieldNames();
  std::set<std::string> definedColumns(definedColumnsV.begin(), definedColumnsV.end());
  //Now verify what known columns are contained in the template
  std::map<std::string, bool> haveColumn;
  for (std::set<std::string>::iterator colItor = knownColumns.begin(); colItor != knownColumns.end(); ++colItor) {
    if (definedColumns.find(*colItor) != definedColumns.end()) {
      haveColumn[*colItor] = true;
    } else {
      haveColumn[*colItor] = false;
      std::cout << std::endl << "INFO: Ft2::writeFT2file(): the known column " << *colItor
                << " is not contained in the template file "
                << ft2Template << ", thus it will NOT be written in the output file" << std::endl << std::endl;
    }
  }
  //Now verify if there are columns in the template that are not known
  for (std::set<std::string>::iterator colItor = definedColumns.begin(); colItor != definedColumns.end(); ++colItor) {
    if (knownColumns.find(*colItor) == knownColumns.end()) {
      std::cerr << std::endl << "WARNING: column " << *colItor << " contained in the template file " << ft2Template
                << " is not known! Thus, it will not be filled in the output file." << std::endl << std::endl;
    }
  }

  //Write the data in the FT2 file
  std::map<double, Status>::iterator statusItor;
  tip::Table::Iterator tableItor;
  double globalTstart = 0;
  for (tableItor = SC_DATA->begin(), statusItor = m_statusMap.begin();
       statusItor != m_statusMap.end();
       ++statusItor, ++tableItor) {
    Status *curStatus = &(statusItor->second);

    //Start time
    if (haveColumn["start"]) {
      if (tableItor == SC_DATA->begin()) globalTstart = statusItor->first;
      (*tableItor)["start"].set(statusItor->first);
    }

    //Stop time
    //Get the next element, by copying the iterator and increment THE COPY
    std::map<double, Status>::iterator next = statusItor;
    ++next;
    //If the next time intervals exists, than stopTime= start time of that interval,
    //otherwise stopTime is the global stop time
    double stopTime;
    if (next != m_statusMap.end()) {
      stopTime = next->first;
    } else {
      stopTime = m_stopTime;
    }

    if (haveColumn["stop"]) {
      (*tableItor)["stop"].set(stopTime);
    }

    //Orbital position vector
    if (haveColumn["sc_position"]) {

      double curPositionArray[3] = {curStatus->spacecraft().position().x(),
                                    curStatus->spacecraft().position().y(),
                                    curStatus->spacecraft().position().z()
      };
      std::vector<double>
          curPositionVector(curPositionArray, curPositionArray + sizeof(curPositionArray) / sizeof(double));
      (*tableItor)["sc_position"].set(curPositionVector);
    }

    // Orbital velocity vector
    if (haveColumn["sc_position"]) {

      double curVelocityArray[3] = {m_velocityMap[statusItor->first].x(),
                                    m_velocityMap[statusItor->first].y(),
                                    m_velocityMap[statusItor->first].z()
      };
      std::vector<double>
          curVelocityVector(curVelocityArray, curVelocityArray + sizeof(curVelocityArray) / sizeof(double));
      (*tableItor)["sc_velocity"].set(curVelocityVector);
    }

    //Ground point coordinates
    if (haveColumn["lat_geo"]) (*tableItor)["lat_geo"].set(curStatus->spacecraft().latitude());
    if (haveColumn["lon_geo"]) (*tableItor)["lon_geo"].set(curStatus->spacecraft().longitude());
    if (haveColumn["rad_geo"]) (*tableItor)["rad_geo"].set(curStatus->spacecraft().altitude() * 1E3);

    //Zenith coordinates
    if (haveColumn["ra_zenith"]) (*tableItor)["ra_zenith"].set(curStatus->spacecraft().zenith().ra());
    if (haveColumn["dec_zenith"]) (*tableItor)["dec_zenith"].set(curStatus->spacecraft().zenith().dec());

    //McIlwain parameters
    if (haveColumn["b_mcilwain"]) (*tableItor)["b_mcilwain"].set(curStatus->spacecraft().B());
    if (haveColumn["l_mcilwain"]) (*tableItor)["l_mcilwain"].set(curStatus->spacecraft().L());

    //Geomagnetic latitude
    if (haveColumn["geomag_lat"]) (*tableItor)["geomag_lat"].set(curStatus->spacecraft().geolat());

    //Lambda parameter
    if (haveColumn["lambda"]) (*tableItor)["lambda"].set(curStatus->spacecraft().lambda());

    //in SAA?
    //Here we do not use the spacecraft.insideSAA() method of the Status class
    //because the SAA poligon used by it is sligthly different from the one used onboard
    //and here we want to reflect the onboard situation
    if (haveColumn["in_saa"]) (*tableItor)["in_saa"].set((bool) m_insideSAA[statusItor->first]);

    //RA and DEC of spacecraft Z axis
    if (haveColumn["ra_scz"]) (*tableItor)["ra_scz"].set(curStatus->spacecraft().zAxis().ra());
    if (haveColumn["dec_scz"]) (*tableItor)["dec_scz"].set(curStatus->spacecraft().zAxis().dec());

    //RA and DEC of spacecraft X axis
    if (haveColumn["ra_scx"]) (*tableItor)["ra_scx"].set(curStatus->spacecraft().xAxis().ra());
    if (haveColumn["dec_scx"]) (*tableItor)["dec_scx"].set(curStatus->spacecraft().xAxis().dec());

    //RA and DEC of the orbital pole
    double raPole = 0, decPole = 0;
    std::map<double, Status>::iterator nextStatusItor = statusItor;
    ++nextStatusItor;
    if (nextStatusItor == m_statusMap.end()) {
      //No more status after the current one, so simply copy the orbital pole from
      //the previous one
      tip::Table::Iterator prevTableItor = tableItor;
      --prevTableItor;
      if (haveColumn["ra_npole"]) (*prevTableItor)["ra_npole"].get(raPole);
      if (haveColumn["dec_npole"]) (*prevTableItor)["dec_npole"].get(decPole);
    } else {
      //Compute the cross product between the two position vectors
      CLHEP::Hep3Vector dir1 = curStatus->spacecraft().position();
      CLHEP::Hep3Vector dir2 = nextStatusItor->second.spacecraft().position();
      astro::SkyDir pole(dir1.cross(dir2));
      raPole = pole.ra();
      decPole = pole.dec();
    }
    if (haveColumn["ra_npole"]) (*tableItor)["ra_npole"].set(raPole);
    if (haveColumn["dec_npole"]) (*tableItor)["dec_npole"].set(decPole);

    //Sun position
    if (haveColumn["ra_sun"]) (*tableItor)["ra_sun"].set(curStatus->sun().ra());
    if (haveColumn["dec_sun"]) (*tableItor)["dec_sun"].set(curStatus->sun().dec());

    //Rocking angle
    if (haveColumn["rock_angle"]) (*tableItor)["rock_angle"].set(curStatus->spacecraft().rockingAngle());

    //LAT mode
    if (haveColumn["lat_mode"]) (*tableItor)["lat_mode"].set(curStatus->lat().mode());

    //LAT configuration
    if (haveColumn["lat_config"]) (*tableItor)["lat_config"].set(curStatus->lat().config());

    //Data quality flag
    if (haveColumn["data_qual"]) (*tableItor)["data_qual"].set(curStatus->lat().dataQuality());

    //Livetime
    if (haveColumn["livetime"]) (*tableItor)["livetime"].set(m_livetimes[statusItor->first]);

    //Attitude quaternion
    if (haveColumn["qsj_1"]) (*tableItor)["qsj_1"].set(curStatus->spacecraft().quaternion().vector().x());
    if (haveColumn["qsj_2"]) (*tableItor)["qsj_2"].set(curStatus->spacecraft().quaternion().vector().y());
    if (haveColumn["qsj_3"]) (*tableItor)["qsj_3"].set(curStatus->spacecraft().quaternion().vector().z());
    if (haveColumn["qsj_4"]) (*tableItor)["qsj_4"].set(curStatus->spacecraft().quaternion().scalar());
  }

  //Update the header
  SC_DATA->setObsTimes(globalTstart, m_stopTime);
  std::string
      pack = ft2Util::Configuration::Instance()->packageName + "_" + ft2Util::Configuration::Instance()->packageVersion;
  SC_DATA->setPhduKeyword("CREATOR", pack);
  SC_DATA->setPhduKeyword("VERSION", version);
  SC_DATA->setPhduKeyword("PROC_VER", processingVersion);
  std::string filenameBase(facilities::Util::basename(filename));
  SC_DATA->setPhduKeyword("FILENAME", filenameBase);
  //This actually write everything to disk
  delete SC_DATA;

}
}
