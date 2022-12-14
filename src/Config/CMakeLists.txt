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
INCLUDE(UnitTest)
INCLUDE(XMLProcessing)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/Events
                     ${PYTHON_INCLUDE_DIR})

SET(PROJECT_HEADERS
  Config_def.h
  Config_FeatureMessage.h
  Config_PluginMessage.h
  Config_XMLReader.h
  Config_ModuleReader.h
  Config_FeatureReader.h
  Config_Keywords.h
  Config_WidgetAPI.h
  Config_WidgetReader.h
  Config_PointerMessage.h
  Config_Common.h
  Config_ValidatorMessage.h
  Config_Prop.h
  Config_PropManager.h
  Config_AttributeMessage.h
  Config_ValidatorReader.h
  Config_DataModelReader.h
  Config_Translator.h
 )

SET(PROJECT_SOURCES
  Config_FeatureMessage.cpp
  Config_PluginMessage.cpp
  Config_XMLReader.cpp
  Config_ModuleReader.cpp
  Config_FeatureReader.cpp
  Config_WidgetAPI.cpp
  Config_WidgetReader.cpp
  Config_PointerMessage.cpp
  Config_Common.cpp
  Config_ValidatorMessage.cpp
  Config_Prop.cpp
  Config_PropManager.cpp
  Config_AttributeMessage.cpp
  Config_ValidatorReader.cpp
  Config_DataModelReader.cpp
  Config_Translator.cpp
)

SET(CMAKE_SWIG_FLAGS -threads -w321,401)
SET_SOURCE_FILES_PROPERTIES(ConfigAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(ConfigAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")
SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/ConfigAPI.py
)
SET(SWIG_LINK_LIBRARIES
  Config
  ${PYTHON_LIBRARIES}
)
IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(ConfigAPI LANGUAGE python SOURCES ConfigAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(ConfigAPI python ConfigAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(ConfigAPI ${SWIG_LINK_LIBRARIES})
IF(WIN32)
  SET_TARGET_PROPERTIES(_ConfigAPI PROPERTIES DEBUG_OUTPUT_NAME _ConfigAPI_d)
ENDIF(WIN32)


SET(XML_RESOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/plugins.xml
  dataModel.xml
)

SET(PROJECT_LIBRARIES
    Events
    ${LIBXML2_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

SOURCE_GROUP ("Resource Files" FILES ${XML_RESOURCES})

ADD_DEFINITIONS(-DCONFIG_EXPORTS -D_SCL_SECURE_NO_WARNINGS)
# -D_SCL_SECURE_NO_WARNINGS - to disable warnings 4996

ADD_LIBRARY(Config SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(Config ${PROJECT_LIBRARIES})

# Prepare plugins.xml

# the solver to activate after installation
SET(DEFAULT_SOLVER "PlaneGCS")

# the list of all solvers
SET(SOLVERS "SolveSpace" "PlaneGCS")

# apply NO_<solver> variables
FOREACH(solver IN LISTS SOLVERS)
  IF(NO_${solver})
    LIST(REMOVE_ITEM SOLVERS ${solver})
  ENDIF()
ENDFOREACH()

# check default solver in the list of solvers
LIST(FIND SOLVERS ${DEFAULT_SOLVER} INDEX)
IF(${INDEX} GREATER -1)
  MESSAGE(STATUS "Default solver: ${DEFAULT_SOLVER}")
ELSE()
  MESSAGE(FATAL_ERROR "Default solver: ${DEFAULT_SOLVER} is switched off.\nUnset NO_${DEFAULT_SOLVER} variable or define another default solver DEFAULT_SOLVER=<SOLVER> from the list: ${SOLVERS}.")
ENDIF()

# prepare variables for configuration
SET(DEFAULT_SOLVER "  <plugin library=\"${DEFAULT_SOLVER}Solver\"/>")
FOREACH(solver IN LISTS SOLVERS)
  IF(DEFINED ALL_SOLVERS)
    SET(ALL_SOLVERS "${ALL_SOLVERS}\n")
  ENDIF()
  SET(ALL_SOLVERS "${ALL_SOLVERS}  <plugin library=\"${solver}Solver\"/>")
ENDFOREACH()

# configuration
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/plugins.xml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/plugins.xml"
  )

INSTALL(TARGETS Config DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

INSTALL(TARGETS _ConfigAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/Config")
  
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
