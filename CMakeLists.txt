cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(bgen VERSION 4.1.1 LANGUAGES C)
set(PROJECT_DESCRIPTION "It fully supports the BGEN format specifications 1.2 and 1.3.")

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Set RPATH only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

# merge request fix: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/1799
if(CMAKE_VERSION VERSION_GREATER 3.12.0)
    # Hide things by default
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(ATHR REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ZSTD REQUIRED)

add_library(bgen
    src/file.c
    src/genotype.c
    src/io.c
    src/layout1.c
    src/layout2.c
    src/metafile.c
    src/report.c
    src/samples.c
    src/variant.c
    src/partition.c
    src/bstring.c
    src/zip/zlib.c
    src/zip/zstd.c
)
add_library(BGEN::bgen ALIAS bgen)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(WARNING_FLAGS /W3)
else()
    set(WARNING_FLAGS -Wall -Wextra -pedantic -Wstrict-prototypes -Wshadow
        -Wconversion -Wmissing-prototypes -Wno-unused-parameter -Wsign-conversion
    )
endif()

include(GenerateExportHeader)
generate_export_header(bgen BASE_NAME BGEN
    INCLUDE_GUARD_NAME BGEN_EXPORT_H
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/bgen/export.h)

target_include_directories(bgen
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(bgen PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(bgen PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(bgen PROPERTIES C_STANDARD 99)
target_compile_definitions(bgen PRIVATE $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>)
target_compile_definitions(bgen PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_link_libraries(bgen PUBLIC ATHR::athr)
target_link_libraries(bgen PUBLIC ZLIB::ZLIB)
target_link_libraries(bgen PUBLIC ZSTD::zstd)
target_compile_options(bgen PRIVATE ${WARNING_FLAGS})

if (NOT c_restrict IN_LIST CMAKE_C_COMPILE_FEATURES)
    message(WARNING "restrict feature is not supported")
    target_compile_definitions(bgen PUBLIC restrict=)
endif()

include(CheckCSourceCompiles)
check_c_source_compiles(
    "
        static void f(char const* err, ...) __attribute__((format(printf, 1, 2)));
        int main(void) {return 0;}
    "
    HAVE_ATTR_FORMAT
)
target_compile_definitions(bgen PRIVATE $<$<BOOL:${HAVE_ATTR_FORMAT}>:HAVE_ATTR_FORMAT>)

include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    message(FATAL_ERROR "Sorry, big-endian platforms are not supported yet.")
endif()

install(TARGETS bgen EXPORT bgen-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bgen/export.h
    DESTINATION include/bgen
)
install(EXPORT bgen-targets
    FILE bgen-targets.cmake
    NAMESPACE BGEN::
    DESTINATION lib/cmake/bgen
)
include(CMakePackageConfigHelpers)
configure_package_config_file(bgen-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/bgen-config.cmake
    INSTALL_DESTINATION lib/cmake/bgen
    NO_SET_AND_CHECK_MACRO
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bgen-config.cmake
    DESTINATION lib/cmake/bgen
)

enable_testing()
add_subdirectory(test)
