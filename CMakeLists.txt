cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

include(cmake/limix.cmake)
limix_initialise()

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
add_definitions(-DTPL_NOLIB)

macro(find_packages)
    find_package(ZLIB REQUIRED)
    include_directories("${ZLIB_INCLUDE_DIRS}")
    
    find_package(ZSTD REQUIRED)
    include_directories("${ZSTD_INCLUDE_DIRS}")
    
    find_package(PROGRESSBAR REQUIRED)
    include_directories("${PROGRESSBAR_INCLUDE_DIRS}")
endmacro(find_packages)

find_packages()

include_directories(src include)
add_subdirectory(src)
add_subdirectory(include)
define_sources(src SOURCES)
define_public_headers(include PUBLIC_HEADERS)

macro(create_libraries)
    set(LIBS "${ZLIB_LIBRARIES};${ZSTD_LIBRARIES};${PROGRESSBAR_LIBRARIES}")
    set(NAME ${PROJECT_NAME})
    set(VERSION ${PROJECT_VERSION})
    limix_library(${NAME} ${VERSION} "${SOURCES}" "${PUBLIC_HEADERS}" "${LIBS}")
endmacro(create_libraries)

create_libraries()

add_subdirectory(test)
enable_testing()
add_test(test_v12_zlib_layout2 test/test_v12_zlib_layout2)
