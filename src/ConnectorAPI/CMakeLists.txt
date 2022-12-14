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

INCLUDE(Common)

SET(PROJECT_HEADERS
  ConnectorAPI.h
  ConnectorAPI_Connector.h
)

SET(PROJECT_SOURCES
  ConnectorAPI_Connector.cpp
)

SET(PROJECT_LIBRARIES
  ModelAPI
  ModelHighAPI
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
)

# Plugin headers dependency
INCLUDE_DIRECTORIES(
  # TODO(spo): modify ConnectorPlugin headers to remove dependency on GeomAPI headers
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  # TODO(spo): it is for *_swig.h files. Can we eliminate it?
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/ConnectorPlugin
)

#TODO(spo): is ${OpenCASCADE_DEFINITIONS} necessary?
ADD_DEFINITIONS(-DCONNECTORAPI_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(ConnectorAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ConnectorAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(ConnectorAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(ConnectorAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

#TODO(spo): is ModelAPI necessary or it could be received by INTERFACE_ (may require modern CMake)?
SET(SWIG_LINK_LIBRARIES
  ConnectorAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)

SET(SWIG_MODULE_ConnectorAPI_EXTRA_DEPS ${SWIG_MODULE_ConnectorAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)
SET(CMAKE_SWIG_FLAGS -threads -w325,321,302,362,322,383,403)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(ConnectorAPI LANGUAGE python SOURCES ConnectorAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(ConnectorAPI python ConnectorAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(ConnectorAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_ConnectorAPI PROPERTIES DEBUG_OUTPUT_NAME _ConnectorAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _ConnectorAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS ConnectorAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/ConnectorAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})

# Tests
IF(${HAVE_SALOME})
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(Test)
ENDIF(${HAVE_SALOME})
