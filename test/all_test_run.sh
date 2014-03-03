#!/bin/sh

# All test run.
find `dirname $0` -name '*test.sh' -exec echo {} \; -exec {} \;
