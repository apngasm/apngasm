link_directories(${APNGASM_BINARY_DIR}/lib)

add_executable(apngasm-bin
    ${APNGASM_SOURCE_DIR}/src/apngasm.cpp
)
set_target_properties(apngasm-bin
	PROPERTIES OUTPUT_NAME apngasm)

target_link_libraries(apngasm-bin apngasm)
