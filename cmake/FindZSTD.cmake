# FindZSTD
# ---------
#
# Find ZSTANDARD include dirs and libraries
#
# This module reads hints about search locations from variables::
#
#   ZSTD_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   ZSTD_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib

set(zstd_incl_dirs "/usr/include" "/usr/local/include")
if(ZSTD_INCLUDEDIR)
    list(APPEND zstd_incl_dirs ${ZSTD_INCLUDEDIR})
endif()

find_path(
    ZSTD_INCLUDE_DIR
    NAMES zstd.h
    HINTS ${zstd_incl_dirs}
)


set(zstd_lib_dirs "/usr/lib" "/usr/local/lib")
if(ZSTD_LIBRARYDIR)
    list(APPEND zstd_lib_dirs ${ZSTD_LIBRARYDIR})
endif()

find_library(
    ZSTD_LIBRARY
    NAMES zstd libzstd
    HINTS ${zstd_lib_dirs}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZSTD DEFAULT_MSG
                                  ZSTD_LIBRARY ZSTD_INCLUDE_DIR)

mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY ZSTD_FOUND)

set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
set(ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})

if(ZSTD_FOUND AND NOT ZSTD_FIND_QUIETLY)
    message(STATUS "ZSTD: ${ZSTD_INCLUDE_DIR}")
endif()
