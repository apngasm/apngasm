# shared library and sources
add_library(libapngasm SHARED
    "${PROJECT_SOURCE_DIR}/src/libapngasm.cpp"
    "${PROJECT_SOURCE_DIR}/src/apngframe.cpp"
)

