#!/usr/bin/env bash

set -e

echo "========================================"
echo " PipeFabric :: Full Simulation Regression"
echo "========================================"

./scripts/sim/run_pipeline_stage.sh
./scripts/sim/run_skid_buffer.sh
./scripts/sim/run_fifo_sync.sh

echo ""
echo "========================================"
echo " All PipeFabric simulations PASSED"
echo "========================================"

echo ""