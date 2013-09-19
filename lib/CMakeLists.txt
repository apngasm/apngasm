# shared library and sources
add_library(apngasm SHARED
	${APNGASM_SOURCE_DIR}/src/libapngasm.cpp
	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
)

#add_library(apngasm-static STATIC
#	${APNGASM_SOURCE_DIR}/src/libapngasm.cpp
#	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
#)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(apngasm ${PNG_LIBRARY})
