#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC="../build/cli/apngasm"
SRC="\
  ../resources/images/apngasm/12.png 650\
  ../resources/images/apngasm/13.png\
  ../resources/images/apngasm/14.png\
  ../resources/images/apngasm/15.png\
  ../resources/images/apngasm/16.png\
  ../resources/images/apngasm/17.png\
  ../resources/images/apngasm/18.png\
  ../resources/images/apngasm/19.png\
  ../resources/images/apngasm/20.png\
  ../resources/images/apngasm/21.png\
  ../resources/images/apngasm/22.png\
  ../resources/images/apngasm/23.png\
  ../resources/images/apngasm/24.png\
  ../resources/images/apngasm/25.png\
  ../resources/images/apngasm/26.png\
  ../resources/images/apngasm/27.png\
  ../resources/images/apngasm/28.png\
  ../resources/images/apngasm/29.png\
  ../resources/images/apngasm/30.png\
  ../resources/images/apngasm/31.png\
  ../resources/images/apngasm/32.png\
  ../resources/images/apngasm/33.png\
  "
DELAY="-d 50"
OUT="./delay_test.png"

# Run.
RUN="${EXEC} ${SRC} -o ${OUT} ${DELAY}"
echo ${RUN}
${RUN}
