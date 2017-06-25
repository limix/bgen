# - Try to find zstd
# Once done this will define
#  ZSTD_FOUND - System has zstd
#  ZSTD_INCLUDE_DIRS - The zstd include directories
#  ZSTD_LIBRARIES - The libraries needed to use zstd

find_path(
    ZSTD_INCLUDE_DIR
    NAMES "zstd.h"
    HINTS
        "/usr/local/facebook/include"
        "/usr/local/include"
        "/usr/include"
        "/home/travis/build/limix/bgen/zstd/lib"
)

find_library(
    ZSTD_LIBRARY
    NAMES "zstd"
    HINTS
        "/usr/local/facebook/lib"
        "/usr/local/lib"
        "/usr/lib"
        "/home/travis/build/limix/bgen/zstd/lib"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZSTD DEFAULT_MSG
    ZSTD_LIBRARY ZSTD_INCLUDE_DIR)

mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY)

set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
set(ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})

