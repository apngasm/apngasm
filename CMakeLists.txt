cmake_minimum_required(VERSION 2.8)

project(apngasm)

include_directories("${APNGASM_SOURCE_DIR}/src")

add_subdirectory("${APNGASM_SOURCE_DIR}/lib")

link_directories("${APNGASM_BINARY_DIR}/lib")

add_executable(apngasm
    src/apngasm.cpp
)

target_link_libraries(apngasm apngasm)
