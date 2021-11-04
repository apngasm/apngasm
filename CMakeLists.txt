cmake_minimum_required(VERSION 3.13)

project(APNGASM)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/cli)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

# Create package command
if (UNIX OR APPLE OR MINGW)
  add_custom_target(
    package
    COMMAND ${CMAKE_MAKE_PROGRAM}
    COMMAND ${CMAKE_MAKE_PROGRAM} package-lib
    COMMAND ${CMAKE_MAKE_PROGRAM} package-cli
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
endif ()
