# shared library and sources
add_library(apngasm SHARED
	${APNGASM_SOURCE_DIR}/src/libapngasm.cpp
	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
)

add_library(apngasm-static STATIC
	${APNGASM_SOURCE_DIR}/src/libapngasm.cpp
	${APNGASM_SOURCE_DIR}/src/apngframe.cpp
)
