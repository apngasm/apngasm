# required cmake version
cmake_minimum_required(VERSION 2.8)

# project name
project(apngasm)

# path to include files
include_directories("${PROJECT_SOURCE_DIR}/src")

# executable file and sources
add_executable(bin/apngasm
    src/apngasm.cpp
)

# path to libraries
link_directories("${PROJECT_BINARY_DIR}/lib")

# libraries to use
target_link_libraries(bin/apngasm apngasm-dev)

# register lib/ as a subdirectory
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

