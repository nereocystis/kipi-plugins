# Find HUpnp 2.0 - HUpnp is a Universal Plug and Play (UPnP) library
# used by the UPnP collection.
#
# This module defines
# HUPNP_INCLUDE_DIR
# HUPNP_LIBS
# HUPNP_FOUND

FIND_PATH(HUPNP_INCLUDE_DIR
          HUpnp HINTS
          ${PC_HUPNP_INCLUDE_DIR}
          /usr/include/HUpnpCore/
         )

FIND_PATH(HUPNPAV_INCLUDE_DIR
          HUpnpAv HINTS
          ${PC_HUPNP_INCLUDE_DIR}
          /usr/include/HUpnpAv/
         )

FIND_LIBRARY(HUPNP_LIBS   HUpnp   PATHS ${PC_HUPNP_LIBRARY})
FIND_LIBRARY(HUPNPAV_LIBS HUpnpAv PATHS ${PC_HUPNP_LIBRARY})

IF(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)
    SET(HUPNP_FOUND TRUE)
    MESSAGE(STATUS "Found HUpnpCore : ${HUPNP_INCLUDE_DIR} ${HUPNP_LIBS}")
ELSE(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)
    SET(HUPNP_FOUND FALSE)
    IF(HUPNP_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could NOT find required package HUpnpCore: <http://herqq.org>")
    ELSE(HUPNP_FIND_REQUIRED)
        MESSAGE(STATUS "Could NOT find required package HUpnpCore: <http://herqq.org>")
    ENDIF(HUPNP_FIND_REQUIRED)
ENDIF(HUPNP_INCLUDE_DIR AND HUPNP_LIBS)

IF(HUPNPAV_INCLUDE_DIR AND HUPNPAV_LIBS)
    SET(HUPNPAV_FOUND TRUE)
    MESSAGE(STATUS "Found HUpnpAv : ${HUPNPAV_INCLUDE_DIR} ${HUPNPAV_LIBS}")
ELSE(HUPNPAV_INCLUDE_DIR AND HUPNPAV_LIBS)
    SET(HUPNPAV_FOUND FALSE)
    IF(HUPNPAV_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could NOT find required package HUpnpAv: <http://herqq.org>")
    ELSE(HUPNPAV_FIND_REQUIRED)
        MESSAGE(STATUS "Could NOT find required package HUpnpAv: <http://herqq.org>")
    ENDIF(HUPNPAV_FIND_REQUIRED)
ENDIF(HUPNPAV_INCLUDE_DIR AND HUPNPAV_LIBS)

INCLUDE(FindPackageHandleStandardArgs)

#FIND_PACKAGE_HANDLE_STANDARD_ARGS(Hupnp DEFAULT_MSG HUPNP_INCLUDE_DIR HUPNP_LIBS)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(HupnpAv DEFAULT_MSG HUPNPAV_INCLUDE_DIR HUPNPAV_LIBS)
