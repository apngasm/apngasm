@echo off

mkdir ..\release

cmake -DBUILD_SHARED_LIBS=OFF -DZLIB_ROOT=%cd%\windows\zlib -DPNG_ROOT=%cd%\windows\libpng -DBOOST_ROOT=%cd%\windows\boost -DCMAKE_INSTALL_PREFIX:PATH=%cd%\..\release ..
cmake --build . --config Release --target INSTALL