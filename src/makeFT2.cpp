#include "ft2Util/Magic7.h"
#include "ft2Util/Ft2.h"
#include "ft2Util/Livetime.h"
#include "util.h"
#include "Configuration.h"
#include "facilities/Util.h"

#include "anyoption.h"

#include <fstream>
#include <string>
#include <sstream>

#include "TTree.h"
#include "TFile.h"
#include "digiRootData/DigiEvent.h"


struct cmdline_t {
  double ft2start;
  double ft2stop;
  std::string m7file;
  std::string ft2file;
  std::string templateFT2;
  std::string clobber;
  std::string digifile;
  std::string meritfile;
  std::string gapfile;
  double step;
  int dataquality;
  int latconfig;
  int version;
  int processingVersion;
  std::string configurationFilePath;
  std::string verify;
} cmdLine;

int parseCommandLine(int argc, char* argv[])
{
  //default values
  cmdLine.ft2start = 0;
  cmdLine.ft2stop = 0;
  cmdLine.m7file = "";
  cmdLine.ft2file = "";
  cmdLine.templateFT2 = "(default)";
  cmdLine.clobber="no";
  cmdLine.digifile = "";
  cmdLine.meritfile = "";
  cmdLine.gapfile = "";
  cmdLine.step = 1.0;
  cmdLine.dataquality = 2;
  cmdLine.latconfig = 1;
  cmdLine.version = 1;
  cmdLine.processingVersion = 1;
  cmdLine.configurationFilePath="";
  cmdLine.verify = "yes";
  
  AnyOption *opt = new AnyOption();
  
  //Set preferences for AnyOption
  opt->noPOSIX(); //Use only long options ("-option value" instead of "-o value/--option value")
  opt->setVerbose(); //Print warning if the user chose a unknown option
  opt->autoUsagePrint(true); //Print the help if the user chose a bad option
  
  //Set the usage help
  opt->addUsage("makeFT2.exe : produce the FT2 file for the Fermi mission.");
  opt->addUsage("");
  opt->addUsage("(See http://fermi.gsfc.nasa.gov/ssc/data/analysis/documentation/Cicerone/Cicerone_Data/LAT_Data_Columns.html#SpacecraftFile)");
  opt->addUsage("");
  opt->addUsage( "Usage: " );
  opt->addUsage( "" );
  opt->addUsage( "-Produce the 'fake' FT2 file (without livetime):");
  opt->addUsage("");
  opt->addUsage("makeFT2.exe -ft2start [start time] -ft2stop [stop time]");
  opt->addUsage("            -m7file [Magic 7 file] -ft2file [output FT2 file]");
  opt->addUsage("            [-clobber ['yes' or 'no'] ]");
  opt->addUsage("");
  opt->addUsage( "-Produce a regular FT2 file (with livetime):");
  opt->addUsage("");
  opt->addUsage("makeFT2.exe -ft2start [start time] -ft2stop [stop time]");
  opt->addUsage("            -m7file [Magic 7 file] -ft2file [output FT2 file]");
  opt->addUsage("            -digifile [Digi file] -meritfile [Merit file]");
  opt->addUsage("            -gapfile [Digi gap file]");
  opt->addUsage("            [-clobber ['yes' or 'no'] -verify ['yes' or 'no']");
  opt->addUsage("             -dataquality [DQ flag] -latconfig [LAT config value]");
  opt->addUsage("             -templateFT2 [FT2 template] -version [version number] ]");
  opt->addUsage("             -procVer [processing version number] ]");
  opt->addUsage("");
  opt->addUsage("Parameters:");
  opt->addUsage("");
  opt->addUsage( " -ft2start         Start time for the output FT2 file (MET) " );
  opt->addUsage( " -ft2stop          Stop time for the output FT2 file (MET) " );
  opt->addUsage( " -m7file           Input Magic 7 file" );
  opt->addUsage( " -ft2file          Name for the output FT2 file" );
  opt->addUsage( " -digifile         Input Digi file" );
  opt->addUsage( " -meritfile        Input Merit file" );
  opt->addUsage( " -gapfile          Input Digi gap file" );
//   opt->addUsage( " -dt               Time step for the FT2 file (default: 1s)" );
  opt->addUsage( " -clobber          (yes|no) Overwrite an existing output? (default = no)" );
  opt->addUsage( " -dataquality      Data Quality flag to set (default = 2)");
  opt->addUsage( " -latconfig        LAT configuration status (default = 1)" );
  opt->addUsage( " -templateFT2      Override the default template file for the output FT2" );
  opt->addUsage( " -version          Version number for the file (integer,default=1)");
  opt->addUsage( " -procVer          Processing version number for the file (integer,default=1)");
  opt->addUsage( " -configFile       User-provided configuration file path");
  opt->addUsage( " -verify           (yes|no) Verify Merit and Digi for gaps (default=yes)");
  opt->addUsage( " -help             Prints this help " );
  opt->addUsage( "" );
  
  //Set the various options
  opt->setCommandFlag("help");
  opt->setCommandOption("ft2start");
  opt->setCommandOption("ft2stop");
  opt->setCommandOption("m7file");
  opt->setCommandOption("ft2file");
  opt->setCommandOption("digifile");
  opt->setCommandOption("meritfile");
  opt->setCommandOption("gapfile");
//   opt->setCommandOption("dt");
  opt->setCommandOption("clobber");
  opt->setCommandOption("dataquality");
  opt->setCommandOption("latconfig");
  opt->setCommandOption("templateFT2");
  opt->setCommandOption("version");
  opt->setCommandOption("procVer");
  opt->setCommandOption("configFile");
  opt->setCommandOption("verify");
  //Process the command line
  opt->processCommandArgs(argc, argv);
  
  //Get the parameters value
  
  if(argc==1) {
    opt->printUsage();
    return -1;
  }
  
  int haveTstart=0,haveTstop=0,haveM7file=0,haveFT2file=0, haveDigi=0,haveMerit=0,haveGaps=0, haveTemplateFT2=0;
  
  if( opt->getValue( "ft2start" ) != NULL )  {cmdLine.ft2start=facilities::Util::stringToDouble(opt->getValue("ft2start")); haveTstart=1;}
  if( opt->getValue( "ft2stop" ) != NULL )  {cmdLine.ft2stop=facilities::Util::stringToDouble(opt->getValue("ft2stop")); haveTstop=1;}
  if( opt->getValue( "m7file" ) != NULL )  { cmdLine.m7file=opt->getValue("m7file"); ft2Util::util::checkFileExists(cmdLine.m7file); haveM7file=1;} 
  if( opt->getValue( "ft2file" ) != NULL )  {cmdLine.ft2file=opt->getValue("ft2file"); haveFT2file=1;}
  if( opt->getValue( "digifile" ) != NULL )  {cmdLine.digifile=opt->getValue("digifile"); ft2Util::util::checkFileExists(cmdLine.digifile); haveDigi=1;}
  if( opt->getValue( "meritfile" ) != NULL )  {cmdLine.meritfile=opt->getValue("meritfile"); ft2Util::util::checkFileExists(cmdLine.meritfile); haveMerit=1;}
  if( opt->getValue( "gapfile" ) != NULL )  {cmdLine.gapfile=opt->getValue("gapfile"); ft2Util::util::checkFileExists(cmdLine.gapfile); haveGaps=1;}
//   if( opt->getValue( "dt" ) != NULL )  cmdLine.step=facilities::Util::stringToDouble(opt->getValue("dt"));
  if( opt->getValue( "clobber" ) != NULL )  cmdLine.clobber=opt->getValue("clobber");
  if( opt->getValue( "dataquality" ) != NULL )  cmdLine.dataquality=facilities::Util::atoi(opt->getValue("dataquality"));
  if( opt->getValue( "latconfig" ) != NULL )  {cmdLine.latconfig=facilities::Util::atoi(opt->getValue("latconfig"));}
  if( opt->getValue( "templateFT2" ) != NULL )  { cmdLine.templateFT2=opt->getValue("templateFT2"); ft2Util::util::checkFileExists(cmdLine.templateFT2); haveTemplateFT2=1;} 
  if( opt->getValue( "version" ) != NULL )  cmdLine.version=facilities::Util::atoi(opt->getValue("version"));
  if( opt->getValue( "procVer" ) != NULL )  cmdLine.processingVersion=facilities::Util::atoi(opt->getValue("procVer"));
  if( opt->getValue( "configFile" ) != NULL )  cmdLine.configurationFilePath=opt->getValue("configFile");
  if( opt->getValue( "verify" ) != NULL )  cmdLine.verify=opt->getValue("verify");
  if( opt->getFlag( "help" ) ) { opt->printUsage(); return -1; }
  
  //Complain if the mandatory parameters are lacking
  int sum=0;
  sum = haveM7file+haveFT2file;

  if(sum != 2) 
  {
    throw std::runtime_error("FATAL: You need to specify all the mandatory parameters. Run 'makeFT2.exe -h' for help.");
  }
  
  //Now check for the proper combination of parameters:
  //If we have a digifile, then we need also merit and gap files
  bool computeLivetime = false;
  sum = haveDigi+haveMerit+haveGaps;
  if(sum != 0 && sum < 3) 
  {
    throw std::runtime_error("FATAL: If you want to compute livetime, you have to provide a Digi file, a Merit file and a Digi gaps file. Run 'makeFT2.exe -h' for help.");
  } else 
  {
    if(sum!=0) computeLivetime = true;
  }
  
  //If we are computing livetime, and the user did not provide ft2start and/or ft2stop, take 
  //the beginning and/or the end of the Digi file
  bool useDigiTstart=false;
  bool useDigiTstop = false;
  if(haveTstart!=1) 
  {
    if(!computeLivetime) 
    {
      throw std::runtime_error("For the fake FT2 file you HAVE to provide ft2tstart and ft2tstop. Run 'makeFT2.exe -h' for help.");
    }
    std::cerr << "ft2tstart has not been provided, using start of the Digi file" << std::endl;
    useDigiTstart = true;
  }
  if(haveTstop!=1) 
  {
    if(!computeLivetime) 
    {
      throw std::runtime_error("For the fake FT2 file you HAVE to provide ft2start and ft2tstop. Run 'makeFT2.exe -h' for help.");
    }
    std::cerr << "ft2tstop has not been provided, using stop of the Digi file" << std::endl;
    useDigiTstop = true;
  }
  
    if (useDigiTstart || useDigiTstop)
    {
        TFile *f = new TFile(cmdLine.digifile.c_str());
        TTree *t = (TTree*) f->Get("Digi");
        DigiEvent *evt=0;
        t->SetBranchAddress("DigiEvent",&evt);
        if (useDigiTstart)
        {
            t->GetEntry(0);
            cmdLine.ft2start = evt->getTimeStamp();
        }
        if (useDigiTstop)
        {
            t->GetEntry(t->GetEntries()-1);
            cmdLine.ft2stop = evt->getTimeStamp();
        }
    }
  //Check that the dt parameter make sense
  if(cmdLine.step < 0.0) 
  {
    throw std::runtime_error("FATAL: The time step cannot be negative. Run 'makeFT2.exe -h' for help.");
  }
  
  //Check if the output file already exists
  if(ft2Util::util::FileExists(cmdLine.ft2file)==1)
  {
    if(cmdLine.clobber!="y" && cmdLine.clobber!="true" && cmdLine.clobber !="yes") 
    {
      std::stringstream str;
      str << "FATAL: File " << cmdLine.ft2file << " already exists, and clobber is not 'yes'. Run 'makeFT2.exe -h' for help.";
      throw std::runtime_error(str.str());
    }
  }
  delete(opt);
  
  //If the configuration file option is left empty, use the predefined configuration
  if(cmdLine.configurationFilePath=="") 
  {
    //Use default configuration file
    cmdLine.configurationFilePath = facilities::commonUtilities::getDataPath("ft2Util")+"/configuration.rc";
  }
  
  //At this point we have a valid configuration. Print it
  std::cout.precision(20);
  std::cout << std::endl;
  std::cout << "Input parameters: " << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "Desired start time for the FT2        :" << cmdLine.ft2start << std::endl;
  std::cout << "Desired stop time for the FT2         :" << cmdLine.ft2stop << std::endl;
  std::cout << "Magic 7 file                          :" << cmdLine.m7file << std::endl;
  std::cout << "Output FT2 file                       :" << cmdLine.ft2file << std::endl;
  if(computeLivetime) {
  std::cout << "Input Digi file                       :" << cmdLine.digifile << std::endl;
  std::cout << "Input Merit file                      :" << cmdLine.meritfile << std::endl;
  std::cout << "Input Digi Gap file                   :" << cmdLine.gapfile << std::endl;
  }
  std::cout << "Overwrite an existing FT2 file        :" << cmdLine.clobber << std::endl;
  std::cout << "Data quality flag                     :" << cmdLine.dataquality << std::endl;
  std::cout << "LAT configuration                     :" << cmdLine.latconfig << std::endl;
  std::cout << "Template for the FT2 file             :" << cmdLine.templateFT2 << std::endl;
  std::cout << "Version number for the FT2 file       :" << cmdLine.version << std::endl;
  std::cout << "Processing version for the FT2 file   :" << cmdLine.processingVersion << std::endl;
  std::cout << std::endl;
  return 0;
}



