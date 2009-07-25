// c++/stl headers
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Headers from this package
#include "ft2Util/FT2_Time.hpp"


#include "facilities/Timestamp.h"
#include "facilities/Util.h"

#include "astro/EarthCoordinate.h"

#include "fitsGen/Ft2File.h"

//#include "st_stream/StreamFormatter.h"
//#include "st_app/AppParGroup.h"
//#include "st_app/StApp.h"
//#include "st_app/StAppFactory.h"

void FT2::WriteFitsFile(FT2 &FT2) {


    //using namespace fitsGen;
    unsigned int START, STOP;
    FT2.Get_FT2_Entry_Index(FT2, FT2.DT.RunStart, START);
    FT2.Get_FT2_Entry_Index(FT2, FT2.DT.RunStop, STOP);
    printf("START=%d STOP=%d\n", START, STOP);

    //unsigned long FT2_ENTR=FT2.Get_FT2_Entries(FT2);
    unsigned long FT2_ENTR = STOP - START - 1;


    std::cout << "Start Instance of Ft2File\n";
    std::cout << "The file name is " << FT2.FT2_fits_File << "\n";
    std::cout << "The file lenght is " << FT2_ENTR << "\n";


    //fitsGen::Ft2File ft2(FT2.FT2_fits_File, FT2_ENTR);
    //if (FT2.new_tpl == true) {
    if (new_tpl == false) {
        std::cout << "You are using the default ft2 template " << FT2.path << "\n";
    } else {
        std::cout << "You are using a new ft2 template " << FT2.path << "\n";
    }
    fitsGen::Ft2File ft2(FT2.FT2_fits_File, FT2_ENTR, "SC_DATA", FT2.path);
    //}


    std::cout << "Instanced\n";

    //!!!!!!!!!!!!!!Verify this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //ft2.header().addHistory("Input pointing history file: " + pointingFile);

    //std::ifstream d2(pointingFile.c_str());
    //std::string line;
    //std::vector<std::string> dataFields;
    //std::cout<<"SC vector\n";
    std::vector<float> scPosition(3);
    //std::cout<<"done\n";



    std::cout << "Loop over ft2 fields and test ft2.tpl\n";


    for (unsigned int i = START; i < STOP; i++) {
        if (FT2.FT2_T.Tstart[i] < FT2.FT2_T.Tstop[i]) {
            ft2["start"].set(FT2.FT2_T.Tstart[i]);
            ft2["stop"].set(FT2.FT2_T.Tstop[i]);
            scPosition[0] = FT2.ORB.x[i];
            scPosition[1] = FT2.ORB.y[i];
            scPosition[2] = FT2.ORB.z[i];
            ft2["sc_position"].set(scPosition);
            ft2["ra_scz"].set(FT2.FT2_SC.RA_SCZ[i]);
            ft2["dec_scz"].set(FT2.FT2_SC.DEC_SCZ[i]);
            ft2["ra_scx"].set(FT2.FT2_SC.RA_SCX[i]);
            ft2["dec_scx"].set(FT2.FT2_SC.DEC_SCX[i]);
            ft2["ra_zenith"].set(FT2.FT2_SC.RA_ZENITH[i]);
            ft2["dec_zenith"].set(FT2.FT2_SC.DEC_ZENITH[i]);
            ft2["lon_geo"].set(FT2.FT2_SC.LON_GEO[i]);
            ft2["lat_geo"].set(FT2.FT2_SC.LAT_GEO[i]);
            ft2["rad_geo"].set(FT2.FT2_SC.RAD_GEO[i]);
            ft2["geomag_lat"].set(FT2.FT2_SC.GEOMAG_LAT[i]);
            ft2["b_mcilwain"].set(FT2.FT2_SC.B_MCILWAIN[i]);
            ft2["l_mcilwain"].set(FT2.FT2_SC.L_MCILWAIN[i]);
            if (fabs(FT2.FT2_T.LiveTime[i]) < FT2.LiveTimeTolerance) {
                FT2.FT2_T.LiveTime[i] = 0;
            }
            ft2["livetime"].set(FT2.FT2_T.LiveTime[i]);
            ft2["lat_mode"].set(FT2.ORB.CM[i]);
            if (FT2.new_tpl == true) {
                ft2["rock_angle"].set(FT2.FT2_SC.ROCKING_ANGLE[i]);
                ft2["ra_npole"].set(FT2.FT2_SC.ORBITAL_POLE_RA[i]);
                ft2["dec_npole"].set(FT2.FT2_SC.ORBITAL_POLE_DEC[i]);
                ft2["lat_config"].set(FT2.LAT_CONFIG);
                ft2["data_qual"].set(FT2.DATA_QUAL);
            }
            ft2["qsj_1"].set(FT2.FT2_SC.QS_J1[i]);
            ft2["qsj_2"].set(FT2.FT2_SC.QS_J2[i]);
            ft2["qsj_3"].set(FT2.FT2_SC.QS_J3[i]);
            ft2["qsj_4"].set(FT2.FT2_SC.QS_J4[i]);
            if (FT2.ORB.SAA[i]) {
                ft2["in_saa"].set(true);
            } else {
                ft2["in_saa"].set(false);
            }
            ft2.next();
        } else if ((FT2.FT2_T.Tstart[i] == FT2.FT2_T.Tstop[i]) && (i == STOP - 1) && (FT2.FT2_T.LiveTime[i] == 0) ){
            printf("Entry %d skipped, Tstart==Tstop, this happened in the last entry, with zero livetime, probable rounding problem\n", i);
        } else if ((FT2.FT2_T.Tstart[i] == FT2.FT2_T.Tstop[i]) && (i < STOP - 1)) {
            printf("Entry %d skipped, Tstart==Tstop, this did not  happened in the last entry !!!!!!!!!  possible problem\n", i);
        } else if ((FT2.FT2_T.Tstart[i] == FT2.FT2_T.Tstop[i]) && (FT2.FT2_T.LiveTime[i] != 0)) {
            printf("Entry %d skipped, Tstart==Tstop, this happened with non-zero livetime !!!!!!!!!! possible problem\n", i);
        }
    }

    ft2.setObsTimes(FT2.FT2_T.Tstart[START], FT2.FT2_T.Tstop[STOP]);
    std::ostringstream creator;
    creator << "ft2Util";
    ft2.setPhduKeyword("CREATOR", creator.str());
    //std::string version = m_pars["file_version"];
    //std::string version = FT2.VERSION.srt();
    ft2.setPhduKeyword("VERSION", FT2.Version);
    std::string filename(facilities::Util::basename(FT2.FT2_fits_File));
    ft2.setPhduKeyword("FILENAME", filename);
    std::cout << "done\n";
}

