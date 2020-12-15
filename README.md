apngasm
=======
The next generation of apngasm, the APNG Assembler.

Installing
==========
If you just want pre-built libapngasm and apngasm:  
*OSX*: For the latest apngasm use homebrew: `brew install apngasm`.  
*Windows*: Click "releases" at the top of the github page and get the latest release.  
*GNU Linux*: [Debian/Ubuntu/Mint/etc.] [PPA](https://code.launchpad.net/~zero-tsuki/+archive/ppa).  

Building
========
apngasm uses CMake to provide cross platform build chains.

Building on (Ubuntu/Debian) Linux:  
----------------------------------
1. Install dependencies/build chain  
    ```
    sudo apt-get install cmake libpng-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libboost-filesystem-dev build-essential
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
4. Installing or [optional] Build packages  
You can either install directly or roll your own .deb package to keep it under system package 
management.  
    To install, just do:
    ```
    sudo make install
    ```
    To make packages:
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
1. Install build requirements. We recommend using a package manager such as 
    [Homebrew](http://brew.sh/). Just keep in mind that if you want to distribute packages you 
    will need the same set of libraries on each system. Since OSX comes with its own (older) 
    versions of libpng and zlib, you will NOT want to install libpng and zlib from your package 
    manger IF you want to distribute packages.
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

Building on Windows (with Visual Studio):
-----------------------------------------
Unfortunately there are two issues with building on Windows:
1. CMake for Windows does not seem to generate Visual Studio projects cleanly (interim targets 
    are all generated as separate solutions).
2. Newer versions of Visual Studio don't seem to be able to handle linking against libpng16 
    very well and require a variety of tweaks and hacks.

If anyone familiar newer versions of Visual Studio and CMake on Windows would be willing to 
make any necessary adjustments or can provide instructions it would be much appreciated.

Building on Windows (with MSYS2):  
---------------------------------
Building on Windows with MSYS2 is similar to building on Linux. The following packages are 
necessary to build apngasm: 
`cmake gcc zlib-devel zlib mingw-w64-x86_64-libpng mingw-w64-x86_64-boost`

Step by step:
1. Install dependencies/build chain  
    ```sh
    pacman -Sy cmake gcc zlib-devel zlib mingw-w64-x86_64-libpng mingw-w64-x86_64-boost
    ```
2. Generate Unix style Makefiles with CMake  
    ```sh
    mkdir build;
    cd build;
    cmake -G"Unix Makefiles" ..
    ```
3. Make  
    ```sh
    make
    ```
4. Installing or Building Packaged Installers
    To install without building, just do:
    ```
    make install
    ```
    To build installer packages, first install NSIS version 3 or above, then:
    ```
    CMAKE_GENERATOR="Unix Makefiles" make package
    ```

Interfaces
----------
apngasm now uses SWIG to generate interface wrappers. Currently Java and Ruby are supported - 
but if you are building for ruby you may as well use the rapngasm gem.

Building for Java:
```
mkdir build
cd build
cmake -DJAVA=true ..
make
make java
make jar
```

Building for Ruby (raw library, not using the rapngasm gem):
```
mkdir build
cd build
cmake -DRUBY=true ..
make
make ruby
```

If you'd like wrappers generated for another language we request you try and add it yourself and 
make a pull request. Each language has its own particular tweaks that may be neccesary so it's 
hard for us to implement on languages we aren't familiar with.

Special Thanks
--------------
* Special thanks to all our backers - without you this wouldn't have been possible
* Thanks to @vflyson for the pkgconfig headers


License
-------
zlib/libpng

Copyright
---------
This version of apngasm is a joint production by Max Stepin and K.K. GenSouSha.
apngasm is wholly copyright Max Stepin, all rights reserved.
