#!/bin/bash

source env.sh
source env_standalone.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

CMAKE_ARGS="-D_ECLIPSE_VERSION=4.3"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=ON"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} -DPYTHON_EXECUTABLE=${PYTHONHOME}/bin/python"
CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=OFF"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

cmake -G "Eclipse CDT4 - Unix Makefiles" ${CMAKE_ARGS}

/dn48/newgeom/common/eclipse-4.4.0/eclipse&
