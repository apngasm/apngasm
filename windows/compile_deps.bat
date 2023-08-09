@echo off

set APNGASM_BUILD_PATH=%cd%

git clone "https://github.com/madler/zlib.git"
cd zlib
mkdir build
cd build
cmake -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_INSTALL_PREFIX:PATH=%APNGASM_BUILD_PATH%\zlib ..
cmake --build . --target INSTALL --config Release

cd %APNGASM_BUILD_PATH%
git clone "https://github.com/glennrp/libpng.git"
cd libpng
mkdir build
cd build
cmake -DCMAKE_C_FLAGS_RELEASE="/MT" -DBUILD_SHARED_LIBS=OFF -DCMAKE_POLICY_DEFAULT_CMP0074=NEW -DCMAKE_INSTALL_PREFIX:PATH=%APNGASM_BUILD_PATH%\libpng -DZLIB_ROOT=%APNGASM_BUILD_PATH%\zlib -DZLIB_USE_STATIC_LIBS=ON -DPNG_SHARED=OFF ..
cmake --build . --target INSTALL --config Release

cd %APNGASM_BUILD_PATH%
curl -O -L "https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.zip"
tar -xf boost_1_82_0.zip
del boost_1_82_0.zip
move boost_1_82_0 boost
cd boost
call bootstrap.bat --prefix=.
b2.exe install --build-dir=tmp --prefix=. --build-type=complete --with-program_options --with-regex --with-system -j4 msvc stage
robocopy include\boost-1_82\boost include\boost /E

exit 0