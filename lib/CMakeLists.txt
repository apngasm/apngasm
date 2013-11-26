# shared library and sources
ADD_LIBRARY(apngasm SHARED
	${APNGASM_SOURCE_DIR}/src/apngasm.cpp
	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
	${APNGASM_SOURCE_DIR}/src/apnghelper.cpp
)

#add_library(apngasm-static STATIC
#	${APNGASM_SOURCE_DIR}/src/libapngasm.cpp
#	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
#	${APNGASM_SOURCE_DIR}/src/apnghelper.cpp
#)

FIND_PACKAGE(PNG REQUIRED)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(apngasm ${PNG_LIBRARY})

#find_package(ZLIB_REQUIRED)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(apngasm ${ZLIB_LIBRARIES})

INSTALL(TARGETS apngasm
	LIBRARY DESTINATION lib
	COMPONENT libraries)

INSTALL(FILES ../src/apngasm.h ../src/apngframe.h
	DESTINATION include
	COMPONENT headers)
