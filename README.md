# PipeFabric

Vendor-agnostic streaming RTL framework for FPGA and DSP development.

## Goals

- Vendor-independent RTL
- AXI-stream-like architecture
- Streaming-first design
- Portable FPGA infrastructure
- DSP and packet-processing pipelines
- Open-source and simulator-friendly

## Features

- Stream interfaces
- Pipeline primitives
- FIFO infrastructure
- CDC utilities
- DSP building blocks
- Verification helpers

## Philosophy

PipeFabric is designed around reusable streaming pipelines instead of monolithic vendor IP.

The framework focuses on:
- portability
- composability
- clean RTL architecture
- open FPGA workflows

## Directory Structure

```text
pipefabric/
├── rtl/
│   ├── pkg/
│   │   ├── pf_types_pkg.sv
│   │   └── pf_logic_pkg.sv
│   │
│   ├── stream/
│   │   ├── pf_stream_if.sv
│   │   ├── pf_pipeline_stage.sv
│   │   └── pf_skid_buffer.sv
│   │
│   ├── fifo/
│   │   └── pf_fifo_sync.sv
│   │
│   ├── primitives/
│   │
│   ├── cdc/
│   │
│   └── dsp/
│
├── sim/
│   ├── tb/
│   │   ├── tb_pf_pipeline_stage.sv
│   │   ├── tb_pf_skid_buffer.sv
│   │   └── tb_pf_fifo_sync.sv
│   │
│   ├── cpp/
│   │   ├── tb_pf_pipeline_stage.cpp
│   │   ├── tb_pf_skid_buffer.cpp
│   │   └── tb_pf_fifo_sync.cpp
│   │
│   └── waves/
│
├── scripts/
│   └── sim/
│       ├── run_pipeline_stage.sh
│       ├── run_skid_buffer.sh
│       ├── run_fifo_sync.sh
│       └── run_all.sh
│
├── docs/
│
├── .gitignore
├── README.md
└── LICENSE
