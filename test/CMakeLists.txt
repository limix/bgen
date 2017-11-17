include_directories (${TEST_SOURCE_DIR}/src)

set(LLIBS bgen ${ZLIB_LIBRARIES} ${ZSTD_LIBRARIES})

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(LLIBS ${LLIBS} m)
endif()

add_executable (test_bgen test_bgen.c)
add_executable (test_store test_store.c)
target_link_libraries (test_bgen ${LLIBS})
target_link_libraries (test_store ${LLIBS})

configure_file(data/example.matrix ${CMAKE_CURRENT_BINARY_DIR}/data/example.matrix COPYONLY)
configure_file(data/example.1bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen COPYONLY)
configure_file(data/example.1bits.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen.index COPYONLY)
configure_file(data/example.32bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen COPYONLY)
configure_file(data/example.32bits.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen.index COPYONLY)
configure_file(data/example.v11.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen COPYONLY)
configure_file(data/example.v11.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen.index COPYONLY)
