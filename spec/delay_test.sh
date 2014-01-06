#!/bin/sh

# Change current directory.
cd `dirname $0`

# Set variable.
EXEC="../bin/apngasm"
SRC="\
  ../resources/images/apngasm/00.png 200\
  ../resources/images/apngasm/01.png 200\
  ../resources/images/apngasm/02.png 200\
  ../resources/images/apngasm/03.png 200\
  ../resources/images/apngasm/04.png 200\
  ../resources/images/apngasm/05.png 200\
  ../resources/images/apngasm/06.png 200\
  ../resources/images/apngasm/07.png 200\
  ../resources/images/apngasm/08.png 200\
  ../resources/images/apngasm/09.png 200\
  ../resources/images/apngasm/10.png 200\
  ../resources/images/apngasm/11.png 200\
  ../resources/images/apngasm/12.png 200\
  ../resources/images/apngasm/13.png 200\
  ../resources/images/apngasm/14.png 200\
  ../resources/images/apngasm/15.png 200\
  ../resources/images/apngasm/16.png 200\
  ../resources/images/apngasm/17.png 200\
  ../resources/images/apngasm/18.png 200\
  ../resources/images/apngasm/19.png 200\
  ../resources/images/apngasm/20.png 200\
  ../resources/images/apngasm/21.png 200\
  ../resources/images/apngasm/22.png 200\
  ../resources/images/apngasm/23.png 200\
  ../resources/images/apngasm/24.png 200\
  ../resources/images/apngasm/25.png 200\
  ../resources/images/apngasm/26.png 200\
  ../resources/images/apngasm/27.png 200\
  ../resources/images/apngasm/28.png 200\
  ../resources/images/apngasm/29.png 200\
  ../resources/images/apngasm/30.png 200\
  ../resources/images/apngasm/31.png 200\
  ../resources/images/apngasm/32.png 200\
  ../resources/images/apngasm/33.png 200\
  "
OUT="./delay_test.png"

# Run.
RUN="${EXEC} ${SRC} -o ${OUT} ${DELAY}"
echo ${RUN}
${RUN}
