# FindProgressBar
# ---------
#
# Find PROGRESSBAR include dirs and libraries
#
# This module reads hints about search locations from variables::
#
#   PROGRESSBAR_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
#   PROGRESSBAR_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib

set(progressbar_incl_dirs "/usr/include" "/usr/local/include"
	"C:/Program Files/ProgressBar/include")
if(PROGRESSBAR_INCLUDEDIR)
    list(APPEND progressbar_incl_dirs ${PROGRESSBAR_INCLUDEDIR})
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(progressbar_incl_dirs ${progressbar_incl_dirs} "$ENV{PROGRAMFILES}/progressbar/include")
endif()

find_path(
    PROGRESSBAR_INCLUDE_DIR
    NAMES progressbar/progressbar.h
    HINTS ${progressbar_incl_dirs}
)


set(progressbar_lib_dirs "/usr/lib" "/usr/local/lib"
	"C:/Program Files/ProgressBar/lib")
if(PROGRESSBAR_LIBRARYDIR)
    list(APPEND progressbar_lib_dirs ${PROGRESSBAR_LIBRARYDIR})
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(progressbar_lib_dirs ${progressbar_lib_dirs} "$ENV{PROGRAMFILES}/progressbar/lib")
endif()


find_library(
    PROGRESSBAR_LIBRARY
    NAMES progressbar libprogressbar
    HINTS ${progressbar_lib_dirs}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PROGRESSBAR DEFAULT_MSG
PROGRESSBAR_LIBRARY PROGRESSBAR_INCLUDE_DIR)

mark_as_advanced(PROGRESSBAR_INCLUDE_DIR PROGRESSBAR_LIBRARY PROGRESSBAR_FOUND)

set(PROGRESSBAR_LIBRARIES ${PROGRESSBAR_LIBRARY})
set(PROGRESSBAR_INCLUDE_DIRS ${PROGRESSBAR_INCLUDE_DIR})

if(PROGRESSBAR_FOUND AND NOT PROGRESSBAR_FIND_QUIETLY)
    message(STATUS "PROGRESSBAR: ${PROGRESSBAR_INCLUDE_DIR}")
endif()
