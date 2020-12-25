cmake_minimum_required(VERSION 3.13)

project(APNGASM_CLI)

# Generate version header
set(APNGASM_CLI_VERSION_MAJOR 3)
set(APNGASM_CLI_VERSION_MINOR 1)
set(APNGASM_CLI_VERSION_PATCH 6)
set(APNGASM_CLI_VERSION "${APNGASM_CLI_VERSION_MAJOR}.${APNGASM_CLI_VERSION_MINOR}.${APNGASM_CLI_VERSION_PATCH}")

configure_file(${PROJECT_SOURCE_DIR}/src/apngasm-cli-version.h.in ${PROJECT_BINARY_DIR}/src/apngasm-cli-version.h @ONLY)
include_directories(${PROJECT_BINARY_DIR}/src)

# Generate executable file
add_executable(apngasm-cli
	${PROJECT_SOURCE_DIR}/src/apngasm-cli.cpp
	${PROJECT_SOURCE_DIR}/src/options.cpp
	${PROJECT_SOURCE_DIR}/src/cli.cpp
)
set_target_properties(apngasm-cli
	PROPERTIES OUTPUT_NAME apngasm)

# Link libraries
link_directories(${PROJECT_BINARY_DIR}/../lib)
include_directories(
	${PROJECT_SOURCE_DIR}/../lib/src
	${PROJECT_BINARY_DIR}/../lib/src
)
target_link_libraries(apngasm-cli apngasm)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(apngasm-cli ${ZLIB_LIBRARIES})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(apngasm-cli ${PNG_LIBRARY})

if (APPLE OR WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif ()
find_package(Boost REQUIRED COMPONENTS program_options regex filesystem system)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(apngasm-cli ${Boost_LIBRARIES})

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
	get_property(INSTALL_LIBS TARGET apngasm PROPERTY LOCATION)
	string(REPLACE "$(Configuration)" "\\\${CMAKE_INSTALL_CONFIG_NAME}" INSTALL_APPS "${INSTALL_APPS}")
	string(REPLACE "$(Configuration)" "\\\${CMAKE_INSTALL_CONFIG_NAME}" INSTALL_LIBS "${INSTALL_LIBS}")
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
