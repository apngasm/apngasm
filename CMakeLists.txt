cmake_minimum_required(VERSION 2.8)

project(APNGASM)

include_directories(${APNGASM_SOURCE_DIR}/src)

add_subdirectory(${APNGASM_SOURCE_DIR}/lib)
add_subdirectory(${APNGASM_SOURCE_DIR}/bin)
