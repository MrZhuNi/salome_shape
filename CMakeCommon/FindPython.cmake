CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  SET(PYTHON_LIBRARY_DLL "python27_d.lib")
ElSE()
  SET(PYTHON_LIBRARY_DLL "python27.lib")
ENDIF()

FILE(TO_CMAKE_PATH "$ENV{PYTHON_LIB_DIR}/${PYTHON_LIBRARY_DLL}" PYTHON_LIBRARY)
FILE(TO_CMAKE_PATH $ENV{PYTHON_INC_DIR} PYTHON_INCLUDE_DIR)

# TOOD: Clean a mess with python interpreter
# FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(PythonLibs)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
