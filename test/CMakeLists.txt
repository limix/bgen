include_directories (${TEST_SOURCE_DIR}/src)

add_executable (test_bgen test_bgen.c)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries (test_bgen LINK_PUBLIC bgen)
else()
  target_link_libraries (test_bgen LINK_PUBLIC bgen m)
endif()

configure_file(data/example.matrix ${CMAKE_CURRENT_BINARY_DIR}/data/example.matrix COPYONLY)
configure_file(data/example.1bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen COPYONLY)
configure_file(data/example.32bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen COPYONLY)
configure_file(data/example.v11.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen COPYONLY)
