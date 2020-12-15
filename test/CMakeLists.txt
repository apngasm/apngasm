cmake_minimum_required(VERSION 3.13)

project(APNGASM_TEST)

include_directories(${PROJECT_BINARY_DIR}/src)

enable_testing()
# Generate executable file
add_executable(apngasm-test
	EXCLUDE_FROM_ALL
	${PROJECT_SOURCE_DIR}/apngtest.cpp
)

set_target_properties(apngasm-test
	PROPERTIES
	OUTPUT_NAME apngtest
	CXX_STANDARD 17
)

link_directories(${PROJECT_BINARY_DIR}/../lib)
include_directories(
	${PROJECT_SOURCE_DIR}/../lib/src
	${PROJECT_BINARY_DIR}/../lib/src
)
target_link_libraries(apngasm-test apngasm)

add_test(NAME apngasm-test
	COMMAND apngasm-test
)

#FIND_PACKAGE(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(apngasm-test ${ZLIB_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/samples DESTINATION ./)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/out)
