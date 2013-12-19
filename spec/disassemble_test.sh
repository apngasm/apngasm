#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC=../bin/apngasm
SRC=../resources/images/apngasm.png
OUT=./test
OUT_JSON="-j"
OUT_XML="-x ../../spec/test/xml/animation.xml"

# Run.
${EXEC} -o ${OUT} -D ${SRC} ${OUT_JSON} ${OUT_XML}
