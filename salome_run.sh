#!/bin/bash

source env.sh

${TOOLS_DIR}/solver.sh "$@"

# kill processes of previous SALOME run
if [ -f .salomeport ]; then
  SALOME_PORT=$(cat .salomeport)
  ${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
  pkill -f "omniNames -start ${SALOME_PORT}"
  rm .salomeport
fi 

SALOME_ARGS=""
if [[ "${SALOME_SQUISH_PREFS:-FALSE}" == "TRUE" ]]; then
  SALOME_ARGS="${SALOME_ARGS} -r ${SOURCES_DIR}/test.squish/shared/testdata/SalomeApp.xml"
fi
SALOME_ARGS="${SALOME_ARGS} --ns-port-log=$(pwd)/.salomeport"

${KERNEL_ROOT_DIR}/bin/salome/runSalome.py ${SALOME_ARGS} >log_runSalome 2>err_runSalome

SALOME_PORT=$(cat .salomeport)
echo "Kill SALOME on port ${SALOME_PORT}"
${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
pkill -f "omniNames -start ${SALOME_PORT}"

