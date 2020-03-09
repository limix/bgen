cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(bgen VERSION 4.0.0 LANGUAGES C)
set(PROJECT_DESCRIPTION "It fully supports the BGEN format specifications 1.2 and 1.3.")

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Hide things by default
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(ATHR REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ZSTD REQUIRED)

add_library(bgen
    src/file.c
    src/geno.c
    src/io.c
    src/layout_one.c
    src/layout_two.c
    src/metafile.c
    src/report.c
    src/samples.c
    src/str.c
    src/variant.c
    src/zip/zlib.c
    src/zip/zstd.c
)
add_library(BGEN::bgen ALIAS bgen)

# Write out mytools_export.h to the current binary directory
# Must include ${CMAKE_CURRENT_BINARY_DIR} in the target's
# search path.
include(GenerateExportHeader)
generate_export_header(bgen)

target_include_directories(bgen
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(bgen PROPERTIES PUBLIC_HEADER include/bgen.h)
set_target_properties(bgen PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(bgen PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(bgen PROPERTIES C_STANDARD 99)
set_target_properties(bgen PROPERTIES DEFINE_SYMBOL "BGEN_EXPORTS")
target_link_libraries(bgen PUBLIC ATHR::athr)
target_link_libraries(bgen PUBLIC ZLIB::ZLIB)
target_link_libraries(bgen PUBLIC ZSTD::zstd)
target_compile_options(bgen PRIVATE
    $<$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>:/W3>
    $<$<NOT:$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>>:-Wall -Wextra -pedantic
    -Wstrict-prototypes -Wshadow -Wconversion -Wmissing-prototypes
    -Wno-unused-parameter>
)
target_compile_features(bgen PRIVATE c_restrict)

include(CheckCSourceCompiles)
check_c_source_compiles(
    "
        static void f(char const* err, ...) __attribute__((format(printf, 1, 2)));
        int main(void) {return 0;}
    "
    HAVE_ATTR_FORMAT
)
target_compile_definitions(bgen PRIVATE $<$<BOOL:${HAVE_ATTR_FORMAT}>:HAVE_ATTR_FORMAT>)

install(TARGETS bgen EXPORT bgen-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

enable_testing()
add_subdirectory(test)
