#!/bin/sh

# Determine script path.
SCRIPT_DIR=`dirname $0`

# Set variable.
EXEC="../cli/apngasm"
SRC="$SCRIPT_DIR/../../resources/images/apngasm.png"
OUT="./cli-out/disassemble-test"
OUT_JSON="-j"
OUT_XML="-x xanimation.xml"

# Run.
RUN="${EXEC} -o ${OUT} -D ${SRC} ${OUT_JSON} ${OUT_XML}"
echo ${RUN}
${RUN}
