/** @file Configuration.h
@brief declare class Configuration
*/

//Avoid to include this header more than one time
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <map>

namespace ft2Util
{
/**
*
* @brief This is a singleton which reads the configuration file and provide the configuration parameters to all the classes
* @author Giacomo Vianello <giacomov@slac.stanford.edu>
*
* $Header$
*/
class Configuration
{
public:
    static Configuration* Instance(std::string in_configurationFilePath="");
    void Print();
    std::string configurationFilePath;
    std::string packageVersion;
    std::string packageName;
    std::string packageAuthor;
    std::string packageAuthorEmail;
    double JulianDateMissionStart;
    //If two time stamps have a difference less than NULL_TIME_DIFFERENCE, they will be considered the same:
    double NULL_TIME_DIFFERENCE;
    double sweepEventsDelayEnd;
    double ticksPerCommand;
    double RollOver;
    double nominalTicksToSecond;
    double Rx;
    double Ry;
    double Rz;
    double magic7ReadPadding;
    double deadPad;
    std::string verify;
private:
  Configuration(std::string in_configurationFilePath); // Private so that it can  not be called
    ~Configuration() {};
    static Configuration* s_pInstance;
    std::map<std::string, std::string> m_parameters;
};

}
#endif
