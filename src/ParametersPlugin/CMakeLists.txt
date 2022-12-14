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
INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(
    ../Locale
    ${QT_INCLUDES}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(PROJECT_HEADERS
    ParametersPlugin.h
    ParametersPlugin_Plugin.h
    ParametersPlugin_Parameter.h
    ParametersPlugin_EvalListener.h
    ParametersPlugin_Validators.h
    ParametersPlugin_WidgetCreator.h
    ParametersPlugin_ParametersMgr.h
    ParametersPlugin_WidgetParamsMgr.h
)

SET(PROJECT_MOC_HEADERS
    ParametersPlugin_WidgetParamsMgr.h
)

SET(PROJECT_SOURCES
    ParametersPlugin_Plugin.cpp
    ParametersPlugin_Parameter.cpp
   ParametersPlugin_EvalListener.cpp
    ParametersPlugin_Validators.cpp
    ParametersPlugin_WidgetCreator.cpp
    ParametersPlugin_ParametersMgr.cpp
    ParametersPlugin_WidgetParamsMgr.cpp
)

SET(XML_RESOURCES
    plugin-Parameters.xml
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/ModuleBase
                    ${OpenCASCADE_INCLUDE_DIR}
)

SET(PROJECT_LIBRARIES
    Events
    Config
    Locale
    ModelAPI
    ModuleBase
    ${QT_LIBRARIES}
)

ADD_DEFINITIONS(-DPARAMETERSPLUGIN_EXPORTS ${OpenCASCADE_DEFINITIONS})

SET(TEXT_RESOURCES
	ParametersPlugin_msg_ru.ts
	ParametersPlugin_msg_en.ts
	ParametersPlugin_msg_fr.ts
)

# QT4_CREATE_TRANSLATION(QM_RESOURCES
# 					   ${PROJECT_SOURCES}
# 					   ${TEXT_RESOURCES}
# 					   OPTIONS -extensions cpp -no-recursive
# 					   )

#SET(PROJECT_AUTOMOC
#    ${CMAKE_CURRENT_BINARY_DIR}/ParametersPlugin_automoc.cpp
#)
# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})


SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${QM_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})
SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})

ADD_LIBRARY(ParametersPlugin MODULE
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${XML_RESOURCES}
            ${TEXT_RESOURCES}
            ${QM_RESOURCES}
            ${PROJECT_AUTOMOC}
)


TARGET_LINK_LIBRARIES(ParametersPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ParametersPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(FILES ${QM_RESOURCES} DESTINATION ${SHAPER_INSTALL_QM_RESOURCES})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/ParametersPlugin")
  
  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  
  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})
  
  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
  install(DIRECTORY Test/data DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
