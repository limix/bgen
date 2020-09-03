# FindZSTD
# --------
#
# Find ZSTANDARD include dirs and libraries
#
# This module reads hints about search locations from variables::
#
#   ZSTD_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   ZSTD_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``ZSTD::zstd``, if
# ZSTD has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   ZSTD_INCLUDE_DIR - Where to find the header files.
#   ZSTD_LIBRARY     - Library when using zstd.
#   ZSTD_FOUND       - True if zstd library is found.


message("Ponto 1")
set(zstd_incl_dirs "/usr/include" "/usr/local/include")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(zstd_incl_dirs ${zstd_incl_dirs} "$ENV{PROGRAMFILES}/zstd/include")
    set(zstd_incl_dirs ${zstd_incl_dirs} "$ENV{PROGRAMDATA}/chocolatey/lib/zstandard/tools/include")
endif()

message("Ponto 2")
if(ZSTD_INCLUDEDIR)
    list(APPEND zstd_incl_dirs ${ZSTD_INCLUDEDIR})
endif()

message("Ponto 3")
find_path(
    ZSTD_INCLUDE_DIR
    NAMES zstd.h
    HINTS ${zstd_incl_dirs}
)

message("Ponto 4")
set(zstd_lib_dirs "/usr/lib" "/usr/local/lib")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(zstd_lib_dirs ${zstd_lib_dirs} "$ENV{PROGRAMFILES}/zstd/lib")
    set(zstd_lib_dirs ${zstd_lib_dirs} "$ENV{PROGRAMDATA}/chocolatey/lib/zstandard/tools/dll")
endif()

message("Ponto 5")
if(ZSTD_LIBRARYDIR)
    list(APPEND zstd_lib_dirs ${ZSTD_LIBRARYDIR})
endif()

message("Ponto 6")
find_library(
    ZSTD_LIBRARY
    NAMES zstd zstdlib libzstd
    HINTS ${zstd_lib_dirs}
)

message("Ponto 7")
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZSTD DEFAULT_MSG
                                  ZSTD_LIBRARY ZSTD_INCLUDE_DIR)

message("Ponto 8")
mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY ZSTD_FOUND)

message("Ponto 9")
set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
set(ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})

message("Ponto 10")
if(ZSTD_FOUND)
    if(NOT TARGET ZSTD::zstd)
        add_library(ZSTD::zstd UNKNOWN IMPORTED)
        set_target_properties(ZSTD::zstd PROPERTIES
            IMPORTED_LOCATION "${ZSTD_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ZSTD_INCLUDE_DIR}")
    endif()
endif()
message("Ponto 11")
