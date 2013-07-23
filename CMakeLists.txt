# required cmake version
cmake_minimum_required(VERSION 2.8)

# project name
project(apngasm)

# path to include files
include_directories("${PROJECT_SOURCE_DIR}/src")

# register lib/ as a subdirectory
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

# path to libraries
link_directories("${PROJECT_SOURCE_DIR}/lib")

# executable file and sources
add_executable(bin/apngasm
    src/apngasm.cpp
)

# libraries to use
target_link_libraries(bin/apngasm ${PROJECT_SOURCE_DIR}/lib/libapngasm.so)
