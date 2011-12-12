#include "Configuration.h"
#include "facilities/commonUtilities.h"
#include "facilities/Util.h"
#include "util.h"
#include <fstream>

namespace ft2Util_2 {

/** This function is called to create an instance of the class.
    Calling the constructor publicly is not allowed. The constructor
    is private and is only called by this Instance function.
*/

Configuration* Configuration::s_pInstance=NULL;

Configuration* Configuration::Instance(std::string in_configurationFilePath)
{
    if (NULL == s_pInstance)   // Only allow one instance of class to be generated.
    {
      s_pInstance = new Configuration(in_configurationFilePath);
    }

    return s_pInstance;
}

Configuration::Configuration(std::string in_configurationFilePath)
{
  //I don't know how to get these information... it should be a #define when compiling, or something like that...
  packageName = "ft2Util_2";
  
  if (in_configurationFilePath=="")
  {
    //Safety net: default configuration file
    configurationFilePath = facilities::commonUtilities::getDataPath(packageName)+"/configuration.rc";
  } else
  {
    configurationFilePath = in_configurationFilePath;
  }
  
  //Let's read the configuration file: it has a structure like:
  // parameter = value
  std::ifstream configurationFile;
  configurationFile.open(configurationFilePath.c_str(), std::ifstream::in);
  if (!configurationFile.is_open()) {
    std::cerr << "FATAL ERROR:  Unable to open configuration file :  " << configurationFilePath << std::endl;
    throw std::runtime_error("Configuration(): Unable to open configuration file");
  }
  int linePos=0;
  //Infinite loop, will exit with break
  while (true) {
    linePos++;
    std::string line;
    std::getline(configurationFile, line);
    
    if (configurationFile.eof())
    {
      //End of file
      break;
    }
    //Continue if this line is a comment
    if(line.size() <=1) continue;
    if(line.substr(0,1)=="#") continue;
    //Tokenize the line
    std::vector<std::string> tokens;
    facilities::Util::stringTokenize(line, "=",  tokens);
    //Save the parameter while removing all white spaces
    std::string key = tokens.at(0);
    std::string value = tokens.at(1);
    facilities::Util::trim(&key);
    facilities::Util::trim(&value);
    //If a line was only of spaces, continue
    if(key.size() <= 1) continue;
    m_parameters[key] = value;
  }
  configurationFile.close();  
  
  //Now fill the constants
  packageVersion = m_parameters["packageVersion"];
  packageAuthor = m_parameters["packageAuthor"];
  packageAuthorEmail = m_parameters["packageAuthorEmail"];
  packageName = m_parameters["packageName"];
  JulianDateMissionStart = facilities::Util::stringToDouble(m_parameters["JulianDateMissionStart"]);
  NULL_TIME_DIFFERENCE = facilities::Util::stringToDouble(m_parameters["NULL_TIME_DIFFERENCE"]);   
  sweepEventsDelayEnd = facilities::Util::stringToDouble(m_parameters["sweepEventsDelayEnd"]);
  ticksPerCommand = facilities::Util::stringToDouble(m_parameters["ticksPerCommand"]);   
  RollOver = facilities::Util::stringToDouble(m_parameters["RollOver"]);
  nominalTicksToSecond = facilities::Util::stringToDouble(m_parameters["nominalTicksToSecond"]); 
  Rx = facilities::Util::stringToDouble(m_parameters["Rx"]);
  Ry = facilities::Util::stringToDouble(m_parameters["Ry"]);
  Rz = facilities::Util::stringToDouble(m_parameters["Rz"]);
  magic7ReadPadding = facilities::Util::stringToDouble(m_parameters["magic7ReadPadding"]);
  verify = m_parameters["verify"];
  deadPad = facilities::Util::stringToDouble(m_parameters["deadPad"]);
}

void Configuration::Print() 
{
  std::cout.precision(14);
  std::cout << std::endl << "Configuration: " << std::endl;
  std::cout << "(from file " << configurationFilePath << ")" << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "packageName                           :" << packageName<< std::endl;
  std::cout << "packageVersion                        :" << packageVersion<< std::endl;
  std::cout << "packageAuthor                         :" << packageAuthor<< std::endl;
  std::cout << "packageAuthorEmail                    :" << packageAuthorEmail<< std::endl;
  std::cout << "JulianDateMissionStart                :" << JulianDateMissionStart<< std::endl;
  std::cout << "NULL_TIME_DIFFERENCE                  :" << NULL_TIME_DIFFERENCE << std::endl;
  std::cout << "sweepEventsDelayEnd                   :" << sweepEventsDelayEnd << std::endl;
  std::cout << "ticksPerCommand                       :" << ticksPerCommand << std::endl;
  std::cout << "RollOver                              :" << RollOver << std::endl;
  std::cout << "nominalTicksToSecond                  :" << nominalTicksToSecond << std::endl;
  std::cout << "Rx                                    :" << Rx << std::endl;
  std::cout << "Ry                                    :" << Ry<< std::endl;
  std::cout << "Rz                                    :" << Rz << std::endl;
  std::cout << "magic7ReadPadding                     :" << magic7ReadPadding << std::endl;
  std::cout << "verify                                :" << verify << std::endl;
  std::cout << "deadPad                               :" << deadPad  << std::endl;
}

}
