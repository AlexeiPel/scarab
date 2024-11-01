#!/bin/bash
# config 
INST_NUM=1000000
OUTPUT_DIR=/home/alexeipel/cse220_home/scarab/output
APP_NAME=clang
# trace
case $APP_NAME in
  clang)
    MODULESDIR=/simpoint_traces/clang/traces/whole/drmemtrace.clang.03072.7752.dir/raw
    TRACEFILE=/simpoint_traces/clang/traces/whole/drmemtrace.clang.03072.7752.dir/trace/drmemtrace.clang.03072.4467.trace.zip
    ;;
  gcc)
    MODULESDIR=/simpoint_traces/gcc/traces/whole/drmemtrace.cc1.04250.2989.dir/raw
    TRACEFILE=/simpoint_traces/gcc/traces/whole/drmemtrace.cc1.04250.2989.dir/trace/drmemtrace.cc1.04250.5506.trace.zip
    ;;
  *)
    echo "unknown application"
    ;;
esac
RUN_SCARAB_CMD="
./scarab \
--frontend=memtrace \
--output_dir=$OUTPUT_DIR \
--inst_limit=$INST_NUM \
--cbp_trace_r0=$TRACEFILE \
--memtrace_modules_log=$MODULESDIR
"