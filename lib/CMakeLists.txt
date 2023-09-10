cmake_minimum_required(VERSION 3.13)

project(APNGASM_LIB)

### Set version
set(APNGASM_LIB_VERSION_MAJOR 3)
set(APNGASM_LIB_VERSION_MINOR 1)
set(APNGASM_LIB_VERSION_PATCH 10)

# Set the version code
set(APNGASM_LIB_VERSION "${APNGASM_LIB_VERSION_MAJOR}.${APNGASM_LIB_VERSION_MINOR}.${APNGASM_LIB_VERSION_PATCH}")
# Generate a header file with the version code
configure_file(${PROJECT_SOURCE_DIR}/src/apngasm-version.h.in ${PROJECT_BINARY_DIR}/src/apngasm-version.h @ONLY)
# Generate a package manifest file with the version
configure_file(${PROJECT_SOURCE_DIR}/manifest.txt.in ${PROJECT_BINARY_DIR}/manifest.txt @ONLY)

include_directories(${PROJECT_BINARY_DIR}/src)

# Sources and Headers
set(APNGASM_HEADERS
  ${PROJECT_SOURCE_DIR}/src/apngasm.h
  ${PROJECT_SOURCE_DIR}/src/apngframe.h
  ${PROJECT_SOURCE_DIR}/src/apngasm-conf.h
  ${PROJECT_BINARY_DIR}/src/apngasm-version.h
)
set(APNGASM_SOURCES
  ${PROJECT_SOURCE_DIR}/src/apngasm.cpp
  ${PROJECT_SOURCE_DIR}/src/apngframe.cpp
  ${PROJECT_SOURCE_DIR}/src/spec/specreader.cpp
  ${PROJECT_SOURCE_DIR}/src/spec/specwriter.cpp
  ${PROJECT_SOURCE_DIR}/src/spec/priv/specreaderimpl.cpp
  ${PROJECT_SOURCE_DIR}/src/spec/priv/specwriterimpl.cpp
  ${PROJECT_SOURCE_DIR}/src/listener/apngasmlistener.cpp
)

# Hack for MSYS2 to detect packages properly
# Note: this hack necessitates a 64 bit build
if (MSYS)
  include_directories("/mingw64/include")
  link_directories("/mingw64/lib")
  set(PNG_ROOT "/mingw64")
  set(ZLIB_ROOT "/mingw64")
  set(Boost_ROOT "/mingw64")
endif ()

# Add the dynamic/shared library
set(APNGASM_DYNAMIC_LIB_TARGET apngasm)
add_library(${APNGASM_DYNAMIC_LIB_TARGET} SHARED
  ${APNGASM_SOURCES}
)
set_target_properties(${APNGASM_DYNAMIC_LIB_TARGET}
  PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
  OUTPUT_NAME apngasm
  CXX_STANDARD 20
)

# Add the static library/archive
set(APNGASM_STATIC_LIB_TARGET apngasm-static)
add_library(${APNGASM_STATIC_LIB_TARGET}
  ${APNGASM_SOURCES}
)
set_target_properties(${APNGASM_STATIC_LIB_TARGET}
  PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
  OUTPUT_NAME apngasm
  CXX_STANDARD 20
)

# Add libraries/includes
#target_link_libraries(${APNGASM_DYNAMIC_LIB_TARGET} stdc++fs)
#target_link_libraries(${APNGASM_STATIC_LIB_TARGET} stdc++fs)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(${APNGASM_DYNAMIC_LIB_TARGET} ${ZLIB_LIBRARIES})
target_link_libraries(${APNGASM_STATIC_LIB_TARGET} ${ZLIB_LIBRARIES})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(${APNGASM_DYNAMIC_LIB_TARGET} ${PNG_LIBRARIES})
target_link_libraries(${APNGASM_STATIC_LIB_TARGET} ${PNG_LIBRARIES})
message(${PNG_VERSION_STRING})

#SET(CMAKE_FIND_LIBRARY_SUFFIXES .dll.a .a .lib ${CMAKE_FIND_LIBRARY_SUFFIXES})
find_package(Boost REQUIRED COMPONENTS program_options regex system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(${APNGASM_DYNAMIC_LIB_TARGET} ${Boost_LIBRARIES})
target_link_libraries(${APNGASM_STATIC_LIB_TARGET} ${Boost_LIBRARIES})

get_target_property(APNGASM_DYNAMIC_LIB_TARGET_NAME ${APNGASM_DYNAMIC_LIB_TARGET} OUTPUT_NAME)
get_target_property(APNGASM_STATIC_LIB_TARGET_NAME ${APNGASM_STATIC_LIB_TARGET} OUTPUT_NAME)
set(APNGASM_LIBRARIES
  ${PROJECT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${APNGASM_DYNAMIC_LIB_TARGET_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${PROJECT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${APNGASM_STATIC_LIB_TARGET_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
)

# Installation
install(FILES ${APNGASM_HEADERS}
  DESTINATION "include"
)
if (NOT DEFINED LIB_INSTALL_DIR)
  set(LIB_INSTALL_DIR "lib")
endif ()
install(
  TARGETS
    ${APNGASM_DYNAMIC_LIB_TARGET}
    ${APNGASM_STATIC_LIB_TARGET}
  DESTINATION ${LIB_INSTALL_DIR}
)

# Create package command
set(PACKAGE_DIR ${PROJECT_BINARY_DIR}/package)

if (WIN32)
  string(REPLACE "$(Configuration)" "\\\${CMAKE_INSTALL_CONFIG_NAME}" INSTALL_LIBS "${INSTALL_LIBS}")
endif ()

configure_file(${PROJECT_SOURCE_DIR}/package/CMakeLists.txt.in ${PACKAGE_DIR}/CMakeLists.txt @ONLY)

if (UNIX OR APPLE OR MINGW)
  add_custom_target(
    package-lib
    COMMAND ${CMAKE_COMMAND} .
    COMMAND ${CMAKE_MAKE_PROGRAM} package
    WORKING_DIRECTORY ${PACKAGE_DIR}
)
endif ()

# Package .in
if (NOT WIN32 OR CYGWIN OR MINGW)
  # Set some pkg-config variables
  set(prefix      ${CMAKE_INSTALL_PREFIX})
  set(exec_prefix ${CMAKE_INSTALL_PREFIX})
  set(libdir      ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})
  set(includedir  ${CMAKE_INSTALL_PREFIX}/include)
  set(LIBS "-lz -lpng")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libapngasm.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libapngasm.pc @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libapngasm.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif ()

# SWIG
option(RUBY "Build Ruby interfaces")
option(JAVA "Build Java interfaces")

if (RUBY)
  include(${PROJECT_SOURCE_DIR}/interfaces/ruby/CMakeLists.txt)
endif ()

if (JAVA)
  include(${PROJECT_SOURCE_DIR}/interfaces/java/CMakeLists.txt)
endif ()
