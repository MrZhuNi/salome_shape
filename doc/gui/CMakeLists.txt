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

SET(input ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in)

SET(output ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

SET(DOC_FILES
  Introduction.rst
  Tutorial.rst
  TUI_tutorial.rst
  tutorial.py
  )

CONFIGURE_FILE(${input} ${output})

#sphinx-build -b html -c doc -D latex_paper_size=a4 -d doc/doctree /dn23/PPGP/vsv/SALOME730/SOURCES/PPGP_SRC/doc doc/html

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/General DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

INSTALL(CODE "
EXECUTE_PROCESS(
COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/build_index.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
)
")


INSTALL(CODE "
EXECUTE_PROCESS(
COMMAND ${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_BINARY_DIR} -d doctree ${CMAKE_CURRENT_BINARY_DIR} html
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)"
)

IF(${HAVE_SALOME})
    INSTALL(CODE "
    EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c \"import shutil;shutil.rmtree('${CMAKE_INSTALL_PREFIX}/share/doc/salome/gui/SHAPER', True);shutil.copytree('${CMAKE_CURRENT_BINARY_DIR}/html', '${CMAKE_INSTALL_PREFIX}/share/doc/salome/gui/SHAPER')\"
    )
    ")
ELSE(${HAVE_SALOME})
    INSTALL(CODE "
    EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c \"import shutil;shutil.rmtree('${CMAKE_INSTALL_PREFIX}/doc/gui', True);shutil.copytree('${CMAKE_CURRENT_BINARY_DIR}/html', '${CMAKE_INSTALL_PREFIX}/doc/gui')\"
    )
    ")
ENDIF(${HAVE_SALOME})
