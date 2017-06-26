# FindZSTD
# ---------
#
# Find ZSTANDARD include dirs and libraries
#
# This module reads hints about search locations from variables::
#
#   ZSTD_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   ZSTD_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib

set(_zstd_INCLUDE_SEARCH_DIRS "/usr/include" "/usr/local/include")
if(ZSTD_INCLUDEDIR)
    list(APPEND _zstd_INCLUDE_SEARCH_DIRS ${ZSTD_INCLUDEDIR})

find_path(
    ZSTD_INCLUDE_DIR
    NAMES zstd.h
    HINTS ${_zstd_INCLUDE_SEARCH_DIRS}
)


set(_zstd_LIBRARY_SEARCH_DIRS "/usr/lib" "/usr/local/lib")
if(ZSTD_LIBRARYDIR)
    list(APPEND _zstd_LIBRARY_SEARCH_DIRS ${ZSTD_LIBRARYDIR})

find_library(
    ZSTD_LIBRARY
    NAMES zstd
    HINTS ${_zstd_LIBRARY_SEARCH_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZSTD DEFAULT_MSG
    ZSTD_LIBRARY ZSTD_INCLUDE_DIR)

mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY)

set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
set(ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})

