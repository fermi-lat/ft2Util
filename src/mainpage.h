
/** @mainpage package ft2Util
* @author Andrea Tramacere <tramacer@slac.stanford.edu>
*
* @section intro Introduction
* ft2Util is a package used to build the FT2 files. \n
FT2 file will store ORBIT (position) and ATTITUDE information and the LIVETIME (plus SAA and OBSMODE flags and Geomagnetic quantities)
LIVETIME is evaluated from the Digi-File counters.\n 
Merit-Recon file is used to identify Recon-Crashes and to correct accordingly LIVETIME. \n
Gaps in the Digi-File are used to evaluate Dead-Time from these  Gaps (A txt file with Gaps Star-Stop is provided). \n
M7 file stores ORB-ATT element more SAA and Observing-Mode Flag. \n
Gaps in M7 file  areinterpolated with different algorithms depending on the kind of gap.\ n



*
* <hr>
* @section FT2  makeFT2Entries
*
* This command is used to generate FT2 file with 1 seconds entries.
*
* @subsection jobOptions jobOptions
* -# - -MC : to work with MonteCarlo Data
* -# - -Gleam : to generate Gleam FT2 file
* -# - -verbose : to get a verbose output
* -# - -h --help : to get the manpage
* -# - -test-quaternion : to correct the normalization of the quaternions

* @param  -DigiFile <FileName>  input Digi file  
* @param  -MeritFile <FileName> input Merit file
* @param  -M7File <FileName>    input Magic7 file
* @param  -FT2_txt_File <FileName> output FT2 textual file
* @param  -FT2_fits_File <FileName> output FT2 fits file
* @param  -Gaps_File <FileName> input Digi gaps textual file
* @param  -DigiTstart <double> time of the first Digi evt
* @param  -DigiTstop <double> time of the last Digi evt
* @param  -Version <> 
* @param  -ATTNomrTolerance <double> Tolerance for The normaliazation of the quaternion (def=1e-5)
* @param  -ATTDeltaT_TstartTolerance <double>  Tolerance for moving ATT entry for actual TSTAMP to enetry TSTART (def=1e-5 s)
* @param  -ORBDeltaT_TstartTolerance <double> Tolerance for moving ORB entry for actual TSTAMP to enetry TSTART (def=1e-5 s)
* @param  -FT2_BIN_WIDTH <double> span of the bin of the non merged FT2 file (def=1 s)

* @subsection  of command line
* Digi: makeFT2Entries.exe -DigiFile digi.root -MeritFile merit.root -M7File m7.txt -FT2_txt_File ft2-out.txt -FT2_fits_File ft2.fits -DigiTstart  223101011.025 -DigiTstop 223101035.126 --test-quaternion
*
* Gleam: makeFT2Entries.exe --Gleam -M7File magic7_080428001.txt -FT2_txt_File ft2-Gleam.dat -FT2_fits_File ft2-Gleam.fits -DigiTstart 258229199.7 -DigiTstop 258234933.8 --test-quaternion 
*
* <hr>
*
* @section mergeFT2Entries mergeFT2Entries
*
* This command is used to merge 1 seconds entries in to 30 seconds entries
*
* @subsection jobOptions jobOptions
* -# -  -h - -help : to get the manpage
* 
* 
* @param -FT2_txt_File ft2-out.txt 
* @param -FT2_fits_File ft2.fits
* @param -FT2_MERGED_BIN_WIDTH <double> span of the bin of the merged FT2 file (def=30 s)
* @param -Version <> vesion of the file
* 
* @section notes release notes
* release.notes
*
* @section requirements requirements
* @verbinclude requirements
*
*/







