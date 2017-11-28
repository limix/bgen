include_directories (${TEST_SOURCE_DIR}/src)

set(LLIBS bgen_static ${ZLIB_LIBRARIES} ${ZSTD_LIBRARIES} ${PROGRESSBAR_LIBRARIES})

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(LLIBS ${LLIBS} m)
endif()

add_executable (test_v12_zlib_layout2 test_v12_zlib_layout2.c example_files.c)
target_link_libraries (test_v12_zlib_layout2 ${LLIBS})

configure_file(data/example.matrix ${CMAKE_CURRENT_BINARY_DIR}/data/example.matrix COPYONLY)

configure_file(data/example.1bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.1bits.bgen COPYONLY)

configure_file(data/example.14bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.14bits.bgen COPYONLY)

configure_file(data/example.32bits.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.32bits.bgen COPYONLY)

configure_file(data/example.v11.bgen ${CMAKE_CURRENT_BINARY_DIR}/data/example.v11.bgen COPYONLY)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	foreach(FILE ${ZSTD_LIBRARIES} ${PROGRESSBAR_LIBRARIES})

		string(REPLACE ".lib" ".dll" DLL_FILE "${FILE}")

		add_custom_command(TARGET test_v12_zlib_layout2 POST_BUILD
	    	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	            ${DLL_FILE}
	        	$<TARGET_FILE_DIR:test_v12_zlib_layout2>)

		message(${DLL_FILE})
		get_filename_component(FNAME ${DLL_FILE} NAME)

		message(${FNAME})
		string(REPLACE "lib" "" TRIMMED_FNAME "${FNAME}")

		message(${TRIMMED_FNAME})

		add_custom_command(TARGET test_v12_zlib_layout2 POST_BUILD
	    	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	            ${DLL_FILE}
	        	$<TARGET_FILE_DIR:test_v12_zlib_layout2>/${TRIMMED_FNAME})

		if(${TRIMMED_FNAME} MATCHES "zstd.dll")
			add_custom_command(TARGET test_v12_zlib_layout2 POST_BUILD
	    	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	            ${DLL_FILE}
	        	"$<TARGET_FILE_DIR:test_v12_zlib_layout2>/zstdlib.dll")
		endif()
		# add_custom_command(TARGET test_v12_zlib_layout2 POST_BUILD
	 #    	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	 #            ${DLL_FILE}
	 #        	$<TARGET_FILE_DIR:test_v12_zlib_layout2>)
	endforeach()
endif()
