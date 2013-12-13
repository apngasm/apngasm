# shared library and sources
ADD_LIBRARY(apngasm SHARED
	${APNGASM_SOURCE_DIR}/src/apngasm.cpp
	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
  ${APNGASM_SOURCE_DIR}/src/spec/specreader.cpp
  ${APNGASM_SOURCE_DIR}/src/spec/priv/specreaderimpl.cpp
)

#add_library(apngasm-static STATIC
#	${APNGASM_SOURCE_DIR}/src/libapngasm.cpp
#	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
# ${APNGASM_SOURCE_DIR}/src/spec/specreader.cpp
#	${APNGASM_SOURCE_DIR}/src/spec/priv/specreaderimpl.cpp
#)

FIND_PACKAGE(PNG REQUIRED)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(apngasm ${PNG_LIBRARY})

#find_package(ZLIB_REQUIRED)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(apngasm ${ZLIB_LIBRARIES})

FIND_PACKAGE(Boost REQUIRED COMPONENTS regex filesystem system) 
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(apngasm ${Boost_LIBRARIES})

INSTALL(TARGETS apngasm
	LIBRARY DESTINATION lib
	COMPONENT libraries)

INSTALL(FILES ../src/apngasm.h ../src/apngframe.h ../src/apngasm-version.h
	DESTINATION include
	COMPONENT headers)
