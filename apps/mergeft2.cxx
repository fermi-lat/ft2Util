// stl/c++ headers
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


// headers for this package
#include "ft2Util/FT2_Time.hpp"
#include "ft2Util/MergeFT2.hpp"
#include "fitsGen/Ft2File.h"
#include "fitsGen/MeritFile.h"
#include "facilities/Util.h"
#include "facilities/Timestamp.h"

void getFileNames_TXT(int iargc, char * argv[], std::string & FT2File,
        std::string & fitsFile, std::string & Version, FT2 &FT2) {

    // std::cout<<"command line"<<std::endl;
    for (int i = 1; i < iargc; i++) {
        std::string par = argv[i];
        if (par == "-FT2_txt_File") FT2File = std::string(argv[i + 1]);
        if (par == "-FT2_fits_File") fitsFile = std::string(argv[i + 1]);
        if (par == "-Version") Version = std::string(argv[i + 1]);
        if (par == "-FT2_MERGED_BIN_WIDTH") {
            FT2.FT2_MERGED_BIN_WIDTH = atof(std::string(argv[i + 1]).c_str());
            printf("FT2_MERGED_BIN_WIDTH =%e\n", FT2.FT2_MERGED_BIN_WIDTH);
        }
        if (par == "-h") {
            std::cout << usage;
            std::exit(0);
        }
        if (par == "--help") {
            std::cout << usage;
            std::exit(0);
        }
    }

    std::cout << "FT2 FILE"
            << FT2File
            << " FITS FILE"
            << fitsFile
            << "Version of file "
            << Version
            << std::endl;
}

void getFileNames_FITS(int iargc, char * argv[], std::string & ft2,
        std::string & ft2_merged, std::string & Version, double & FT2_MERGED_BIN_WIDTH) {

    // std::cout<<"command line"<<std::endl;
    for (int i = 1; i < iargc; i++) {
        std::string par = argv[i];
        if (par == "-FT2_fits_File") ft2 = std::string(argv[i + 1]);
        if (par == "-FT2_fits_merged_File") ft2_merged = std::string(argv[i + 1]);
        if (par == "-Version") Version = std::string(argv[i + 1]);
        if (par == "-FT2_MERGED_BIN_WIDTH") {
            FT2_MERGED_BIN_WIDTH = atof(std::string(argv[i + 1]).c_str());
            printf("FT2_MERGED_BIN_WIDTH =%e\n", FT2_MERGED_BIN_WIDTH);
        }
        if (par == "-h") {
            std::cout << usage;
            std::exit(0);
        }
        if (par == "--help") {
            std::cout << usage;
            std::exit(0);
        }
    }

    std::cout << "ft2 fits file is "
            << ft2
            << "\n"
            << "ft2 fits merged file is "
            << ft2_merged
            << "\n"
            << "Version of file is "
            << Version
            << std::endl;
}

int main(int iargc, char **argv) {
    printf("v1r2p26-test\n");

    bool new_tpl(false);
    std::string path = "/afs/slac.stanford.edu/u/gl/tramacer/fitsGen/ScienceTools-LATEST-1-2481/data/ft2.tpl";
    if (iargc < 5) {
        std::cout << usage;
        std::exit(0);
    }

    for (int i = 1; i < iargc; i++) {
        std::string par = argv[i];

        if (par == "-new_tpl") {
            new_tpl = true;
            path = std::string(argv[i + 1]);
            std::cout << "You are using an alternative ft2 template " << path << "\n";
        }
    }

    for (int i = 1; i < iargc; i++) {
        std::string par = argv[i];
        if (par == "-FT2_fits_File") {
            FITS(iargc, argv, new_tpl, path);
        } else if (par == "-FT2_txt_File") {
            TXT(iargc, argv, new_tpl, path);
        }
    }

}

