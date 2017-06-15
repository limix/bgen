include_directories (${TEST_SOURCE_DIR}/src)

add_executable (test_bgen test_bgen.c)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries (test_bgen LINK_PUBLIC bgen)
else()
  target_link_libraries (test_bgen LINK_PUBLIC bgen m)
endif()

file(COPY data/example.1bits.bgen DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY data/example.32bits.bgen DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
