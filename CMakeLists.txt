cmake_minimum_required (VERSION 2.8)

project(bgen C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

file(STRINGS "VERSION" BGEN_VERSION)

find_package( ZLIB REQUIRED )

include_directories( ${ZLIB_INCLUDE_DIRS} )
include_directories( include/ )
include_directories(.)

add_subdirectory(bgen)

add_library(bgen SHARED bgen/bgen.c bgen/layout2.c bgen/util/choose.c bgen/util/mem.c)

add_library(bgen_static STATIC bgen/bgen.c bgen/layout2.c bgen/util/choose.c bgen/util/mem.c)

set_target_properties(bgen
  PROPERTIES
    VERSION ${BGEN_VERSION}
    PUBLIC_HEADER include/bgen/bgen.h)

install(TARGETS bgen
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
    PUBLIC_HEADER DESTINATION include/bgen)
target_link_libraries( bgen ${ZLIB_LIBRARIES} )

install(TARGETS bgen_static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/bgen)
target_link_libraries( bgen_static ${ZLIB_LIBRARIES} )

add_subdirectory(test)

enable_testing()
add_test(NAME TestBgen COMMAND test_bgen)
