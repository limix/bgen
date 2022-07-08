function(bgen_add_test name)
  set(target test_${name})
  add_executable(${target} src/${name}.c)
  target_link_libraries(${target} PRIVATE BGEN::bgen)
  target_compile_options(${target} PRIVATE ${WARNING_FLAGS})
  target_compile_definitions(
    ${target} PRIVATE $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>)
  target_compile_definitions(
    ${target} PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)

  set(datadir "${CMAKE_CURRENT_BINARY_DIR}/data/")
  target_compile_definitions(${target} PUBLIC "TEST_DATADIR=\"${datadir}\"")

  add_test(
    NAME ${name}
    COMMAND ${target} -E environment
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

  add_custom_command(
    TARGET ${target}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/${name}.tmp")

  # Ugly hacky to make the tests work on Windows.
  file(TO_CMAKE_PATH "$ENV{PATH}" PATH)
  list(APPEND PATH $<TARGET_FILE_DIR:ZLIB::zlib>/../bin)
  list(APPEND PATH $<TARGET_FILE_DIR:ZSTD::zstd>/../bin)
  list(APPEND PATH $<TARGET_FILE_DIR:ATHR::athr>)
  list(APPEND PATH $<TARGET_FILE_DIR:BGEN::bgen>)
  string(REPLACE ";" "\\;" PATH "${PATH}")
  set_tests_properties(${name} PROPERTIES ENVIRONMENT "PATH=${PATH}")

endfunction()

function(bgen_copy filename)
  file(COPY data/${filename} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data")
endfunction()

bgen_add_test(v12_zlib_layout2)
bgen_add_test(zero_len_chrom_id)
bgen_add_test(pbgen_example)
bgen_add_test(metadata_example)
bgen_add_test(haplotype)
bgen_add_test(complex)
bgen_add_test(index)
bgen_add_test(open_genotype)
bgen_add_test(assert_interface_2)
bgen_add_test(assert_interface_3)
bgen_add_test(assert_interface_3_complex)
bgen_add_test(wrong_bgen_files)
bgen_add_test(wrong_metadata_files)
bgen_add_test(variant_position_overflow)
bgen_add_test(one_million)
bgen_add_test(create_metafile)

bgen_copy(example.matrix)
bgen_copy(example.1bits.bgen)
bgen_copy(example.1bits.bgen.metafile)
bgen_copy(example.14bits.bgen)
bgen_copy(example.14bits.bgen.metafile)
bgen_copy(example.14bits.bgen.metafile.truncated)
bgen_copy(example.14bits.bgen.truncated)
bgen_copy(example.32bits.bgen)
bgen_copy(example.32bits.bgen.metafile)
bgen_copy(example.v11.bgen)

bgen_copy(pbgen_example.pgen)
bgen_copy(zero_len_chrom_id.bgen)
bgen_copy(wrong.metafile)
bgen_copy(haplotypes.bgen)
bgen_copy(complex.23bits.bgen)

bgen_copy(random.bgen)
bgen_copy(random.bgen.metafile)

bgen_copy(roundtrip1.bgen)
