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
rtl/
├── pkg/
├── primitives/
├── stream/
├── fifo/
├── cdc/
└── dsp/
