@echo off

mkdir ..\release

cmake -DBUILD_SHARED_LIBS=OFF -DZLIB_ROOT=%cd%\zlib -DPNG_ROOT=%cd%\libpng -DBOOST_ROOT=%cd%\boost -DCMAKE_INSTALL_PREFIX:PATH=%cd%\..\release ..
cmake --build . --config Release --target INSTALL