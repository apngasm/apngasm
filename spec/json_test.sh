#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC=../bin/apngasm
SPEC_FILE=../resources/images/test.json
OUT=./json_test.png

# Run.
${EXEC} -f ${SPEC_FILE} -o ${OUT}

