/** @file Ft2.h
@brief declare class Ft2
*/

//Avoid to include this header more than one time
#ifndef FT2_H
#define FT2_H

#include <map>


#include "ft2Util/Status.h"
#include "ft2Util/TimeInterval.h"
#include "facilities/commonUtilities.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace ft2Util
{
/**
*@class Ft2
* @brief Represents the Ft2 file.
* @author Giacomo Vianello <giacomov@slac.stanford.edu>
* $Header$
*/
class Ft2
{
public:
    /**
    Constructor.
    @param m7File The name of the input Magic 7 file
    @param tstart The desired start time for the FT2 file
    @param tstop The desired stop time for the FT2 file
    @param latconfig The value for the LAT configuration to be used in the FT2 file
    @param dataquality The value for the Data Quality flag to be used in the FT2 file
    @param digiFile The name of the input Digi file
    @param meritFile The name of the input Merit file
    @param gapFile The name of the input DigiGAP file
    */
    Ft2 ( std::string m7File, double tstart, double tstop, int latconfig, int dataquality,
          std::string digiFile="", std::string meritFile="", std::string gapFile="");
    /**
    Write the FT2 file
    @param filename The name of the output FT2 file
    @param version The version for this file, to be written in the VERSION keyword in the header of the FT2 file
    @param ft2Template The template to be used to write the FT2 file: this template can contain all or just a subset
    of all the columns known to the software
    */
    void writeFT2file(const std::string filename,const int version=1,const int processingVersion=1,
                      const std::string ft2Template="");
    ~Ft2();

private:
    std::map<double, ft2Util::Status> m_statusMap;
    std::map<double, double> m_livetimes;
    std::map<double, int> m_insideSAA;
    std::map<double, CLHEP::Hep3Vector> m_velocityMap;
    double m_stopTime;
    void verifyDigiAndMerit(const std::string& digiFile, const std::string& meritFile, std::vector<double> &timeIntervals);
    void padTimeIntervals(double& tstart, double& tstop, std::vector<TimeInterval>& timeIntervals);
};
}
#endif
