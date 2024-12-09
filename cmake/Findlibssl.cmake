# - Try to find glog
# Once done, this will define
#
#  libssl_FOUND - system has libssl installed
#  libssl_INCLUDE_DIRS - the libssl include directories
#  libssl_LIBRARIES - link these to use libssl
#
# The user may wish to set, in the CMake GUI or otherwise, this variable:
#  libssl_DIR - path to start searching for the module

SET(libssl_DIR
        "${libssl_DIR}"
        CACHE
        PATH
        "Where to start looking for this component.")

UNSET(libssl_INCLUDE_DIR CACHE)
UNSET(libssl_LIBRARY CACHE)

IF (WIN32)
    FIND_PATH(libssl_INCLUDE_DIR
            NAMES
            openssl/ssl.h
            PATHS
            "."
            HINTS
            ${libssl_DIR}
            PATH_SUFFIXES
            include)

    FIND_LIBRARY(libssl_LIBRARY
            NAMES
            ssl
            PATHS
            "."
            HINTS
            ${libssl_DIR}
            PATH_SUFFIXES
            lib64
            lib
            x86_64-linux-gnu)

ELSE ()
    FIND_PATH(libssl_INCLUDE_DIR
            NAMES
            openssl/ssl.h
            PATHS
            "/usr"
            "/usr/local"
            HINTS
            ${libssl_DIR}
            PATH_SUFFIXES
            include)

    FIND_LIBRARY(libssl_LIBRARY
            NAMES
            ssl
            PATHS
            "/usr"
            "/usr/lib"
            "/usr/lib64"
            "/usr/local/lib"
            "/usr/local/lib64"
            HINTS
            ${libssl_DIR}
            PATH_SUFFIXES
            lib64
            lib
            x86_64-linux-gnu)
ENDIF ()

MARK_AS_ADVANCED(libssl_INCLUDE_DIR
        libssl_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libssl
        DEFAULT_MSG
        libssl_INCLUDE_DIR
        libssl_LIBRARY)

IF (libssl_FOUND)
    ADD_DEFINITIONS(-DGLOG_USE_GLOG_EXPORT)
    ADD_DEFINITIONS(-DGLOG_NO_ABBREVIATED_SEVERITIES)
    SET(libssl_INCLUDE_DIRS
            "${libssl_INCLUDE_DIR}")
    # Add any dependencies here
    SET(libssl_LIBRARIES
            "${libssl_LIBRARY}")
    # Add any dependencies here
    MARK_AS_ADVANCED(libssl_DIR)
ENDIF ()
