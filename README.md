apngasm
=======
The next generation of apngasm, the APNG Assembler.

Installing
==========
If you just want pre-built libapngasm and apngasm:  
*OSX*: For the latest apngasm use homebrew: `brew install apngasm`. Otherwise Click "releases" at the top of the github page and get the latest build with an OSX package.  
*Windows*: Click "releases" at the top of the github page and get the latest release.  
*GNU Linux*: [Debian/Ubuntu/Mint/etc.] [PPA](https://code.launchpad.net/~zero-tsuki/+archive/ppa)

Building
========
apngasm uses CMake to provide cross platform build chains.

For all systems first clone the repository then enter the repository directory in 

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
You can either install directly or roll your own .deb package to keep it under system package management.  
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
Please see detailed instructions [here](https://github.com/apngasm/apngasm/blob/master/docs/building/win/instructions.md)

Interfaces
----------
apngasm now has interface wrappers with SWIG. Currently Java and Ruby are supported out of box - but if you are building for ruby you may as well use the rapngasm gem.

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

If you'd like wrappers generated for another language we request you try and add it yourself and make a pull request. Each language has its own particular tweaks that may be neccesary so it's hard for us to implement on languages we aren't familiar with.

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
The original apngasm is wholly copyright Max Stepin. The apngasm name and project is copyright Max Stepin all rights reserved.
