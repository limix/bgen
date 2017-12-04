include_directories("${TEST_SOURCE_DIR}/src")

add_executable(test_v12_zlib_layout2 test_v12_zlib_layout2.c example_files.c)
target_link_libraries(test_v12_zlib_layout2 bgen)

configure_file(data/example.matrix "${CMAKE_CURRENT_BINARY_DIR}/data/example.matrix" COPYONLY)
configure_file(data/example.1bits.bgen "${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen" COPYONLY)
configure_file(data/example.14bits.bgen "${CMAKE_CURRENT_BINARY_DIR}/data/example.14bits.bgen" COPYONLY)
configure_file(data/example.32bits.bgen "${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen" COPYONLY)
configure_file(data/example.v11.bgen "${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen" COPYONLY)
