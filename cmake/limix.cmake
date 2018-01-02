# limix 
# ---------
#
# Common configuration and handy functions for limix projects.
#
# This module provides the folloing::
#
#   limix_initialise - Called at the start of the main CMakelists.txt
#   limix_library - Create static and shared library in one call
include(GNUInstallDirs)

function(display_welcome)
    foreach(MSG ${WELCOME})
        string(REPLACE "{NAME}" ${PROJECT_NAME} MSG ${MSG})
        string(REPLACE "{VERSION}" ${PROJECT_VERSION} MSG ${MSG})
        string(REPLACE "{GITHUB_URL}" ${GITHUB_URL} MSG ${MSG})
        message(${MSG})
    endforeach()
endfunction(display_welcome)

macro(limix_config)
    enable_testing()
    
    if (NOT CMAKE_BUILD_TYPE)
        set(MSG "CMAKE_BUILD_TYPE has not been set.")
        set(MSG "${MSG} Using the default value \"Release\".")
        message(STATUS "${MSG}")
        set(CMAKE_BUILD_TYPE Release)
    endif()

    macro(set_rpath)
        set(CMAKE_MACOSX_RPATH TRUE)
        set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR FALSE)
    endmacro(set_rpath)

    set_rpath()

    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

    # Windows specific common configuration
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
      add_definitions(-Dinline=__inline)
    endif()
endmacro(limix_config)

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

function(easy_install TARGET_NAME INCLUDE_DIR)
    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${INCLUDE_DIR})
endfunction(easy_install)

function(add_library_type TYPE NAME VERSION SOURCES PUBLIC_HEADERS LIBS)
    if (TYPE MATCHES "STATIC")
        set(NAME "${NAME}_static")
    endif()
    add_library(${NAME} ${TYPE} "${SOURCES}")

    easy_set_target_property(${NAME} VERSION ${VERSION})
    if (TYPE MATCHES "SHARED")
        easy_set_target_property(${NAME} PUBLIC_HEADER "${PUBLIC_HEADERS}")
    endif()


    if (TYPE MATCHES "SHARED")
        easy_install(${NAME} include/${NAME})
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
    string(REPLACE ";" "\\;" MYPATH "${MYPATH}")

    set_property(TEST test_${NAME} APPEND PROPERTY ENVIRONMENT
              "PATH=${MYPATH}")
endmacro(limix_add_test)
