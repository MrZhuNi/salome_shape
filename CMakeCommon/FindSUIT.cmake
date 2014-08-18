
SET(SUIT_DIR $ENV{SUIT_DIR})
IF(EXISTS ${SUIT_DIR})

    MESSAGE(STATUS "SUIT found at ${SUIT_DIR}")
    IF(WIN32)
        SET(SUIT_LIB_DIR ${SUIT_DIR}/lib)
    ElSE()
        SET(SUIT_LIB_DIR ${SUIT_DIR}/lib/salome)
    ENDIF()

    FIND_LIBRARY(Event Event ${SUIT_LIB_DIR})
    FIND_LIBRARY(PyConsole PyConsole ${SUIT_LIB_DIR})
    FIND_LIBRARY(PyInterp PyInterp ${SUIT_LIB_DIR})
    FIND_LIBRARY(qtx qtx ${SUIT_LIB_DIR})
    FIND_LIBRARY(suit suit ${SUIT_LIB_DIR})

    IF(WIN32)
        SET(SUIT_INCLUDE ${SUIT_DIR}/include)
    ElSE()
        SET(SUIT_INCLUDE ${SUIT_DIR}/include/salome)
    ENDIF()

ENDIF(EXISTS ${SUIT_DIR})
