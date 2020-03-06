cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)
project(bgen VERSION 3.0.4 LANGUAGES C)
set(PROJECT_DESCRIPTION "It fully supports the BGEN format specifications 1.2 and 1.3.")

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(ATHR REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ZSTD REQUIRED)

add_library(bgen
    src/file.c
    src/geno.c
    src/index.c
    src/io.c
    src/layout/one.c
    src/layout/two.c
    src/metafile.c
    src/str.c
    src/variant.c
    src/zip/zlib.c
    src/zip/zstd.c
)
add_library(BGEN::bgen ALIAS bgen)


target_include_directories(bgen
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
    $<$<NOT:$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>>:-Wall -Wextra -pedantic -Wstrict-prototypes
        -Wshadow -Wconversion -Wmissing-prototypes -Wno-unused-parameter>
)
target_compile_features(bgen PRIVATE c_restrict)


install(TARGETS bgen EXPORT bgen-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

enable_testing()
add_subdirectory(test)

# include(cmake/limix.cmake)
# limix_initialise()
# project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C)
# limix_config()

# set(CMAKE_C_STANDARD 11)
# add_definitions(-DTPL_NOLIB)

# find_package(ZLIB REQUIRED)
# include_directories("${ZLIB_INCLUDE_DIRS}")

# find_package(ZSTD REQUIRED)
# include_directories("${ZSTD_INCLUDE_DIRS}")

# find_package(ATHR REQUIRED)
# include_directories("${ATHR_INCLUDE_DIRS}")

# if (NOT c_restrict IN_LIST CMAKE_C_COMPILE_FEATURES)
#     add_compile_definitions(restrict= )
# endif()

# if(NOT MSVC)
#     add_compile_options(-Wall -Wextra -Wno-pedantic -Werror -Wno-missing-field-initializers -Wno-deprecated-declarations)
# endif(NOT MSVC)

# limix_process_default_dirs()

# limix_add_library(${PROJECT_NAME} ${PROJECT_VERSION} "${SOURCES}" "${HEADERS}"
# "${PRIVATE_HEADERS}" "${ZLIB_LIBRARIES};${ZSTD_LIBRARIES};${ATHR_LIBRARIES}")

# add_subdirectory(test)
