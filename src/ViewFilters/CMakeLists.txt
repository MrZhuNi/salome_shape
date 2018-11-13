## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

INCLUDE(Common)

SET(PROJECT_HEADERS
    ViewFilters.h
    ViewFilters_Plugin.h
	ViewFilters_HorizontalPlane.h
	ViewFilters_VerticalPlane.h
)

SET(PROJECT_SOURCES
    ViewFilters_Plugin.cpp
	ViewFilters_HorizontalPlane.cpp
	ViewFilters_VerticalPlane.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI
    Events
    Config
    GeomAPI
)

ADD_DEFINITIONS(-DVIEWFILTERS_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(ViewFilters SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ViewFilters ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ${OpenCASCADE_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
)

INSTALL(TARGETS ViewFilters DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
