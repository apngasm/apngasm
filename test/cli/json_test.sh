#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC="../cli/apngasm"
SPEC_FILE="../resources/images/test.json"
OUT="./json_test.png"

# Run.
RUN="${EXEC} -f ${SPEC_FILE} -o ${OUT}"
echo ${RUN}
${RUN}