void TXT(int iargc, char **argv, bool new_tpl, std::string path) {
    try {
        printf("Merging from txt  file: obsolete warning!!\n");

        FT2 FT2;



        //File Handlign
        std::string FT2File;
        std::string fitsFile;
        std::string line, comment;
        std::string Version;

        double time, t_stop, time_elapsed(0), live_time(0);
        unsigned int Entries(0);
        bool first_bin(true), new_entry(false);
        bool in_saa;

        getFileNames_TXT(iargc, argv, FT2File, fitsFile, Version, FT2);

        std::ifstream inFT2File(FT2File.c_str());

        FT2.Version = Version;

        //Read FT2 File
        while (std::getline(inFT2File, line, '\n')) {



            //A SIMPLE TOKENIZER
            std::vector<std::string> tokens; // Create vector to hold our words
            std::string delimiters = ",";
            // skip delimiters at beginning.
            std::string::size_type lastPos = line.find_first_not_of(delimiters, 0);
            // find first "non-delimiter".
            std::string::size_type pos = line.find_first_of(delimiters, lastPos);
            while (std::string::npos != pos || std::string::npos != lastPos) {
                // found a token, add it to the vector.
                tokens.push_back(line.substr(lastPos, pos - lastPos));

                // skip delimiters.  Note the "not_of"
                lastPos = line.find_first_not_of(delimiters, pos);

                // find next "non-delimiter"
                pos = line.find_first_of(delimiters, lastPos);
            }


            //SKIP lines that start with # character
            comment = line.substr(0, 1);
            if (comment.find("#", 0) == std::string::npos) {

                if (first_bin || new_entry) {

                    first_bin = false;
                    new_entry = false;

                    Entries++;

                    printf("new entries, entr=%d\n", Entries);
                    //--------------------!!!!!!! Very Important ------------------
                    //Fills arrayes from FT2txt file
                    //!!!Attention the fields MUST be put in as
                    //expected from the WriteFitsFile Method
                    //This implies that SC.x SC.y and SC.z must
                    //converted to the ORB.x ect.. format i.e. im Km
                    //and The in_saa entry MUST be coverted
                    //to ORB.SAA with 0 false and 1 true
                    //--------------------------------------------------------------

                    //update arrays size
                    FT2.FT2_T.Set_FT2Time_Size(FT2.FT2_T, Entries);
                    FT2.Update_FT2_Entries(FT2, Entries);
                    FT2.ORB.Set_ORB_Size(FT2.ORB, Entries);
                    FT2.FT2_SC.Set_FT2SC_Size(FT2.FT2_SC, Entries);

                    printf("arrays resized\n");


                    //store values
                    FT2.FT2_T.Tstart[Entries - 1] = atof(tokens[1].c_str());
                    std::cout << "Tstart " << tokens[1] << "\n";
                    std::cout << "Tstop " << tokens[2] << "\n";
                    FT2.ORB.x[Entries - 1] = atof(tokens[3].c_str());
                    std::cout << tokens[3] << " ";
                    FT2.ORB.y[Entries - 1] = atof(tokens[4].c_str());
                    std::cout << tokens[4] << " ";
                    FT2.ORB.z[Entries - 1] = atof(tokens[5].c_str());
                    std::cout << tokens[5] << " ";
                    FT2.FT2_SC.RA_SCZ[Entries - 1] = atof(tokens[6].c_str());
                    std::cout << tokens[6] << " ";
                    FT2.FT2_SC.DEC_SCZ[Entries - 1] = atof(tokens[7].c_str());
                    std::cout << tokens[7] << " ";
                    FT2.FT2_SC.RA_SCX[Entries - 1] = atof(tokens[8].c_str());
                    std::cout << tokens[8] << " ";
                    FT2.FT2_SC.DEC_SCX[Entries - 1] = atof(tokens[9].c_str());
                    std::cout << tokens[9] << " ";
                    FT2.FT2_SC.RA_ZENITH[Entries - 1] = atof(tokens[10].c_str());
                    std::cout << tokens[10] << " ";
                    FT2.FT2_SC.DEC_ZENITH[Entries - 1] = atof(tokens[11].c_str());
                    std::cout << tokens[11] << std::endl;
                    FT2.FT2_SC.LON_GEO[Entries - 1] = atof(tokens[12].c_str());
                    FT2.FT2_SC.LAT_GEO[Entries - 1] = atof(tokens[13].c_str());
                    FT2.FT2_SC.RAD_GEO[Entries - 1] = atof(tokens[14].c_str());
                    FT2.FT2_SC.GEOMAG_LAT[Entries - 1] = atof(tokens[15].c_str());
                    if (tokens[16] == "true") {
                        FT2.ORB.SAA[Entries - 1] == 1;
                    } else {
                        FT2.ORB.SAA[Entries - 1] == 0;
                    }
                    FT2.FT2_SC.B_MCILWAIN[Entries - 1] = atof(tokens[17].c_str());
                    FT2.FT2_SC.L_MCILWAIN[Entries - 1] = atof(tokens[18].c_str());
                    FT2.ORB.CM[Entries - 1] = atoi(tokens[19].c_str());
                    FT2.FT2_SC.QS_J1[Entries - 1] = atof(tokens[21].c_str());
                    FT2.FT2_SC.QS_J2[Entries - 1] = atof(tokens[22].c_str());
                    FT2.FT2_SC.QS_J3[Entries - 1] = atof(tokens[23].c_str());
                    FT2.FT2_SC.QS_J4[Entries - 1] = atof(tokens[24].c_str());
                }
                //std::cout<<"tokens"
                //	 <<tokens[18]
                //	 <<" "
                //	 <<tokens[19]
                //		 <<" "
                //	 <<tokens[20]
                //	 <<std::endl;
                live_time += atof(tokens[20].c_str());
                //printf("live time=%e\n",live_time);
                time_elapsed += (atof(tokens[2].c_str()) - atof(tokens[1].c_str()));

                //  printf("time elapsed=%e\n",time_elapsed);

                if (time_elapsed >= FT2.FT2_MERGED_BIN_WIDTH) {
                    FT2.FT2_T.Tstop[Entries - 1] = atof(tokens[2].c_str());
                    //store t_stop
                    //store live_time
                    //add one element to the arrays
                    FT2.FT2_T.LiveTime[Entries - 1] = live_time;
                    new_entry = true;
                    live_time = 0;
                    time_elapsed = 0;
                }

                t_stop = atof(tokens[2].c_str());
            }
        }
        if (new_entry == false) {
            FT2.FT2_T.Tstop[Entries - 1] = t_stop;
            //store t_stop
            //store live_time
            //add en element to the arrays
            FT2.FT2_T.LiveTime[Entries - 1] = live_time;
            new_entry = true;
            live_time = 0;
            time_elapsed = 0;
        }

        inFT2File.close();


        //Write Fits File
        FT2.FT2_fits_File = fitsFile;
        FT2.WriteFitsFileMerged(FT2);

    } catch (std::exception & eObj) {
        std::cout << eObj.what() << std::endl;
        std::exit(1);
    }
}

