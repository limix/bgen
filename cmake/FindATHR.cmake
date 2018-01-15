# FindATHR
# ---------
#
# Find ATHR include dirs and libraries
#
# This module reads hints about search locations from variables::
#
#   ATHR_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   ATHR_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib

set(athr_incl_dirs "/usr/include" "/usr/local/include")

if(ATHR_INCLUDEDIR)
    list(APPEND athr_incl_dirs ${ATHR_INCLUDEDIR})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    list(APPEND athr_incl_dirs "$ENV{PROGRAMFILES}/athr/include")
endif()

find_path(
    ATHR_INCLUDE_DIR
    NAMES athr.h
    HINTS ${athr_incl_dirs}
)

set(athr_lib_dirs "/usr/lib" "/usr/local/lib")

if(ATHR_LIBRARYDIR)
    list(APPEND athr_lib_dirs ${ATHR_LIBRARYDIR})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    list(APPEND athr_lib_dirs "$ENV{PROGRAMFILES}/athr/lib")
endif()

find_library(
    ATHR_LIBRARY
    NAMES athr libathr
    HINTS ${athr_lib_dirs}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ATHR DEFAULT_MSG
ATHR_LIBRARY ATHR_INCLUDE_DIR)

mark_as_advanced(ATHR_INCLUDE_DIR ATHR_LIBRARY ATHR_FOUND)

set(ATHR_LIBRARIES ${ATHR_LIBRARY})
set(ATHR_INCLUDE_DIRS ${ATHR_INCLUDE_DIR})
