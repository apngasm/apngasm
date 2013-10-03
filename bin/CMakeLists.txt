LINK_DIRECTORIES(${APNGASM_BINARY_DIR}/lib)

ADD_EXECUTABLE(apngasm-cli
    ${APNGASM_SOURCE_DIR}/src/apngasm-cli.cpp
)
SET_TARGET_PROPERTIES(apngasm-cli
	PROPERTIES OUTPUT_NAME apngasm)

TARGET_LINK_LIBRARIES(apngasm-cli apngasm)

FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options regex) 
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}) 
TARGET_LINK_LIBRARIES(apngasm-cli ${Boost_LIBRARIES})                                                                                                                                                                                                                            

INSTALL(TARGETS apngasm-cli
	RUNTIME	DESTINATION bin
	COMPONENT applications)
