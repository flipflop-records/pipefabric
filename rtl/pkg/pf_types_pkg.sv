// =============================================================================
// PipeFabric
// =============================================================================
// File    : pf_types_pkg.sv
// Author  : Vladislav Temnyakov
// Version : 0.1.0
// Created : 2026-05-22
//
// Description:
// -----------------------------------------------------------------------------
// Common synthesizable type definitions used across PipeFabric RTL.
//
// Includes:
//   - Signed integer types
//   - Unsigned integer types
//   - Standardized bit-width aliases
//
// Notes:
//   - FPGA-oriented
//   - Vendor-independent
//   - Synthesizable-only package
//
// License:
//   MIT License
// =============================================================================

package pf_types_pkg;

    typedef logic          [63:0] uint64;
		typedef logic signed   [63:0] int64;

		typedef logic          [47:0] uint48;
		typedef logic signed   [47:0] int48;

    typedef logic          [31:0] uint32;
		typedef logic signed   [31:0] int32;

		typedef logic          [15:0] uint16;
		typedef logic signed   [15:0] int16;

		typedef logic          [7:0]  uint8;
		typedef logic signed   [7:0]  int8;
	
endpackage