void FT2::WriteFitsFileMerged(FT2 &FT2) {


    //using namespace fitsGen;

    unsigned long FT2_ENTR = FT2.Get_FT2_Entries(FT2);

    std::cout << "Start Instance of Ft2File\n";
    std::cout << "The file name is " << FT2.FT2_fits_File << "\n";
    std::cout << "The file lenght is " << FT2_ENTR << "\n";

    if (new_tpl == false) {
        std::cout << "You are using the default ft2 template " << FT2.path << "\n";
    } else {
        std::cout << "You are using a new ft2 template " << FT2.path << "\n";
    }
    fitsGen::Ft2File ft2(FT2.FT2_fits_File, FT2_ENTR, "SC_DATA", FT2.path);

    std::cout << "Instanced\n";

    //!!!!!!!!!!!!!!Verify this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //ft2.header().addHistory("Input pointing history file: " + pointingFile);

    //std::ifstream d2(pointingFile.c_str());
    //std::string line;
    //std::vector<std::string> dataFields;
    std::cout << "SC vector\n";
    std::vector<float> scPosition(3);
    std::cout << "done\n";



    std::cout << "Loop over fields and test ft2.tlp\n";
    for (unsigned int i = 0; i < FT2_ENTR; i++) {
        ft2["start"].set(FT2.FT2_T.Tstart[i]);
        ft2["stop"].set(FT2.FT2_T.Tstop[i]);
        scPosition[0] = FT2.ORB.x[i];
        scPosition[1] = FT2.ORB.y[i];
        scPosition[2] = FT2.ORB.z[i];
        ft2["sc_position"].set(scPosition);
        ft2["ra_scz"].set(FT2.FT2_SC.RA_SCZ[i]);
        ft2["dec_scz"].set(FT2.FT2_SC.DEC_SCZ[i]);
        ft2["ra_scx"].set(FT2.FT2_SC.RA_SCX[i]);
        ft2["dec_scx"].set(FT2.FT2_SC.DEC_SCX[i]);
        ft2["ra_zenith"].set(FT2.FT2_SC.RA_ZENITH[i]);
        ft2["dec_zenith"].set(FT2.FT2_SC.DEC_ZENITH[i]);
        ft2["lon_geo"].set(FT2.FT2_SC.LON_GEO[i]);
        ft2["lat_geo"].set(FT2.FT2_SC.LAT_GEO[i]);
        ft2["rad_geo"].set(FT2.FT2_SC.RAD_GEO[i]);
        ft2["geomag_lat"].set(FT2.FT2_SC.GEOMAG_LAT[i]);
        ft2["b_mcilwain"].set(FT2.FT2_SC.B_MCILWAIN[i]);
        ft2["l_mcilwain"].set(FT2.FT2_SC.L_MCILWAIN[i]);
        if (fabs(FT2.FT2_T.LiveTime[i]) < FT2.LiveTimeTolerance) {
            FT2.FT2_T.LiveTime[i] = 0;
        }
        ft2["livetime"].set(FT2.FT2_T.LiveTime[i]);
        ft2["lat_mode"].set(FT2.ORB.CM[i]);
        if (FT2.new_tpl == true) {
            ft2["rock_angle"].set(FT2.FT2_SC.ROCKING_ANGLE[i]);
            ft2["ra_npole"].set(FT2.FT2_SC.ORBITAL_POLE_RA[i]);
            ft2["dec_npole"].set(FT2.FT2_SC.ORBITAL_POLE_DEC[i]);
            ft2["lat_config"].set(FT2.LAT_CONFIG);
            ft2["data_qual"].set(FT2.DATA_QUAL);
        }
        ft2["qsj_1"].set(FT2.FT2_SC.QS_J1[i]);
        ft2["qsj_2"].set(FT2.FT2_SC.QS_J2[i]);
        ft2["qsj_3"].set(FT2.FT2_SC.QS_J3[i]);
        ft2["qsj_4"].set(FT2.FT2_SC.QS_J4[i]);

        if (FT2.ORB.SAA[i]) {
            ft2["in_saa"].set(true);
        } else {
            ft2["in_saa"].set(false);
        }

        ft2.next();
    }

    ft2.setObsTimes(FT2.FT2_T.Tstart[0], FT2.FT2_T.Tstop[FT2_ENTR - 1]);
    std::ostringstream creator;
    creator << "ft2Util";
    ft2.setPhduKeyword("CREATOR", creator.str());
    //std::string version = m_pars["file_version"];
    //std::string version = FT2.VERSION.srt();
    ft2.setPhduKeyword("VERSION", FT2.Version);
    std::string filename(facilities::Util::basename(FT2.FT2_fits_File));
    ft2.setPhduKeyword("FILENAME", filename);
    std::cout << "done\n";
}
