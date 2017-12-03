cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
include(limix.cmake)

define_standard_variables()
project(bgen VERSION ${PROJECT_VERSION} LANGUAGES C)

display_welcome()

message(WARNING "-- TODO: convert gnu version check into feature requirement")
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)

  if (GCC_VERSION VERSION_GREATER 4.9)
    set (CMAKE_C_STANDARD 11)
  else ()
    set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
  endif ()
endif ()

# Common configuration
set(CMAKE_MACOSX_RPATH TRUE)
if (NOT CMAKE_BUILD_TYPE)
    set(MSG "CMAKE_BUILD_TYPE has not been set.")
    set(MSG "${MSG} Using the default value \"Release\".")
    message(STATUS "${MSG}")
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Windows specific common configuration
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-Dinline=__inline)
endif()

add_definitions(-DTPL_NOLIB)

find_package(ZLIB REQUIRED)
find_package(ZSTD REQUIRED )
find_package(PROGRESSBAR REQUIRED)

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${ZSTD_INCLUDE_DIRS})
include_directories(${PROGRESSBAR_INCLUDE_DIRS})
include_directories(include)
include_directories(src)

add_subdirectory(src)
define_sources(src SOURCES)

add_subdirectory(include)
define_public_headers(include PUBLIC_HEADERS)

add_library(bgen SHARED ${SOURCES})
add_library(bgen_static STATIC ${SOURCES})

set_target_properties(bgen
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${PUBLIC_HEADERS}")

install(TARGETS bgen
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION include/bgen)
target_link_libraries(bgen
    ${ZLIB_LIBRARIES}
    ${ZSTD_LIBRARIES}
    ${PROGRESSBAR_LIBRARIES})

install(TARGETS bgen_static
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION include/bgen)
target_link_libraries(bgen_static
    ${ZLIB_LIBRARIES}
    ${ZSTD_LIBRARIES}
    ${PROGRESSBAR_LIBRARIES})

add_subdirectory(test)

enable_testing()
add_test(test_v12_zlib_layout2 test/test_v12_zlib_layout2)
