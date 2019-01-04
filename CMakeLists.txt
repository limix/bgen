cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
include(cmake/limix.cmake)
limix_initialise()
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C)
limix_config()

set(CMAKE_C_STANDARD 11)
add_definitions(-DTPL_NOLIB)

find_package(ZLIB REQUIRED)
include_directories("${ZLIB_INCLUDE_DIRS}")

find_package(ZSTD REQUIRED)
include_directories("${ZSTD_INCLUDE_DIRS}")

find_package(ATHR REQUIRED)
include_directories("${ATHR_INCLUDE_DIRS}")

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wno-pedantic -Werror -Wno-missing-field-initializers -Wno-deprecated-declarations)
endif(NOT MSVC)

limix_process_default_dirs()

limix_add_library(${PROJECT_NAME} ${PROJECT_VERSION} "${SOURCES}" "${HEADERS}"
"${PRIVATE_HEADERS}" "${ZLIB_LIBRARIES};${ZSTD_LIBRARIES};${ATHR_LIBRARIES}")

add_subdirectory(test)
