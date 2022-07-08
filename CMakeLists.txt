cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(
  bgen
  VERSION 4.1.9
  LANGUAGES C)
set(PROJECT_DESCRIPTION
    "It fully supports the BGEN format specifications 1.2 and 1.3.")

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)

cpmaddpackage(NAME athr VERSION 4.0.3 GITHUB_REPOSITORY horta/almosthere)
cpmaddpackage(
  NAME
  zstd
  GITHUB_REPOSITORY
  facebook/zstd
  VERSION
  1.5.2
  SOURCE_SUBDIR
  build/cmake
  OPTIONS
  "ZSTD_LEGACY_SUPPORT OFF")
cpmaddpackage(NAME zlib VERSION 1.2.12.2 GITHUB_REPOSITORY horta/zlib)

if(zstd_ADDED)
  if(NOT TARGET ZSTD::zstd)
    add_library(ZSTD::zstd ALIAS libzstd_static)
    set_target_properties(
      libzstd_static PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                "$<BUILD_INTERFACE:${zstd_SOURCE_DIR}/lib>")
  endif()
endif()

add_library(
  bgen
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
  src/unzlib.c
  src/unzstd.c)
add_library(BGEN::bgen ALIAS bgen)

include(GenerateExportHeader)
generate_export_header(
  bgen
  BASE_NAME
  BGEN
  INCLUDE_GUARD_NAME
  BGEN_EXPORT_H
  EXPORT_FILE_NAME
  ${CMAKE_CURRENT_BINARY_DIR}/bgen/export.h)

target_include_directories(
  bgen
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(bgen PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(bgen PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(bgen PROPERTIES C_STANDARD 99)
target_compile_definitions(bgen
                           PRIVATE $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>)
target_compile_definitions(
  bgen PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_link_libraries(bgen PUBLIC ATHR::athr)
target_link_libraries(bgen PUBLIC ZLIB::zlibstatic)
target_link_libraries(bgen PUBLIC ZSTD::zstd)
target_compile_options(bgen PRIVATE ${WARNING_FLAGS})

if(NOT c_restrict IN_LIST CMAKE_C_COMPILE_FEATURES)
  message(WARNING "restrict feature is not supported")
  target_compile_definitions(bgen PUBLIC restrict=)
endif()

include(CheckCSourceCompiles)
check_c_source_compiles(
  "
        static void f(char const* err, ...) __attribute__((format(printf, 1, 2)));
        int main(void) {return 0;}
  "
  HAVE_ATTR_FORMAT)
target_compile_definitions(
  bgen PRIVATE $<$<BOOL:${HAVE_ATTR_FORMAT}>:HAVE_ATTR_FORMAT>)

include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
  message(FATAL_ERROR "Sorry, big-endian platforms are not supported yet.")
endif()

install(
  TARGETS bgen
  EXPORT bgen-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include)

install(DIRECTORY include/ DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bgen/export.h
        DESTINATION include/bgen)
install(
  EXPORT bgen-targets
  FILE bgen-targets.cmake
  NAMESPACE BGEN::
  DESTINATION lib/cmake/bgen)
include(CMakePackageConfigHelpers)
configure_package_config_file(
  bgen-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/bgen-config.cmake
  INSTALL_DESTINATION lib/cmake/bgen
  NO_SET_AND_CHECK_MACRO)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bgen-config.cmake
        DESTINATION lib/cmake/bgen)

enable_testing()
add_subdirectory(test)

set(CPACK_PACKAGE_NAME bgen)
set(CPACK_PACKAGE_VENDOR "Danilo Horta")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "It fully supports the BGEN format specifications 1.2 and 1.3")
set(CPACK_PACKAGE_CONTACT "Danilo Horta")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/package)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_RELOCATABLE YES)
set(CPACK_MONOLITHIC_INSTALL YES)
include(CPack)
