include_directories (${TEST_SOURCE_DIR}/src)

set(LLIBS bgen ${ZLIB_LIBRARIES} ${ZSTD_LIBRARIES})

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(LLIBS ${LLIBS} m)
endif()

#add_executable (test_bgen test_bgen.c)
#target_link_libraries (test_bgen ${LLIBS})

# add_executable (test_v12_zlib_layout2 test_v12_zlib_layout2.c example_files.c)
# target_link_libraries (test_v12_zlib_layout2 ${LLIBS})

add_executable (test_store_cache test_store_cache.c example_files.c cache.c)
target_link_libraries (test_store_cache ${LLIBS})

add_executable (test_load_cache test_load_cache.c example_files.c cache.c)
target_link_libraries (test_load_cache ${LLIBS})

configure_file(data/example.matrix ${CMAKE_CURRENT_BINARY_DIR}/data/example.matrix COPYONLY)

configure_file(data/example.1bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen COPYONLY)
# configure_file(data/example.1bits.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen.index COPYONLY)

configure_file(data/example.14bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.14bits.bgen COPYONLY)
# configure_file(data/example.14bits.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.14bits.bgen.index COPYONLY)

configure_file(data/example.32bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen COPYONLY)
# configure_file(data/example.32bits.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen.index COPYONLY)

configure_file(data/example.v11.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen COPYONLY)
# configure_file(data/example.v11.bgen.index ${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen.index COPYONLY)
