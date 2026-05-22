#!/usr/bin/env bash

set -e

echo "========================================"
echo " PipeFabric :: pf_pipeline_stage TB"
echo "========================================"

verilator -Wall --cc --exe --build --sv \
  rtl/pkg/pf_types_pkg.sv \
  rtl/pkg/pf_logic_pkg.sv \
  rtl/stream/pf_stream_if.sv \
  rtl/stream/pf_pipeline_stage.sv \
  sim/tb/tb_pf_pipeline_stage.sv \
  sim/cpp/tb_pf_pipeline_stage.cpp \
  --top-module tb_pf_pipeline_stage

echo ""
echo "========================================"
echo " Running simulation"
echo "========================================"

./obj_dir/Vtb_pf_pipeline_stage

echo ""