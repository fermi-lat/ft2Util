
/** @mainpage package ft2Util_2
* @author Giacomo Vianello <giacomov@slac.stanford.edu>
*
* @section intro Introduction
This package provide the capabilities to both handle both produce an FT2 file. See 
http://fermi.gsfc.nasa.gov/ssc/data/analysis/documentation/Cicerone/Cicerone_Data/LAT_Data_Columns.html#SpacecraftFile
for a description of this kind of file.
*
* <hr>
@section handling Handling an FT2 file
To read and get information from an FT2 file, you need to instance a StatusFile class. Then, you can get any attitude/position 
information about the spacecraft or the LAT at any given time (covered by the input FT2 file). For example:
<ul>
 <li> Instantiate the class StatusFile using the name of a FT2
 file as input:
 @verbatim
 #include <ft2Util_2/StatusFile.h>
 
 ft2Util_2::StatusFile statusfile("../data/ft2_example.fits");
 @endverbatim 
 
 <li> Use the () operator to get all the informations for a given
 time contained in the FT2 file (in MET). You can obtain them
 using a syntax like statusfile(time).spacecraft.[method] for 
 the spacecraft-related quantities, 
 or statusfile(time).lat.[method] for LAT-related quantities. See ft2Util_2::ExtendedPointingInfo for the methods of the spacecraft
 object, and ft2Util_2::LatInfo for the methods of the lat object. For example:
 @verbatim
 //Pick up a time, for example 6.2 s after the beginning of the 
 //3rd interval
 
 double time=statusfile.startTime[2]+ 6.2
 
 //Get the RA of the x Axis of the spacecraft
 
 double RA_SCX=statusfile(time).spacecraft.xAxis().ra();
 
 //Get the RA of the x Axis of the LAT
 
 double RA_LATX=statusfile(time).lat.xAxis().ra();
 @endverbatim
</ul>

* <hr>
*
* @section creating Creating an FT2 file
*
*@subsection s1 1s FT2 file
* To create an FT2 file you can use the makeFT2.exe executable. There are two flavours of FT2 files: the "fake" FT2 file, and the normal
FT2 file. The former lacks the livetime information, and it is generated inside the pipeline at the chunk level to be used as input 
to GLEAM. The latter provides the livetime information, and it is generated inside the pipeline at the run level.\n
To produce an FT2 file you will always need a Magic 7 file. If you want to compute also the 
livetime ("normal" FT2 file), you will need also a Digi file, a Merit file, and a DigiGAP file. You can download all those files for a 
particular run from the data catalog. This is not the place to describe the Digi and the Merit files,
but this is a short introduction to the Magic 7 and DigiGAP files, which are less common:
<ul>
<li> Magic 7 file: this is a text file, which contains two different kinds of messages issued by the spacecraft: the ATT message, or the
ORB message. Here is a section of a real M7 file:
@verbatim
2008-01-01 01:56:40.160000 ATT 220845400 160000 -0.093747 -0.618179 0.668382 0.402905 0.000000 0.000000 0.000000
2008-01-01 01:56:40.360000 ATT 220845400 360000 -0.093711 -0.618222 0.668410 0.402800 0.001228 0.000560 0.001097
2008-01-01 01:56:40.560000 ATT 220845400 560000 -0.093674 -0.618265 0.668439 0.402694 0.001227 0.000560 0.001096
2008-01-01 01:56:40.760000 ATT 220845400 760000 -0.093638 -0.618308 0.668468 0.402589 0.001228 0.000560 0.001097
2008-01-01 01:56:40.960000 ATT 220845400 960000 -0.093602 -0.618351 0.668496 0.402483 0.001228 0.000560 0.001097
2008-01-01 01:56:40.960000 ORB 220845400 960000 -4315522.500000 -5205704.000000 1508676.875000 0.000000 0.000000 0.000000 5 0
2008-01-01 01:56:41.160000 ATT 220845401 160000 -0.093566 -0.618395 0.668525 0.402378 0.001228 0.000560 0.001097
2008-01-01 01:56:41.360000 ATT 220845401 360000 -0.093530 -0.618438 0.668554 0.402272 0.001227 0.000560 0.001096
2008-01-01 01:56:41.560000 ATT 220845401 560000 -0.093493 -0.618481 0.668582 0.402167 0.001228 0.000560 0.001097
2008-01-01 01:56:41.760000 ATT 220845401 760000 -0.093457 -0.618524 0.668611 0.402061 0.001227 0.000560 0.001096
2008-01-01 01:56:41.960000 ATT 220845401 960000 -0.093421 -0.618567 0.668639 0.401956 0.001228 0.000560 0.001097
2008-01-01 01:56:41.960000 ORB 220845401 960000 -4310713.500000 -5210621.000000 1505446.375000 4809.000000 -4917.000000 -3230.500000 5 0
@endverbatim
Here is a description of the two kind of message and their fields:
<ul> 
<li>ATT: this kind of message is issued by the spacecraft every 0.2 s (5 Hz), and contains 12 space-separated fields with attitude information:
<table>
<tr> 
<th> Field <br/> </th> 
<th> Description <br /> </th> 
</tr> 
<tr> 
<td> 1 <br /> </td> 
<td> Date <br/> </td> 
</tr> 
<tr> 
<td> 2 <br /> </td> 
<td > Time (UTC) <br /> </td> 
</tr> 
<tr> 
<td > 3 <br /> </td> 
<td > Record type (=ATT) <br /> </td> 
</tr> 
<tr> 
<td > 4 <br /> </td> 
<td > Spacecraft message timestamp, in MET </td> 
</tr> 
<tr> 
<td > 5 <br /> </td> 
<td > Spacecraft message timestamp, microseconds of the current second </td> 
</tr> 
<tr> 
<td > 6-9 <br /> </td> 
<td > The x, y, z, and w components of the attitude quaternion in the <a href="http://celestrak.com/columns/v02n01/" rel="nofollow">ECI</a> J2000 frame </td> 
</tr> 
<tr> 
<td > 10-12 </td> 
<td > The "rotational state" of the spacecraft, that is, the x,y,z components of the angular <br /> 
velocity vector of the spacecraft (rad/s), in the spacecraft reference frame (see figure 1) <br /> </td> 
</tr> 
</table>  
</li> 
<li>ORB: this kind of message is issued by the spacecraft every second (1 Hz) and contains 13 space-separated fields with information about the current orbit position:
<table >
<tr> 
<th > Field <br /> </th> 
<th > Description <br /> </th> 
</tr> 
<tr> 
<td > 1 <br /> </td> 
<td > Date <br /> </td> 
</tr> 
<tr> 
<td > 2 <br /> </td> 
<td > Time (UTC) <br /> </td> 
</tr> 
<tr> 
<td > 3 <br /> </td> 
<td > Record type (=ORB) <br /> </td> 
</tr> 
<tr> 
<td > 4 <br /> </td> 
<td > Spacecraft message timestamp, in MET </td> 
</tr> 
<tr> 
<td > 5 <br /> </td> 
<td > Spacecraft message timestamp, microseconds of the current second </td> 
</tr> 
<tr> 
<td > 6-8 <br /> </td> 
<td > The x, y, z, coordinates of the spacecraft position in the <a href="http://celestrak.com/columns/v02n01/" rel="nofollow">ECI</a> J2000 frame (meters) <br /> </td> 
</tr> 
<tr> 
<td > 9-11 <br /> </td> 
<td > The x,y,z components of the velocity vector of the spacecraft in the <a href="http://celestrak.com/columns/v02n01/" >ECI</a>J2000 frame (m/s) <br /> </td> 
</tr> 
<tr> 
<td > 12 <br /> </td> 
<td > Spacecraft GNC mode (integer) <br /> 
(1=capture, 2=sunpoint, 3=inertial pointing,4=Maneuver,5=zenithpoint/survey; 6-7=reentry modes) <br /> </td> 
</tr> 
<tr> 
<td > 13 <br /> </td> 
<td > Flag indicating whether or not the observatory is within the LAT SAA boundary 1==IN, 0==OUT </td> 
</tr> 
</table> 
</div> 
</li> 
</ul> 
</li>

<li> DigiGAP file: it can happen that, when downloading data from the spacecraft, some datagrams are missed, so a certain number of
event cannot be properly processed and are lost. Usually these gaps in the data stream are recovered with subsequent downloads, but
it can happen that data with gaps reach the pipeline, and it happened a few times that the data were completely lost. 
There is a software (svac/findGaps) which scan the data for this kind of gaps. It generates a text file (digiGAP file) with this structure:
@verbatim
r0xxxxxxxxx 79194 83166
@endverbatim
where the first column is the run number, the second column is the event ID of the last good event, and the third column is the
event ID of the first good event after the gap. The time between those two events is effectively dead time, so it has to be taken
into account computing the deadtime.
</li>
</ul>
This is the command line to produce the FT2 files: (you can obtain this by running "makeFT2.exe -help"):
@verbatim
makeFT2.exe : produce the FT2 file for the Fermi mission.

(See http://fermi.gsfc.nasa.gov/ssc/data/analysis/documentation/Cicerone/Cicerone_Data/LAT_Data_Columns.html#SpacecraftFile)                                      

Usage: 

-Produce the 'fake' FT2 file (without livetime):

makeFT2.exe -ft2start [start time] -ft2stop [stop time]
-m7file [Magic 7 file] -ft2file [output FT2 file]
[-clobber ['yes' or 'no'] ]                      

-Produce a regular FT2 file (with livetime):

makeFT2.exe -ft2start [start time] -ft2stop [stop time]
-m7file [Magic 7 file] -ft2file [output FT2 file]
-digifile [Digi file] -meritfile [Merit file]    
-gapfile [Digi gap file]
[-clobber ['yes' or 'no'] -verify ['yes' or 'no']
-dataquality [DQ flag] -latconfig [LAT config value]
-templateFT2 [FT2 template] -version [version number] ]

Parameters:

-ft2start         Start time for the output FT2 file (MET)
-ft2stop          Stop time for the output FT2 file (MET)
-m7file           Input Magic 7 file
-ft2file          Name for the output FT2 file
-digifile         Input Digi file
-meritfile        Input Merit file
-gapfile          Input Digi gap file
-clobber          (yes|no) Overwrite an existing output? (default = no)
-dataquality      Data Quality flag to set (default = 2)
-latconfig        LAT configuration status (default = 1)
-templateFT2      Override the default template file for the output FT2
-version          Version number for the file (integer,default=1)
-configFile       User-provided configuration file path
-verify           (yes|no) Verify Merit and Digi for gaps (default=yes)
-help             Prints this help
@endverbatim
while this is the command line to rebin a FT2 file produced with the command above:  (you can obtain this by running "mergeFT2.exe -help"):
@verbatim
mergeFT2.exe : Merge bins from 1s FT2 file producing a 30s FT2 file, or a FT2 file with the specified binning.

(See http://fermi.gsfc.nasa.gov/ssc/data/analysis/documentation/Cicerone/Cicerone_Data/LAT_Data_Columns.html#SpacecraftFile)

mergeFT2.exe -inputFT2 [input 1s FT2 file] -outputFT2 [output FT2 file]
-version [version int]
[-dt [time step] -clobber ['yes' or 'no']

Parameters:

-inputFT2         Input 1s FT2 file
-outputFT2        Name for the output FT2 file
-version          Version to be written in the header (integer, default=1)
-dt               Bin size for the output file*
-clobber          (yes|no) Overwrite an existing output? (default = no)
-help             Prints this help

* Note that some bins in the output file could have a different duration than 'dt', since the program will prematurely cut bins for example if LAT_MODE or LAT_CONFIG change value, or if it encounters bins with livetime=0 in the input FT2 file
@endverbatim
  * 
*/







