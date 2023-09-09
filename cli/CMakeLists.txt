cmake_minimum_required(VERSION 3.13)

project(APNGASM_CLI)

### Set version
set(APNGASM_CLI_VERSION_MAJOR 3)
set(APNGASM_CLI_VERSION_MINOR 1)
set(APNGASM_CLI_VERSION_PATCH 10)

# Set the version code
set(APNGASM_CLI_VERSION "${APNGASM_CLI_VERSION_MAJOR}.${APNGASM_CLI_VERSION_MINOR}.${APNGASM_CLI_VERSION_PATCH}")
# Generate a header file with the version code
configure_file(${PROJECT_SOURCE_DIR}/src/apngasm-cli-version.h.in ${PROJECT_BINARY_DIR}/src/apngasm-cli-version.h @ONLY)

# Add include paths
include_directories(
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_SOURCE_DIR}/../lib/src
  ${PROJECT_BINARY_DIR}/../lib/src # Version header is generated here
)

# Add library build path to link directory paths
link_directories(BEFORE ${PROJECT_BINARY_DIR}/../lib)

# Hack for MSYS2 to detect packages properly
# Note: this hack necessitates a 64 bit build
if (MSYS)
  include_directories("/mingw64/include")
  link_directories("/mingw64/lib")
  set(Boost_ROOT "/mingw64")
endif ()

# Search and prepare Boost
find_package(Boost REQUIRED COMPONENTS program_options regex system)
include_directories(${Boost_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

# Generate executable
add_executable(apngasm-cli
  ${PROJECT_SOURCE_DIR}/src/apngasm-cli.cpp
  ${PROJECT_SOURCE_DIR}/src/options.cpp
  ${PROJECT_SOURCE_DIR}/src/cli.cpp
)
set_target_properties(apngasm-cli
  PROPERTIES 
  OUTPUT_NAME apngasm
  CXX_STANDARD 20
)

# Link to built apngasm library
target_link_libraries(apngasm-cli
  apngasm
  ${Boost_LIBRARIES}
)

# Installation
install(TARGETS apngasm-cli DESTINATION "bin")

# Create package command
set(PACKAGE_DIR ${PROJECT_BINARY_DIR}/package)
set(APNGASM_BINARIES ${PROJECT_BINARY_DIR}/apngasm${CMAKE_EXECUTABLE_SUFFIX})

if (WIN32 AND NOT MINGW)
  set(INSTALL_HEADERS
    ${PROJECT_SOURCE_DIR}/../lib/src/apngasm.h
    ${PROJECT_SOURCE_DIR}/../lib/src/apngframe.h
    ${PROJECT_SOURCE_DIR}/../lib/src/apngasm-conf.h
    ${PROJECT_BINARY_DIR}/../lib/src/apngasm-version.h)
  #get_property(INSTALL_LIBS TARGET apngasm PROPERTY LOCATION)
  #string(REPLACE "$(Configuration)" "\\\${CMAKE_INSTALL_CONFIG_NAME}" INSTALL_APPS "${INSTALL_APPS}")
  #string(REPLACE "$(Configuration)" "\\\${CMAKE_INSTALL_CONFIG_NAME}" INSTALL_LIBS "${INSTALL_LIBS}")
endif ()

include(${PROJECT_BINARY_DIR}/../lib/manifest.txt)
configure_file(${PROJECT_SOURCE_DIR}/package/CMakeLists.txt.in ${PACKAGE_DIR}/CMakeLists.txt @ONLY)

if (UNIX OR APPLE OR MINGW)
  add_custom_target(
    package-cli
    COMMAND ${CMAKE_COMMAND} .
    COMMAND ${CMAKE_MAKE_PROGRAM} package
    WORKING_DIRECTORY ${PACKAGE_DIR}
  )
  install(FILES ${PROJECT_SOURCE_DIR}/../docs/man/apngasm.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)
endif ()
