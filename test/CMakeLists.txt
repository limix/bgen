include_directories (${TEST_SOURCE_DIR}/src)

add_executable (test_bgen_reader test_bgen_reader.c)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries (test_bgen_reader LINK_PUBLIC bgen_reader)
else()
  target_link_libraries (test_bgen_reader LINK_PUBLIC bgen_reader m)
endif()

# file(COPY table.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
