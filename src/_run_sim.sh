#!/bin/bash

# option
SCARAB_PARAMS="
--frontend=memtrace \
--output_dir=$(pwd)/../output \
--inst_limit=1000000 \
--cbp_trace_r0=/simpoint_traces/clang/traces/whole/drmemtrace.clang.03072.7752.dir/trace/drmemtrace.clang.03072.4467.trace.zip \
--memtrace_modules_log=/simpoint_traces/clang/traces/whole/drmemtrace.clang.03072.7752.dir/raw \
--dcache_repl=15
"

# run
mkdir -p ../output
./scarab $SCARAB_PARAMS
