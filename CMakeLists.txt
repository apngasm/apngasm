cmake_minimum_required(VERSION 3.13)

project(APNGASM)

# If BUILD_SHARED_LIBS was set to OFF, prepare for static linking
if (DEFINED BUILD_SHARED_LIBS AND NOT BUILD_SHARED_LIBS)
  set(Boost_USE_STATIC_LIBS ON)
  if (MSVC)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)
  endif()

  set(ZLIB_USE_STATIC_LIBS ON)
  
  if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  elseif (LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  endif()
endif()

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
