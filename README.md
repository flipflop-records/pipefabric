# PipeFabric

Vendor-agnostic streaming RTL framework for FPGA and DSP development.

---

## Goals

- Vendor-independent RTL
- AXI-stream-like architecture
- Streaming-first design
- Portable FPGA infrastructure
- DSP and packet-processing pipelines
- Open-source and simulator-friendly

---

## Features

- Stream interfaces
- Pipeline primitives
- FIFO infrastructure
- CDC utilities
- DSP building blocks
- Verification helpers

---

## Philosophy

PipeFabric is designed around reusable streaming pipelines instead of monolithic vendor IP.

The framework focuses on:

- portability
- composability
- clean RTL architecture
- open FPGA workflows

---

## Design Principles

- Fully synthesizable RTL
- Explicit valid/ready flow control
- Small composable primitives
- No hidden vendor dependencies
- Verification-first development
- Clean timing-oriented architecture

---

## Toolchain

Currently tested with:

- SystemVerilog
- Verilator
- GCC / Clang
- Linux / macOS

Planned support:

- Vivado
- Quartus
- QuestaSim

---

## Current Status

Implemented:

- `pf_stream_if`
- `pf_pipeline_stage`
- `pf_skid_buffer`
- `pf_fifo_sync`

Verification:

- Verilator-based C++ regression tests
- Stream protocol validation
- Backpressure testing
- FIFO ordering verification

---

## Quick Start

Run full regression:

```bash
./scripts/sim/run_all.sh
```

Run individual tests:

```bash
./scripts/sim/run_pipeline_stage.sh
./scripts/sim/run_skid_buffer.sh
./scripts/sim/run_fifo_sync.sh
```

---

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
```

---

## Verification Strategy

PipeFabric uses a mixed SystemVerilog + C++ verification flow.

- SystemVerilog wrappers instantiate DUTs and interfaces
- C++ testbenches drive protocol-level verification
- Verilator is used as the primary simulation backend
- Regression scripts automate simulation runs

The verification environment is designed for:

- deterministic simulations
- reusable stream drivers
- protocol validation
- future randomized testing infrastructure

---

## Roadmap

Planned modules and infrastructure:

- asynchronous FIFOs
- CDC synchronizers
- DSP primitives
- FIR / CIC filters
- packet routers
- DMA-style stream movers
- configurable stream arbiters
- waveform automation
- lint and CI integration

---

## License

MIT License
