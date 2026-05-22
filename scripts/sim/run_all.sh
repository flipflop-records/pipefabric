#!/usr/bin/env bash

set -e

echo "========================================"
echo " PipeFabric :: Full Simulation Regression"
echo "========================================"

./scripts/sim/run_pipeline_stage.sh
./scripts/sim/run_skid_buffer.sh

echo ""
echo "========================================"
echo " All PipeFabric simulations PASSED"
echo "========================================"

echo ""