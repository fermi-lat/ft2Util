/** @file FT2_Time.cxx
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

 */



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

// headers for exceptions
#include <stdexcept>

int main(int argc, char **argv) {

    try {
        //------ MAKE FT2 FILES ----------
        //This is the main class
        FT2 FT2;
        //--------------------------------

        printf("v1r2p31 \n");

        //--- Get File Names ---
        FT2.getFileNames(argc, argv, FT2);

        if (FT2.Gleam == true) {
            FT2.Gleam_FT2(FT2);
        } else if (FT2.MC == true) {
            FT2.DigiMC_FT2(FT2);
            //printf("No More supported\n");
        } else {
            if ((FT2.DigiCutT1 == true) && (FT2.DigiCutT2 == true)) {
                FT2.DigiCut = true;
            }

            if ((FT2.DigiCutT1 == false) && (FT2.DigiCutT1 == true)) {
                printf("Please provide also DigiTstart\n");
                exit(0);
            }

            if ((FT2.DigiCutT1 == true) && (FT2.DigiCutT1 == false)) {
                printf("Please provide also DigiTstop\n ");
                exit(0);
            }
            FT2.Digi_FT2(FT2);
        }
    } catch (std::exception & eObj) {
        std::cout << eObj.what() << std::endl;
        std::exit(1);
    }
}


