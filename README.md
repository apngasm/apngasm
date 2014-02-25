apngasm
=======
The next generation of apngasm, the APNG Assembler.

Building
========
apngasm uses CMake to provide cross platform build chains.

For all systems first clone the repository then enter the repository directory in 

Building on (Ubuntu/Debian) Linux:  
----------------------------------
1. Install dependencies/build chain  
    ```
    sudo apt-get install cmake libpng-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev   libboost-filesystem-dev
    ```
2. Generate Makefiles with CMake  
    ```
    mkdir build;
    cd build;
    cmake ../
    ```
3. Make  
    ```
    make
    ```
4. [optional] Build packages  
    ```
    make package
    ```

    Then install them:
    ```
    sudo dpkg -i ./lib/package/libapngasm*.deb
    sudo dpkg -i ./cli/package/apngasm*.deb
    ```

Building on OS-X (with homebrew):  
---------------------------------
1. Install build requirements. We recommend using a package manager such as [Homebrew](http://brew.sh/). Just keep in mind that if you want to distribute packages you will need the same set of libraries on each system. Since OSX comes with its own (older) versions of libpng and zlib, you will NOT want to install libpng and zlib from your package manger IF you want to distribute packages.
```
brew install cmake boost libpng lzlib
```

2. Generate Makefiles with CMake  
    ```
    mkdir build;
    cd build;
    cmake ../
    ```
3. Make  
    ```
    make
    ```
5. [optional] Build packages  
    On OSX you will need Package Maker from [developer.apple.com](https://developer.apple.com/downloads). Use the search tool to find the latest "Auxiliary tools for Xcode" that contains "Package Maker". Download and install it. Then:  
    ```
    make package
    ```  
    Installing packages:  
    Open up the build directory in finder, navigate to lib/packages to find the libapngasm dmg. Open up the dmg file and run the package installer. Next do the same the apngasm dmg in cli/packages.
  
Building on Windows (with Visual Studio):  
----------------------------------------------
Windows is the most difficult build platform to set up as it totally lacks any sort of standard packaging and many of the tools require manual configuration though a GUI. If anyone has a more full description of how to build please fork/edit this description on github and make a pull request.
1. Install CMake for windows from [here](http://www.cmake.org/cmake/resources/software.html)  
2. Install or build libpng and zlib and put them (and their headers) somewhere in that path that CMake can find them.
3. Generate VS project files with the CMake GUI.  
3. Open VS project files in Visual Studio  
4. Hit build
    Libraries will be found in the lib directory.  
    The apngasm command line tool can be found in the bin directory.
5. [optional] Generate Installer
    ??? we're still working on this one.  

License
-------
zlib/libpng

Copyright
---------
This version of apngasm is a joint production by Max Stepin and Genshin Souzou Kabushiki Kaisha.
The original apngasm is wholly copyright Max Stepin. The apngasm name and project is copyright Max Stepin all rights reserved.
