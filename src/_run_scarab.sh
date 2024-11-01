#!/bin/bash

# config
INST_NUM=1000000
OUTPUT_DIR=$(pwd)/../output
APP_NAME=clang

# trace
case $APP_NAME in
  clang)
    MODULESDIR=/simpoint_traces/clang/traces/whole/drmemtrace.clang.03072.7752.dir/raw
    TRACEFILE=/simpoint_traces/clang/traces/whole/drmemtrace.clang.03072.7752.dir/trace/drmemtrace.clang.03072.4467.trace.zip
    ;;
  *)
    echo "unknown application"
    ;;
esac

# input
SCARAB_PARAMS="
--frontend=memtrace \
--output_dir=$OUTPUT_DIR \
--inst_limit=$INST_NUM \
--cbp_trace_r0=$TRACEFILE \
--memtrace_modules_log=$MODULESDIR
"

# run
./scarab $SCARAB_PARAMS
