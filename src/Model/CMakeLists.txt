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
    Model.h
    Model_Application.h
    Model_Document.h
    Model_Objects.h
    Model_Session.h
    Model_Data.h
    Model_AttributeDouble.h
    Model_AttributeDoubleArray.h
    Model_AttributeDocRef.h
    Model_AttributeReference.h
    Model_AttributeRefAttr.h
    Model_AttributeRefList.h
    Model_AttributeRefAttrList.h
    Model_AttributeBoolean.h
    Model_AttributeImage.h
    Model_AttributeIntArray.h
    Model_AttributeString.h
    Model_AttributeStringArray.h
    Model_AttributeInteger.h
    Model_AttributeSelection.h
    Model_AttributeSelectionList.h
    Model_AttributeTables.h
    Model_AttributeValidator.h
    Model_BodyBuilder.h
    Model_Events.h
    Model_Expression.h
    Model_FeatureValidator.h
    Model_FiltersFactory.h
    Model_ResultBody.h
    Model_ResultConstruction.h
    Model_ResultPart.h
    Model_ResultField.h
    Model_ResultGroup.h
    Model_ResultParameter.h
    Model_Tools.h
    Model_Update.h
    Model_Validator.h
)

SET(PROJECT_SOURCES
    Model_Application.cpp
    Model_Document.cpp
    Model_Objects.cpp
    Model_Session.cpp
    Model_Data.cpp
    Model_AttributeDouble.cpp
    Model_AttributeDoubleArray.cpp
    Model_AttributeDocRef.cpp
    Model_AttributeReference.cpp
    Model_AttributeRefAttr.cpp
    Model_AttributeRefList.cpp
    Model_AttributeRefAttrList.cpp
    Model_AttributeBoolean.cpp
    Model_AttributeImage.cpp
    Model_AttributeIntArray.cpp
    Model_AttributeString.cpp
    Model_AttributeStringArray.cpp
    Model_AttributeInteger.cpp
    Model_AttributeSelection.cpp
    Model_AttributeSelectionList.cpp
    Model_AttributeTables.cpp
    Model_AttributeValidator.cpp
    Model_BodyBuilder.cpp
    Model_Events.cpp
    Model_Expression.cpp
    Model_FeatureValidator.cpp
    Model_FiltersFactory.cpp
    Model_ResultBody.cpp
    Model_ResultConstruction.cpp
    Model_ResultPart.cpp
    Model_ResultField.cpp
    Model_ResultGroup.cpp
    Model_ResultParameter.cpp
    Model_Tools.cpp
    Model_Update.cpp
    Model_Validator.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI
    Events
    Config
    Selector
    GeomData
    GeomAPI
    GeomAlgoAPI
    Locale
    ModelGeomAlgo
    ${OpenCASCADE_ApplicationFramework_LIBRARIES}
)
SET(PROJECT_INCLUDES
  ../ModelAPI
  ../Events
  ../Config
  ../Selector
  ../GeomData
  ../GeomDataAPI
  ../GeomAlgoAPI
  ../GeomAPI
  ../Locale
  ../ModelGeomAlgo
  ../ConstructionPlugin
  ${OpenCASCADE_INCLUDE_DIR}
)


IF(TKTInspector)
    ADD_DEFINITIONS(-DTINSPECTOR)
ENDIF()

ADD_DEFINITIONS(-DMODEL_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(Model SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Model ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(${PROJECT_INCLUDES})

INSTALL(TARGETS Model DESTINATION ${SHAPER_INSTALL_BIN})
