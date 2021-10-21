#!/bin/sh

# Determine script path.
SCRIPT_DIR=`dirname $0`

# Set variable.
EXEC="../cli/apngasm"
SPEC_FILE="$SCRIPT_DIR/../../resources/images/test.xml"
OUT="./cli-out/xml-test.png"

# Run.
RUN="${EXEC} -f ${SPEC_FILE} -o ${OUT}"
echo ${RUN}
${RUN}
