
/** @mainpage package ft2Util
* @author Andrea Tramacere <tramacer@slac.stanford.edu>
*
* @section intro Introduction
* ft2Util is a package used to build the FT2 files. Digi file is used to evaluate
* the LiveTime for eache FT2 entry. The Merit File is used to find 
* recon crasches and correct LiveTime Accrdingly. The M7 file is used t o generate FT2 time entries.
*
* <hr>
* @section FT2  makeFT2Entries
*
* This command is used to generate FT2 file with 1 seconds entries.
*
* @subsection jobOptions jobOptions
* -# - -MC to work with MonteCarlo Data
* -# - -Gleam to generate Gleam FT2 file
* 
* @param -M7File magic7.txt  
*
* @param -FT2_txt_File ft2-out.txt 
*
* @param -FT2_fits_File ft2.fits
*
* <hr>
*
* @section mergeFT2Entries mergeFT2Entries
*
* This command is used to merge 1 seconds entries in to 30 seconds entries
*
* @subsection jobOptions jobOptions
*
* None
* 
* @param -FT2_txt_File ft2-out.txt 
*
* @param -FT2_fits_File ft2.fits
*
*
* 
* @section notes release notes
* release.notes
*
* @section requirements requirements
* @verbinclude requirements
*
*/