int main(int argc, char* argv[])
{
    using namespace ft2Util;
    
    facilities::commonUtilities::setupEnvironment();
    
    int crtl = parseCommandLine(argc,argv);

    if (crtl < 0) return -1;

    //Print the configuration: this will instance the singleton Configuration, if it has not been already instanced
    ft2Util::Configuration::Instance(cmdLine.configurationFilePath)->Print();
    
    if(cmdLine.verify=="no")
    {
      //Disable verification of input files
      ft2Util::Configuration::Instance()->verify="no";
      std::cerr << "\nCHECK FOR GAPS DISABLED! (you specified verify=no in the command line)" << std::endl;
    }

    //Make the FT2 file
    Ft2 ft2(cmdLine.m7file,cmdLine.ft2start, cmdLine.ft2stop, cmdLine.latconfig, cmdLine.dataquality, 
            cmdLine.digifile, cmdLine.meritfile,cmdLine.gapfile);
    //Write the file on disk
    if (cmdLine.templateFT2=="(default)")
    {
        ft2.writeFT2file(cmdLine.ft2file,cmdLine.version,cmdLine.processingVersion);
    } else
    {
        ft2.writeFT2file(cmdLine.ft2file,cmdLine.version,cmdLine.processingVersion, cmdLine.templateFT2);
    }
}
