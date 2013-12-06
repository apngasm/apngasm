#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC=../bin/apngasm
SRC=../resources/images/apngasm/*.png
OUT=./test.png

# Run.
${EXEC} ${SRC} -o ${OUT}
