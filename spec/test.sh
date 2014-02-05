#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC="../build/cli/apngasm"
SRC="../resources/images/apngasm/*.png"
DELAY="-d 50"
OUT="./test.png"

# Run.
RUN="${EXEC} ${SRC} -o ${OUT} ${DELAY}"
echo ${RUN}
${RUN}
