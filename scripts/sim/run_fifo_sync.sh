#!/usr/bin/env bash

set -e

echo "========================================"
echo " PipeFabric :: pf_fifo_sync TB"
echo "========================================"

verilator -Wall --cc --exe --build --sv \
  rtl/pkg/pf_types_pkg.sv \
  rtl/pkg/pf_logic_pkg.sv \
  rtl/stream/pf_stream_if.sv \
  rtl/fifo/pf_fifo_sync.sv \
  sim/tb/tb_pf_fifo_sync.sv \
  sim/cpp/tb_pf_fifo_sync.cpp \
  --top-module tb_pf_fifo_sync

echo ""
echo "========================================"
echo " Running simulation"
echo "========================================"

./obj_dir/Vtb_pf_fifo_sync

echo ""