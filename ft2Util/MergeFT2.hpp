/** @file  MergeFT2.hpp
@brief 

@author Andrea Tramacere <tramacer@slac.stanford.edu>

*/

#ifndef __mergeft2__
#define __mergeft2__

const char usage[] =
    "usage: mergeFT2Entries.exe  [OPTIONS]\n"
    " -FT2_txt_File <FileName>\n"
    " -FT2_fits_File <FileName>\n"
    " -FT2_fits_merged_File <FileName> \n"
    " -FT2_MERGED_BIN_WIDTH <span of the bin of the merged FT2 file (def=30 s)>\n"
    " -Version <vesion of the file>\n"
    "--old_tpl to merge files with old ft2 template\n"
    " -h --help\n";

void getFileNames_TXT(int iargc, char * argv[], std::string & FT2File,
		       std::string & fitsFile,  std::string & Version,FT2 &FT2);

void getFileNames_FITS(int iargc, char * argv[], std::string & ft2,
        std::string & ft2_merged, std::string & Version, double & FT2_MERGED_BIN_WIDTH );

void FITS(int iargc, char *argv[], bool old_tpl);

void TXT(int iargc, char *argv[], bool old_tpl);

void  count_rows( std::string ft2_name, double FT2_MERGED_BIN_WIDTH, unsigned int & nrows, unsigned int & nrows_merged);
#endif
