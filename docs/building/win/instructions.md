apngasm Build Instructions for Windows
--------------------------------------
This guide was written for Windows 8 using Visual Studio 2013.  
Building on Windows is significantly more complex than Linux or OSX. Due to this an other factors this guide will likely not be actively maintained. If you find part of this guide needs revision please do not report it as an issue. Instead; fork the repository on github, make your revisions, and submit a pull request.

Preperation
===========
0. Install [git](http://www.git-scm.com) unless you have already installed it with something else. git should be in your system path and the git command should be accessable from the command line or from git bash.
1. Install Visual Studio. Express versions are fine. 
2. Install [CMake](http://www.cmake.org/). Install using the "Win32 Installer". Add CMake to the system path. You must install version 2.8.12 or greater.
3. Install [NSIS](http://nsis.sourceforge.net/). You must install version 3.0a2 or later.

Obtain Dependency Sources
=========================
1. [zlib](http://www.zlib.net): Download source zip (v1.2.8 or greater). 
2. [libpng](http://www.libpng/pub/png/libpng.html): Download source zip.
3. [boost](http://www.boost.org): Download the zip of the Current Release.

Build zlib and libpng
=====================
1. Decompress the files you downloaded. To make things easier we recommend you make a dedicated directory like "libs" to build things.
2. In the libpng directory, edit /projects/vstudio/zlib.props. Change the <ZLibSrcDir> entry to the path where you put zlib.
3. Open the vstudio.sln file. Do the upgrades or whatever Visual Studio asks you.
4. Change build type to "Release Library".
5. Build -> Build Solution.
6. Check that the Release Library directory contains built zlib and libpng libraries.
* If the build fails or you have trouble with this please be advised we have no idea how to help you out. Try Google or StackOverflow.

Build boost
===========
1. Decompress the boost sources.
2. Open a command prompt and navigate to the directory where you decompressed bost.
3. Run bootstrap.bat .
4. Run b2.exe .
5. Header files will be located in the /boost directory, built libraries will be in the /stage/lib directory

Copy Libraries/Headers and Add to System Path
=============================================
You can copy all your header files and lib files into a common directory to save time.  
We recommend a *nix like structure with a lib and an include directory.  
The files you need in include are:  
"(libpng directory)/png.h"  
"(libpng directory)/pngconf.h"  
"(libpng directory)/pnglibconf.h"  
"(zlib directory)/zlib.h"  
"(zlib directory)/zconf.h"  
"(boost directory)/boost/"  
And the files you need in lib are:  
"(libpng directory)/projects/vstudio/Release Library/libpng16.lib"  
"(libpng directory)/projects/vstudio/Release Library/zlib.lib"  
"(boost directory)/stage/lib/libboost_filesystem-vc120-mt-1_55.lib"  
"(boost directory)/stage/lib/libboost_filesystem-vc120-mt-gd-1_55.lib"  
"(boost directory)/stage/lib/libboost_program_options-vc120-mt-1_55.lib"  
"(boost directory)/stage/lib/libboost_program_options-vc120-mt-gd-1_55.lib"  
"(boost directory)/stage/lib/libboost_regex-vc120-mt-1_55.lib"  
"(boost directory)/stage/lib/libboost_regex-vc120-mt-gd-1_55.lib"  
"(boost directory)/stage/lib/libboost_system-vc120-mt-1_55.lib"  
"(boost directory)/stage/lib/libboost_system-vc120-mt-gd-1_55.lib"  
*Your versions may be different. These were just versions we got from our build.  
  
Then add the include and lib directories to your system path. Either set it permanently by going to:  
1. Control Panel->System->Details  
2. Environment Variables  
3. Add paths to the "Path" variable  
  
Or temporarily by entering the following line in your terminal:  
```set path=%path%;c:\usr\lib;c:\usr\include``` <-replacing with your lib and include directories, or individual entries for libpng, zlib and boost separately if you chose not to copy files to common locations.

Get apngasm Source
==================
Clone apngasm with something like this: ```git clone http://github.com/apngasm/apngasm```

Build apngasm Project
=====================
In a command line:  
1. cd apngasm  
2. mkdir build  
3. cd build  
4. cmake ..  
5. Open the APNGASM project file  
6. Change build configuration to Release  
7. Build->Build Solution  
8. An installer for the command line tool, libraries and headers can be found in cli/package under the build directory. Individual libraries can be found in lib, the command line executable can be found in cli.
