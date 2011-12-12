#include "ft2Util_2/Magic7.h"
#include "ft2Util_2/Ft2.h"
#include "ft2Util_2/Livetime.h"
#include "util.h"
#include "facilities/Util.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "Configuration.h"
#include "anyoption.h"

#include <fstream>
#include <string>
#include <sstream>
#include <fitsGen/Ft2File.h>
#include <iomanip>

struct cmdline_t {
    std::string inputFT2;
    std::string outputFT2;
    std::string templateFT2;
    std::string clobber;
    int version;
    int processingVersion;
    int dt;
} cmdLine;

int parseCommandLine(int argc, char* argv[])
{
    //default values
    cmdLine.inputFT2 = "";
    cmdLine.outputFT2 = "";
//   cmdLine.templateFT2="(default)";
    cmdLine.clobber="no";
    cmdLine.version = 1;
    cmdLine.processingVersion = 1;
    cmdLine.dt = 30;

    AnyOption *opt = new AnyOption();

    //Set preferences for AnyOption
    opt->noPOSIX(); //Use only long options ("-option value" instead of "-o value/--option value")
    opt->setVerbose(); //Print warning if the user chose a unknown option
    opt->autoUsagePrint(true); //Print the help if the user chose a bad option

    //Set the usage help
    opt->addUsage("mergeFT2.exe : Merge bins from 1s FT2 file producing a 30s FT2 file, or a FT2 file with the specified binning.");
    opt->addUsage("");
    opt->addUsage("(See http://fermi.gsfc.nasa.gov/ssc/data/analysis/documentation/Cicerone/Cicerone_Data/LAT_Data_Columns.html#SpacecraftFile)");
    opt->addUsage("");
    opt->addUsage("mergeFT2.exe -inputFT2 [input 1s FT2 file] -outputFT2 [output FT2 file]");
    opt->addUsage("            -version [version int] -procVer [processing version]]");
    opt->addUsage("            [-dt [time step] -clobber ['yes' or 'no']");
    opt->addUsage("");
    opt->addUsage("Parameters:");
    opt->addUsage("");
    opt->addUsage( " -inputFT2         Input 1s FT2 file" );
    opt->addUsage( " -outputFT2        Name for the output FT2 file " );
    opt->addUsage( " -version          Version to be written in the header (integer, default=1)");
    opt->addUsage( " -procVer          Processing version to be written in the header (integer, default=1)");
    opt->addUsage( " -dt               Bin size for the output file*" );
    opt->addUsage( " -clobber          (yes|no) Overwrite an existing output? (default = no)" );
    /*  opt->addUsage( " -templateFT2      Override the default template file for the output FT2" );*/
    opt->addUsage( " -help             Prints this help " );
    opt->addUsage( "" );
    opt->addUsage( "* Note that some bins in the output file could have a different duration than 'dt', since the program will prematurely "
                   "cut bins for example if LAT_MODE or LAT_CONFIG change value, or if it encounters bins with livetime=0 in the input "
                   "FT2 file");
    opt->addUsage( "" );
    //Set the various options
    opt->setCommandFlag("help");
    opt->setCommandOption("inputFT2");
    opt->setCommandOption("outputFT2");
//   opt->setCommandOption("templateFT2");
    opt->setCommandOption("version");
    opt->setCommandOption("procVer");
    opt->setCommandOption("dt");
    opt->setCommandOption("clobber");

    //Process the command line
    opt->processCommandArgs(argc, argv);

    //Get the parameters value

    if (argc==1) {
        opt->printUsage();
        return -1;
    }

    int haveInputFT2=0,haveOutputFT2=0; //, haveTemplateFT2=0

    if ( opt->getValue( "inputFT2" ) != NULL )  {
        cmdLine.inputFT2=opt->getValue("inputFT2");
        ft2Util_2::util::checkFileExists(cmdLine.inputFT2);
        haveInputFT2=1;
    }
    if ( opt->getValue( "outputFT2" ) != NULL )  {
        cmdLine.outputFT2=opt->getValue("outputFT2");
        haveOutputFT2=1;
    }
    if ( opt->getValue( "dt" ) != NULL )  cmdLine.dt=facilities::Util::atoi(opt->getValue("dt"));
    if ( opt->getValue( "clobber" ) != NULL )  cmdLine.clobber=opt->getValue("clobber");
    if ( opt->getValue( "version" ) != NULL )  {
        cmdLine.version=facilities::Util::atoi(opt->getValue("version"));
    }
    if ( opt->getValue( "procVer" ) != NULL )  {
      cmdLine.processingVersion=facilities::Util::atoi(opt->getValue("procVer"));
    }
//   if( opt->getValue( "templateFT2" ) != NULL )  { cmdLine.templateFT2=opt->getValue("templateFT2"); ft2Util_2::util::checkFileExists(cmdLine.templateFT2); haveTemplateFT2=1;}
    if ( opt->getFlag( "help" ) ) {
        opt->printUsage();
        return -1;
    }

    //Complain if the mandatory parameters are lacking
    int sum=0;
    sum = haveInputFT2+haveOutputFT2;

    if (sum != 2)
    {
        throw std::runtime_error("FATAL: You need to specify all the mandatory parameters. Run 'mergeFT2.exe -h' for help.");
    }

    //Check that the dt parameter make sense
    if (cmdLine.dt < 0.0)
    {
        throw std::runtime_error("FATAL: The time step 'dt' cannot be negative. Run 'mergeFT2.exe -h' for help.");
    }

    //Check if the output file already exists
    if (ft2Util_2::util::FileExists(cmdLine.outputFT2)==1)
    {
        if (cmdLine.clobber!="y" && cmdLine.clobber!="true" && cmdLine.clobber !="yes")
        {
            std::stringstream str;
            str << "FATAL: File " << cmdLine.outputFT2 << " already exists, and clobber is not 'yes'. Run 'mergeFT2.exe -h' for help.";
            throw std::runtime_error(str.str());
        }
    }
    delete(opt);

    //At this point we have a valid configuration. Print it
    std::cout.precision(20);
    std::cout << std::endl;
    std::cout << "Input parameters: " << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "Input FT2 file                             : " << cmdLine.inputFT2 << std::endl;
    std::cout << "Output FT2 file                            : " << cmdLine.outputFT2 << std::endl;
    std::cout << "Overwrite an existing FT2 file             : " << cmdLine.clobber << std::endl;
    std::cout << "File version                               : " << cmdLine.version << std::endl;
    std::cout << "File processing version                    : " << cmdLine.version << std::endl;
    std::cout << "Time step for the output FT2 file          : " << cmdLine.dt << std::endl;
//   std::cout << "Template for the FT2 file                  : " << cmdLine.templateFT2 << std::endl;
    std::cout << std::endl;
    return 0;
}



