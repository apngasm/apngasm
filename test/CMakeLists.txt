cmake_minimum_required(VERSION 3.13)

project(APNGASM_TEST)

link_directories(BEFORE ${PROJECT_BINARY_DIR}/../lib)

enable_testing()

# Generate C++ test
add_executable(apngasm-cpp-test
  EXCLUDE_FROM_ALL
  ${PROJECT_SOURCE_DIR}/cpp/apngasm-test.cpp
)

set_target_properties(apngasm-cpp-test
  PROPERTIES
  OUTPUT_NAME apngasm-cpp-test
  CXX_STANDARD 20
)

link_directories(${PROJECT_BINARY_DIR}/lib)
include_directories(
  ${PROJECT_SOURCE_DIR}/../lib/src
  ${CMAKE_BINARY_DIR}/lib/src
)
target_link_libraries(apngasm-cpp-test apngasm)

add_test(NAME apngasm-cpp-test
  COMMAND apngasm-cpp-test ${PROJECT_SOURCE_DIR}/samples
)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(apngasm-cpp-test ${ZLIB_LIBRARIES})

#file(COPY ${PROJECT_SOURCE_DIR}/samples DESTINATION ./)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/cli-out)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/cpp-out)

add_custom_command(
  DEPENDS ${CMAKE_BINARY_DIR}/test/apngasm-cpp-test
  OUTPUT
    ${CMAKE_BINARY_DIR}/test/cpp-out/circle_anim.png
    ${CMAKE_BINARY_DIR}/test/cpp-out/circle_anim2.png
    ${CMAKE_BINARY_DIR}/test/cpp-out/penguins.json
    ${CMAKE_BINARY_DIR}/test/cpp-out/penguins.xml
    ${CMAKE_BINARY_DIR}/test/cpp-out/test5_anim.png

	#  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
  COMMAND apngasm-cpp-test ${PROJECT_SOURCE_DIR}/samples
)

add_custom_target(test
  DEPENDS
    ${CMAKE_BINARY_DIR}/test/cpp-out/circle_anim.png
    ${CMAKE_BINARY_DIR}/test/cpp-out/circle_anim2.png
    ${CMAKE_BINARY_DIR}/test/cpp-out/penguins.json
    ${CMAKE_BINARY_DIR}/test/cpp-out/penguins.xml
    ${CMAKE_BINARY_DIR}/test/cpp-out/test5_anim.png
)


