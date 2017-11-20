# FindOMEM
# ---------
#
# Find ZSTANDARD include dirs and libraries
#
# This module reads hints about search locations from variables::
#
#   OMEM_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   OMEM_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib

set(omem_incl_dirs "/usr/include" "/usr/local/include")
if(OMEM_INCLUDEDIR)
    list(APPEND omem_incl_dirs ${OMEM_INCLUDEDIR})
endif()

find_path(
    OMEM_INCLUDE_DIR
    NAMES omem/omem.h
    HINTS ${omem_incl_dirs}
)


set(omem_lib_dirs "/usr/lib" "/usr/local/lib")
if(OMEM_LIBRARYDIR)
    list(APPEND omem_lib_dirs ${OMEM_LIBRARYDIR})
endif()

find_library(
    OMEM_LIBRARY
    NAMES omem libomem
    HINTS ${omem_lib_dirs}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OMEM DEFAULT_MSG
                                  OMEM_LIBRARY OMEM_INCLUDE_DIR)

mark_as_advanced(OMEM_INCLUDE_DIR OMEM_LIBRARY OMEM_FOUND)

set(OMEM_LIBRARIES ${OMEM_LIBRARY})
set(OMEM_INCLUDE_DIRS ${OMEM_INCLUDE_DIR})

if(OMEM_FOUND AND NOT OMEM_FIND_QUIETLY)
    message(STATUS "OMEM: ${OMEM_INCLUDE_DIR}")
endif()
