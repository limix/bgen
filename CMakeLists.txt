cmake_minimum_required (VERSION 3.2.2)

project(bgen_reader C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

file(STRINGS "VERSION" BGEN_READER_VERSION)

find_package( ZLIB REQUIRED )

include_directories( ${ZLIB_INCLUDE_DIRS} )
include_directories(.)

add_subdirectory(bgen_reader)

add_library(bgen_reader SHARED $<TARGET_OBJECTS:bgen_reader_objs>)

add_library(bgen_reader_static STATIC $<TARGET_OBJECTS:bgen_reader_objs>)

set_target_properties(bgen_reader
  PROPERTIES
    VERSION ${BGEN_READER_VERSION}
    PUBLIC_HEADER "bgen_reader/bgen_reader.h")

install(TARGETS bgen_reader
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
        PUBLIC_HEADER DESTINATION include/bgen_reader)
target_link_libraries( bgen_reader ${ZLIB_LIBRARIES} )

install(TARGETS bgen_reader_static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/bgen_reader)
target_link_libraries( bgen_reader_static ${ZLIB_LIBRARIES} )

add_subdirectory(test)

enable_testing()
add_test(NAME TestBgenReader COMMAND test_bgen_reader)
