#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC="../build/cli/apngasm"
SRC="../resources/images/apngasm.png"
OUT="./test"
OUT_JSON="-j"
OUT_XML="-x ../../test/test/xml/animation.xml"

# Run.
RUN="${EXEC} -o ${OUT} -D ${SRC} ${OUT_JSON} ${OUT_XML}"
echo ${RUN}
${RUN}