void FITS(int iargc, char **argv, bool new_tpl, std::string path) {
    try {

        double FT2_MERGED_BIN_WIDTH(30);

        printf("Merging from fits file\n");

        //File Handlign
        std::string ft2_name;
        std::string ft2_name_merged;
        std::string line, comment;
        std::string Version;
       

        double time, lt, next_lt, prev_lt, el, T_stop, T_start, time_elapsed(0), live_time(0), t1, t2;
        unsigned int Entries(0), nrows, nrows_merged;
        bool first_bin(true), new_entry(false), gap(false);

        getFileNames_FITS(iargc, argv, ft2_name, ft2_name_merged, Version, FT2_MERGED_BIN_WIDTH);

        count_rows(ft2_name, FT2_MERGED_BIN_WIDTH, nrows, nrows_merged);

        fitsGen::MeritFile ft2(ft2_name, "SC_DATA");
        if (new_tpl ==false) {
            path ="/nfs/farm/g/glast/u30/builds/rh9_gcc32opt/ScienceTools/ScienceTools-v9r8p2/fitsGen/v4r1p1/data/ft2.tpl";
            std::cout << "You are using the default ft2 template " << path << "\n";
        }

        fitsGen::Ft2File ft2_merged(ft2_name_merged, 1, "SC_DATA", path);
        unsigned int nEntries(0);
        for (; ft2.itor() != ft2.end(); ft2.next()) {

            nEntries++;
            if (first_bin || new_entry) {

                first_bin = false;
                new_entry = false;


                Entries++;

                ft2_merged["start"].set(ft2["start"]);
                ft2_merged["stop"].set(ft2["stop"]);

                if (Entries == 1) {
                    ft2.row()["start"].get(T_start);
                }
                std::vector<float> scPosition;
                ft2.row()["sc_position"].get(scPosition);
                ft2_merged["sc_position"].set(scPosition);

                ft2_merged["ra_scz"].set(ft2["ra_scz"]);
                ft2_merged["dec_scz"].set(ft2["dec_scz"]);
                ft2_merged["ra_scx"].set(ft2["ra_scx"]);
                ft2_merged["dec_scx"].set(ft2["dec_scx"]);
                ft2_merged["ra_zenith"].set(ft2["ra_zenith"]);
                ft2_merged["dec_zenith"].set(ft2["dec_zenith"]);
                ft2_merged["lon_geo"].set(ft2["lon_geo"]);
                ft2_merged["lat_geo"].set(ft2["lat_geo"]);
                ft2_merged["rad_geo"].set(ft2["rad_geo"]);
                ft2_merged["geomag_lat"].set(ft2["geomag_lat"]);
                ft2_merged["in_saa"].set(ft2["in_saa"]);
                ft2_merged["b_mcilwain"].set(ft2["b_mcilwain"]);
                ft2_merged["l_mcilwain"].set(ft2["l_mcilwain"]);
                ft2_merged["lat_mode"].set(ft2["lat_mode"]);
                if (new_tpl == true) {
                    ft2_merged["rock_angle"].set(ft2["rock_angle"]);
                    ft2_merged["ra_npole"].set(ft2["ra_npole"]);
                    ft2_merged["dec_npole"].set(ft2["dec_npole"]);
                    ft2_merged["lat_config"].set(ft2["lat_config"]);
                    ft2_merged["data_qual"].set(ft2["data_qual"]);
                }
                ft2_merged["qsj_1"].set(ft2["qsj_1"]);
                ft2_merged["qsj_2"].set(ft2["qsj_2"]);
                ft2_merged["qsj_3"].set(ft2["qsj_3"]);
                ft2_merged["qsj_4"].set(ft2["qsj_4"]);

            }

            ft2.row()["livetime"].get(lt);
            live_time += lt;

            ft2.row()["start"].get(t1);
            ft2.row()["stop"].get(t2);
            T_stop = t2;

            time_elapsed += (t2 - t1);

            //  printf("time elapsed=%e\n",time_elapsed);

            //check live time in next entry
            if (nEntries < nrows) {
                //std::cout<<Entries<<" "<<nrows<<"\n";
                ft2.next();
                ft2.row()["livetime"].get(next_lt);
                ft2.prev();
            }

            if (lt == 0 && nEntries > 1) {
                gap = true;
                if (prev_lt > 0) {
                    new_entry = true;
                }
                if (next_lt > 0) {
                    new_entry = true;
                }
            }

            if (lt > 0) {
                gap = false;
            }




            if (time_elapsed >= FT2_MERGED_BIN_WIDTH && gap == false) {
                new_entry = true;
            }

            if (new_entry == true) {
                ft2_merged["stop"].set(t2);
                //store t_stop
                //store live_time
                //add one element to the arrays
                ft2_merged["livetime"].set(live_time);
                ft2_merged.next();
                live_time = 0;
                time_elapsed = 0;
            }
            prev_lt = lt;

        }
        //To eval in the last bin
        if (new_entry == false) {
            ft2_merged["stop"].set(t2);
            //store live_time
            //add en element to the arrays
            ft2_merged["livetime"].set(live_time);
            new_entry = true;
            live_time = 0;
            time_elapsed = 0;
        }
        printf("merged  entries, entr=%d\n", Entries);

        ft2_merged.setObsTimes(T_start, T_stop);
        std::ostringstream creator;
        creator << "ft2Util";
        ft2_merged.setPhduKeyword("CREATOR", creator.str());
        ft2_merged.setPhduKeyword("VERSION", Version);
        std::string filename(facilities::Util::basename(ft2_name_merged));
        ft2_merged.setPhduKeyword("FILENAME", filename);
        std::cout << "done\n";



    } catch (std::exception & eObj) {
        std::cout << eObj.what() << std::endl;
        std::exit(1);
    }

}

void count_rows(std::string ft2_name, double FT2_MERGED_BIN_WIDTH, unsigned int & nrows, unsigned int & nrows_merged) {

    fitsGen::MeritFile infile(ft2_name, "SC_DATA");

    double time_elapsed(0), t1, t2;
    nrows = 0;
    nrows_merged = 0;
    for (; infile.itor() != infile.end(); infile.next()) {

        nrows++;

        infile.row()["start"].get(t1);
        infile.row()["stop"].get(t2);
        time_elapsed += (t2 - t1);

        //  printf("time elapsed=%e\n",time_elapsed);
        if (time_elapsed >= FT2_MERGED_BIN_WIDTH) {
            time_elapsed = 0;
            nrows_merged++;
        }

    }
    std::cout << "fits file has " << nrows << " rows\n";
    std::cout << "fits merged file has " << nrows_merged << "  (+1) rows\n";
}