int main(int argc, char* argv[])
{
    using namespace ft2Util_2;

    int crtl = parseCommandLine(argc,argv);

    if (crtl < 0) return -1;

    //Create the output FT2 file, using the input file as template:
    //this way we are sure to get two consistent files
    fitsGen::Ft2File outputFT2(cmdLine.outputFT2,1,"SC_DATA",cmdLine.inputFT2);

    //Open the input file read-only
    const tip::Table * inputFT2 = 0;
    inputFT2 = tip::IFileSvc::instance().readTable(cmdLine.inputFT2, "SC_DATA");

    //Get the column names
    std::vector<std::string> columns = outputFT2.getFieldNames();
    long int nEntries=0;
    //Loop over the entries in the input file
    double globalTstart=0, globalTstop=0;
    for (tip::Table::ConstIterator itor=inputFT2->begin(); itor != inputFT2->end(); /*Manual increment*/ )
    {
        nEntries++;
        //Copy all the values in the columns of the current entry of the input file to the current entry in the
        //output file: tstop and livetime will be overwritten later
        for (std::vector<std::string>::iterator colItor = columns.begin(); colItor != columns.end(); ++colItor)
        {
            try {
                outputFT2[*colItor] = (*itor)[*colItor];
            } catch (tip::TipException e)
            {
                std::cerr << e.what() << std::endl;
                std::cerr << "Error while reading column " << *colItor << std::endl;
                throw std::runtime_error("Input/Output problem: error reading the input file");
            }

        }
        //Get the start for this bin
        double outputStart;
        (*itor)["START"].get(outputStart);
        if (nEntries==1) {
            //This is the first iteration: save the start file, which will be used to update some keywords in the header of the
            //output file
            globalTstart = outputStart;
        }
        std::cout.precision(14);
        std::cerr.precision(14);
        std::cout << "\n* Entry number " << nEntries << ":" << std::endl;
        std::cout << "  " << std::setw(15) << outputStart << " ";
        //Now accumulate the livetime
        //(infinite loop, we'll exit with break)
        //Under normal circumstancies the following loop should accumulate "dt" bins (default=30). But, if while accumulating
        //there is a change in LAT_CONFIG,LAT_MODE,DATA_QUAL or if the livetime becomes zero (saturation or digi crash), then the loop
        //exits (so the current output bin will be < 30 s), and a new bin will start
        int prevLatConfig, prevLatMode, prevDataQual;
        double prevLivetime;
        int thisLatConfig, thisLatMode, thisDataQual;
        double thisLivetime,thisStop, thisStart;

        (*itor)["lat_config"].get(prevLatConfig);
        (*itor)["lat_mode"].get(prevLatMode);
        (*itor)["data_qual"].get(prevDataQual);
        (*itor)["livetime"].get(prevLivetime);
        double livetime=0;
        bool rewind=false;
        for (;  itor != inputFT2->end(); ++itor)
        {
            (*itor)["lat_config"].get(thisLatConfig);
            (*itor)["lat_mode"].get(thisLatMode);
            (*itor)["livetime"].get(thisLivetime);
            (*itor)["data_qual"].get(thisDataQual);
            (*itor)["stop"].get(thisStop);
            (*itor)["start"].get(thisStart);

            //Get the duration of the next bin, if any
            double nextBinStart=0,nextBinStop=0;
            double nextBinDuration =0 ;
            ++itor;
            if (itor!=inputFT2->end())
            {                
                (*itor)["start"].get(nextBinStart);
                (*itor)["stop"].get(nextBinStop);
                nextBinDuration = nextBinStop-nextBinStart;
            }
            --itor;
            //The following condition will always produces time bins <= dt (usually <=30 s)
            if (  thisStop+nextBinDuration > outputStart+cmdLine.dt )
            {
                //The current entry in the input file is the last entry to be comprised in the current bin in the output file
                rewind = false;
                livetime += thisLivetime;
                std::cout << ".";
                break;
            }
            if (thisLatConfig !=prevLatConfig)
            {
                std::cerr << "  WARNING: change in LAT configuration (from " << prevLatConfig << " to " << thisLatConfig << ") at " << thisStart << std::endl;
                //The current entry in the input file marks the beginning of a new bin, thus rewind so that the next loop will start from here
                rewind=true;
                break;
            }
            if (thisLatMode != prevLatMode)
            {
                std::cerr << "  WARNING: change in LAT mode (from " << prevLatMode << " to " << thisLatMode << ") at " << thisStart << std::endl;
                //The current entry in the input file marks the beginning of a new bin, thus rewind so that the next loop will start from here
                rewind=true;
                break;
            }
            if (thisDataQual != prevDataQual)
            {
                std::cerr << "  WARNING: change in DATA_QUAL (from " << prevDataQual << " to " << thisDataQual << ") at " << thisStart << std::endl;
                //The current entry in the input file marks the beginning of a new bin, thus rewind so that the next loop will start from here
                rewind=true;
                break;
            }
            if (thisLivetime <= 0 && prevLivetime > 0)
            {
                std::cerr << "  WARNING: livetime=0 at " << thisStart << ". Stopping there. "<< std::endl;
                //The current entry in the input file marks the beginning of a new bin, thus rewind so that the next loop will start from here
                rewind=true;
                break;
            }
            if (thisLivetime > 0 && prevLivetime <=0)
            {
                //This is the first bin without livetime=0 after bin(s) with livetime=0, we need to start a new bin in the output file
                std::cerr << "  Starting from livetime=0 at " << thisStart << std::endl;

                rewind=true;
                break;
            }
            //The current entry in the input file belongs to the current bin in the output file
            std::cout << ".";
            livetime += thisLivetime;
        }

        if (rewind)
        {
            //One of the condition determining a premature ending of the current bin has been hitted
            --itor;
            (*itor)["stop"].get(thisStop);
        }

        //Update the stop value and the livetime
        outputFT2["stop"].set( thisStop );
        std::cout << " " << thisStop << std::endl;
        outputFT2["livetime"].set( livetime );
        double livetimeFraction = 0;
        double telapse = thisStop-outputStart;
        if (telapse > 0) livetimeFraction = livetime/telapse;
        std::cout.precision(6);
        std::cout << "  Elapsed time " <<  telapse << ", Livetime = " << livetime  << " (" << livetimeFraction*100 << " %)" << std::endl;
        globalTstop = thisStop;
        //Step to the next entry in the input file, and in the output file, if there are still entries to process in the input file
        if (itor != inputFT2->end()) {
            ++itor;
            outputFT2.next();
        }
    }
    std::cout << std::endl;

    //Now update some keywords in the header of the output file
    outputFT2.setObsTimes(globalTstart, globalTstop);
    std::string pack = ft2Util_2::Configuration::Instance()->packageName+"_"+ft2Util_2::Configuration::Instance()->packageVersion;
    outputFT2.setPhduKeyword("CREATOR", pack);
    outputFT2.setPhduKeyword("VERSION", cmdLine.version);
    outputFT2.setPhduKeyword("PROC_VER", cmdLine.processingVersion);
    std::string filename(facilities::Util::basename(cmdLine.outputFT2));
    outputFT2.setPhduKeyword("FILENAME", filename);
}
