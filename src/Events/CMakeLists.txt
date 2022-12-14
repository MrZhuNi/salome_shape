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
FIND_PACKAGE(SWIG REQUIRED)

INCLUDE(${SWIG_USE_FILE})

SET(PROJECT_HEADERS
    Events.h
    Events_Message.h
    Events_MessageGroup.h
    Events_Listener.h
    Events_Loop.h
    Events_LongOp.h
    Events_InfoMessage.h
    Events_MessageBool.h
)

SET(PROJECT_SOURCES
    Events_Message.cpp
    Events_MessageGroup.cpp
    Events_Listener.cpp
    Events_Loop.cpp
    Events_LongOp.cpp
    Events_InfoMessage.cpp
    Events_MessageBool.cpp
)

SET(PROJECT_LIBRARIES
    Locale
)

ADD_DEFINITIONS(-DEVENTS_EXPORTS)
ADD_LIBRARY(Events SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

SET(CMAKE_SWIG_FLAGS -threads -w362)

SET_SOURCE_FILES_PROPERTIES(Events.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(Events.i PROPERTIES SWIG_DEFINITIONS "-shadow")

INCLUDE_DIRECTORIES(
    ../Locale
)

TARGET_LINK_LIBRARIES(Events ${PROJECT_LIBRARIES})

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/EventsAPI.py
)

SET(SWIG_LINK_LIBRARIES
  Events
  ${PYTHON_LIBRARIES}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(EventsAPI LANGUAGE python SOURCES Events.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(EventsAPI python Events.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(EventsAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_EventsAPI PROPERTIES DEBUG_OUTPUT_NAME _EventsAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS Events DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(TARGETS _EventsAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})
