cmake_minimum_required (VERSION 2.6.4)

project(bgen C)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)

  if (GCC_VERSION VERSION_GREATER 4.9)
    set (CMAKE_C_STANDARD 11)
  else ()
    set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
  endif ()
endif ()

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-Dinline=__inline)
endif()

file(STRINGS "VERSION" BGEN_VERSION)

find_package( ZLIB REQUIRED )
if(NOT ZLIB_FOUND)
    message(STATUS "Could not find zlib library. If you have it installed,
                    please, define ZLIB_LIBRARYDIR and ZLIB_INCLUDEDIR
                    appropriately to fix it.")
endif()


find_package( ZSTD REQUIRED )

include_directories( ${ZLIB_INCLUDE_DIRS} )
include_directories( ${ZSTD_INCLUDE_DIRS} )
include_directories( include/ )
include_directories( src/ )

add_subdirectory(src)

add_library(bgen SHARED
    src/util/mem.c
    src/util/string.c
    src/util/choose.c
    src/util/file.c
    src/variants_index.c
    src/layout/two.c
    src/layout/one.c
    src/tpl/tpl.c
    src/zip/zstd_wrapper.c
    src/zip/zlib_wrapper.c
    src/bgen.c)

add_library(bgen_static STATIC
    src/util/mem.c
    src/util/string.c
    src/util/choose.c
    src/util/file.c
    src/variants_index.c
    src/layout/two.c
    src/layout/one.c
    src/tpl/tpl.c
    src/zip/zstd_wrapper.c
    src/zip/zlib_wrapper.c
    src/bgen.c)

set_target_properties(bgen
  PROPERTIES
    VERSION ${BGEN_VERSION}
    PUBLIC_HEADER
        "include/bgen/bgen.h;include/bgen/number.h;include/bgen/paul_stdint.h;include/bgen/stdint.h;include/bgen/string.h")

install(TARGETS bgen
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
    PUBLIC_HEADER DESTINATION include/bgen)
target_link_libraries( bgen ${ZLIB_LIBRARIES} ${ZSTD_LIBRARIES})

install(TARGETS bgen_static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/bgen)
target_link_libraries(bgen_static ${ZLIB_LIBRARIES} ${ZSTD_LIBRARIES})

add_subdirectory(test)

enable_testing()
#add_test(test_bgen test/test_bgen)
add_test(test_v12_zlib_layout2 test/test_v12_zlib_layout2)
add_test(test_store test/test_store)
