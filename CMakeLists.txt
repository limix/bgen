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
endif()

file(STRINGS "VERSION" BGEN_VERSION)

find_package( ZLIB REQUIRED )
if(NOT ZLIB_FOUND)
    message(STATUS "Could not find zlib library. If you have it installed,
                    please, define ZLIB_LIBRARYDIR and ZLIB_INCLUDEDIR
                    appropriately to fix it.")
endif()


find_package( ZSTD REQUIRED )
if(NOT ZSTD_FOUND)
    message(STATUS "Could not find zstd library. If you have it installed,
                    please, define ZSTD_LIBRARYDIR and ZSTD_INCLUDEDIR
                    appropriately to fix it.")
endif()

include_directories( ${ZLIB_INCLUDE_DIRS} )
include_directories( ${ZSTD_INCLUDE_DIRS} )
include_directories( include/ )
include_directories(.)

add_subdirectory(bgen)

add_library(bgen SHARED bgen/bgen.c bgen/layout1.c bgen/layout2.c bgen/util/choose.c bgen/util/mem.c)

add_library(bgen_static STATIC bgen/bgen.c bgen/layout1.c bgen/layout2.c bgen/util/choose.c bgen/util/mem.c)

set_target_properties(bgen
  PROPERTIES
    VERSION ${BGEN_VERSION}
    PUBLIC_HEADER include/bgen/bgen.h)

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
add_test(test_bgen test/test_bgen)
