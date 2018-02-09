include_directories("${TEST_SOURCE_DIR}/src")

limix_add_test(test_bgen ${PROJECT_NAME} "test_bgen.c")
limix_add_test(test_zero_len_chrom_id ${PROJECT_NAME} "test_zero_len_chrom_id.c")

list(APPEND DATA_FILES example.matrix example.1bits.bgen)
list(APPEND DATA_FILES example.14bits.bgen example.32bits.bgen)
list(APPEND DATA_FILES example.v11.bgen)
list(APPEND DATA_FILES pbgen_example.pgen)
list(APPEND DATA_FILES zero_len_chrom_id.bgen)

foreach(DATA_FILE ${DATA_FILES})
    set(ORIGIN "data/${DATA_FILE}")
    file(COPY ${ORIGIN} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data")
endforeach()
