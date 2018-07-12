# limix cmake module (1.0.3)
#
# Common configuration and handy functions for limix projects.
#
# This module provides the following::
#
# * limix_initialise - Called at the start of the main CMakelists.txt
# * limix_library - Create static and shared library in one call
include(GNUInstallDirs)

function(display_welcome)
  foreach(MSG ${WELCOME})
    string(REPLACE "{NAME}" ${PROJECT_NAME} MSG ${MSG})
    string(REPLACE "{VERSION}" ${PROJECT_VERSION} MSG ${MSG})
    string(REPLACE "{GITHUB_URL}" ${GITHUB_URL} MSG ${MSG})
    message(${MSG})
  endforeach()
endfunction(display_welcome)

macro(limix_windows_config)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-Dinline=__inline)

  set(CompilerFlags
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE)
endmacro(limix_windows_config)

macro(limix_config)
  enable_testing()

  if(NOT CMAKE_BUILD_TYPE)
    set(MSG "CMAKE_BUILD_TYPE has not been set.")
    set(MSG "${MSG} Using the default value \"Release\".")
    message(STATUS "${MSG}")
    set(CMAKE_BUILD_TYPE Release)
  endif()

  macro(set_rpath)
    set(CMAKE_MACOSX_RPATH TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR FALSE)
    if (NOT DEFINED CMAKE_INSTALL_RPATH)
        if (DEFINED CMAKE_INSTALL_PREFIX)
          set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
        endif()
    endif()
  endmacro(set_rpath)

  set_rpath()

  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

  # Windows specific common configuration
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    limix_windows_config()
  endif()

  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
endmacro(limix_config)

macro(limix_process_default_dirs)
  include_directories(src include)
  add_subdirectory(src)
  add_subdirectory(include)

  define_sources(src SOURCES)
  define_public_headers(include HEADERS)
endmacro(limix_process_default_dirs)

macro(limix_initialise)
  file(STRINGS ${CMAKE_HOME_DIRECTORY}/NAME PROJECT_NAME)
  file(STRINGS ${CMAKE_HOME_DIRECTORY}/VERSION PROJECT_VERSION)
  file(STRINGS ${CMAKE_HOME_DIRECTORY}/WELCOME WELCOME)
  file(STRINGS ${CMAKE_HOME_DIRECTORY}/GITHUB_URL GITHUB_URL)
  display_welcome()
endmacro(limix_initialise)

function(easy_set_target_property NAME PROPERTY VALUE)
  set_target_properties(${NAME} PROPERTIES ${PROPERTY} "${VALUE}")
endfunction(easy_set_target_property)

function(easy_shared_install TARGET_NAME)
  install(TARGETS ${TARGET_NAME}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endfunction(easy_shared_install)

function(easy_static_install TARGET_NAME)
  install(TARGETS ${TARGET_NAME}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endfunction(easy_static_install)

function(add_library_type TYPE NAME VERSION SOURCES PUBLIC_HEADERS LIBS)
  if(TYPE MATCHES "STATIC")
    set(NAME "${NAME}_static")
  endif()
  add_library(${NAME} ${TYPE} "${SOURCES}")

  easy_set_target_property(${NAME} VERSION ${VERSION})
  if(TYPE MATCHES "SHARED")
    easy_set_target_property(${NAME} PUBLIC_HEADER "${PUBLIC_HEADERS}")
    
    string(REPLACE "." ";" VERSION_LIST ${VERSION})
    list(GET VERSION_LIST 0 VERSION_MAJOR)
    easy_set_target_property(${NAME} SOVERSION ${VERSION_MAJOR})
    
    easy_shared_install(${NAME})
  else()
    easy_static_install(${NAME})
  endif()

  target_link_libraries(${NAME} "${LIBS}")
endfunction(add_library_type)

function(limix_add_library NAME VERSION SRCS HDRS LIBS)
  add_library_type("SHARED" ${NAME} ${VERSION} "${SRCS}" "${HDRS}" "${LIBS}")
  add_library_type("STATIC" ${NAME} ${VERSION} "${SRCS}" "${HDRS}" "${LIBS}")
endfunction(limix_add_library)

macro(limix_add_test NAME LIBRARY SOURCES)
  add_executable(${NAME} ${SOURCES})
  target_link_libraries(${NAME} ${LIBRARY})
  add_test(test_${NAME} ${NAME} -E environment)
  file(TO_CMAKE_PATH "$ENV{PATH}" MYPATH)

  list(APPEND MYPATH ${CMAKE_BINARY_DIR})
  list(APPEND MYPATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  string(REPLACE ";" "\\;" MYPATH "${MYPATH}")

  set_property(TEST test_${NAME} APPEND PROPERTY ENVIRONMENT "PATH=${MYPATH}")
endmacro(limix_add_test)

function(limix_convert_rel_to_full _FULL BASE_DIR REL)
  foreach(SRC ${REL})
    list(APPEND FULL "${BASE_DIR}/${SRC}")
  endforeach()
  set(${_FULL} ${FULL} PARENT_SCOPE)
endfunction(limix_convert_rel_to_full)
