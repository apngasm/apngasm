apngasm
=======
The next generation of apngasm, the APNG Assembler.

Building
--------
apngasm uses CMake to provide cross platform build chains.

For all systems first clone the repository then enter the repository directory in 

Building on (Ubuntu/Debian) Linux:  

1. Install dependencies/build chain
```
sudo apt-get install cmake libpng-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libboost-filesystem-dev
```

2. Create temporary directory
```
mkdir build
```

3. Move current directory
```
cd build
```

4. Generate makefiles with cmake  
```
cmake ../
```

5. Make  
```
make
```

6. [optional] Build packages
```
make package-apngasm-all
```

Building on OS-X (with homebrew):  

1. Install cmake and libpng  
```
brew install cmake boost libpng lzlib
```

2. Create temporary directory
```
mkdir build
```

3. Move current directory
```
cd build
```

4. Generate makefiles with cmake  
```
cmake ../
```

5. Make  
```
make
```
  
Building on Windows (with Visual Studio 2012):  
1. Install cmake  
2. Generate VS project files  
3. Open VS project files in Visual Studio  
4. Hit build  
  
Libraries will be found in the lib directory.  
The apngasm command line tool can be found in the bin directory.

License
-------
zlib/libpng

Copyright
---------
This version of apngasm is a joint production by Max Stepin and Genshin Souzou Kabushiki Kaisha.
The original apngasm is wholly copyright Max Stepin. The apngasm name and project is copyright Max Stepin all rights reserved.
