# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    GeomDataAPI.h
    GeomDataAPI_Point.h
    GeomDataAPI_Dir.h
    GeomDataAPI_Point2D.h
    GeomDataAPI_Point2DArray.h
)

SET(PROJECT_SOURCES
    GeomDataAPI_Point.cpp
    GeomDataAPI_Dir.cpp
    GeomDataAPI_Point2D.cpp
    GeomDataAPI_Point2DArray.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI
)

INCLUDE_DIRECTORIES(
  ../GeomAPI        # only for SWIG
  ../Events         # only for SWIG
  ../ModelAPI
)

ADD_DEFINITIONS(-DGEOMDATAAPI_EXPORTS)
ADD_LIBRARY(GeomDataAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
SET_TARGET_PROPERTIES(GeomDataAPI PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(GeomDataAPI ${PROJECT_LIBRARIES})

SET(CMAKE_SWIG_FLAGS -w325,321,362,383,302,403)

SET_SOURCE_FILES_PROPERTIES(GeomDataAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(GeomDataAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/GeomDataAPI.py
)

SET(SWIG_LINK_LIBRARIES
  GeomDataAPI
  ${PYTHON_LIBRARIES}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(GeomDataAPI LANGUAGE python SOURCES GeomDataAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(GeomDataAPI python GeomDataAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(GeomDataAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_GeomDataAPI PROPERTIES DEBUG_OUTPUT_NAME _GeomDataAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS GeomDataAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(TARGETS _GeomDataAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/GeomDataAPI")
  
  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  
  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})
  
  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
