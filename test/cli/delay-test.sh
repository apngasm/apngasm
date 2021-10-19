#!/bin/sh

# Determine script path.
SCRIPT_DIR=`dirname $0`

# Set variable.
EXEC="../cli/apngasm"
SRC="\
  $SCRIPT_DIR/../../resources/images/apngasm/12.png 650\
  $SCRIPT_DIR/../../resources/images/apngasm/13.png\
  $SCRIPT_DIR/../../resources/images/apngasm/14.png\
  $SCRIPT_DIR/../../resources/images/apngasm/15.png\
  $SCRIPT_DIR/../../resources/images/apngasm/16.png\
  $SCRIPT_DIR/../../resources/images/apngasm/17.png\
  $SCRIPT_DIR/../../resources/images/apngasm/18.png\
  $SCRIPT_DIR/../../resources/images/apngasm/19.png\
  $SCRIPT_DIR/../../resources/images/apngasm/20.png\
  $SCRIPT_DIR/../../resources/images/apngasm/21.png\
  $SCRIPT_DIR/../../resources/images/apngasm/22.png\
  $SCRIPT_DIR/../../resources/images/apngasm/23.png\
  $SCRIPT_DIR/../../resources/images/apngasm/24.png\
  $SCRIPT_DIR/../../resources/images/apngasm/25.png\
  $SCRIPT_DIR/../../resources/images/apngasm/26.png\
  $SCRIPT_DIR/../../resources/images/apngasm/27.png\
  $SCRIPT_DIR/../../resources/images/apngasm/28.png\
  $SCRIPT_DIR/../../resources/images/apngasm/29.png\
  $SCRIPT_DIR/../../resources/images/apngasm/30.png\
  $SCRIPT_DIR/../../resources/images/apngasm/31.png\
  $SCRIPT_DIR/../../resources/images/apngasm/32.png\
  $SCRIPT_DIR/../../resources/images/apngasm/33.png\
  "
DELAY="-d 50"
OUT="./cli-out/delay-test.png"

# Run.
RUN="${EXEC} ${SRC} -o ${OUT} ${DELAY}"
echo ${RUN}
${RUN}
