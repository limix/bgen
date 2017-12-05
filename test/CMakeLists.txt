include_directories("${TEST_SOURCE_DIR}/src")

add_executable(v12_zlib_layout2 v12_zlib_layout2.c example_files.c)
target_link_libraries(v12_zlib_layout2 bgen)
add_test(test_v12_zlib_layout2 v12_zlib_layout2)

list(APPEND DATA_FILES example.matrix example.1bits.bgen)
list(APPEND DATA_FILES example.14bits.bgen example.32bits.bgen)
list(APPEND DATA_FILES example.v11.bgen)

foreach(DATA_FILE ${DATA_FILES})
    set(ORIGIN "data/${DATA_FILE}")
    file(COPY ${ORIGIN} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data")
endforeach()
