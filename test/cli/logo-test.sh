#!/bin/sh

# Determine script path.
SCRIPT_DIR=`dirname $0`

# Set variable.
EXEC="../cli/apngasm"
SRC="$SCRIPT_DIR/../../resources/images/apngasm/*.png"
DELAY="-d 50"
OUT="./cli-out/logo-test.png"

# Run.
RUN="${EXEC} ${SRC} -o ${OUT} ${DELAY}"
echo ${RUN}
${RUN}
