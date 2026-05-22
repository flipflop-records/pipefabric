`default_nettype none
// =============================================================================
// PipeFabric
// =============================================================================
// File    : pf_logic_pkg.sv
// Author  : Vladislav Temnyakov
// Version : 0.1.0
// Created : 2026-05-22
//
// Description:
// -----------------------------------------------------------------------------
// Common logic and semantic type definitions for PipeFabric.
//
// Includes:
//   - Boolean logic types
//   - Optional-state modeling types
//
// Notes:
//   - Intended for RTL and modeling usage
//   - Minimal semantic abstraction layer
//
// License:
//   MIT License
// =============================================================================

package pf_logic_pkg;

    typedef enum logic [1:0] {
        PF_NO,
        PF_YES,
        PF_MAYBE
    } pf_option_t;

    typedef enum logic {
        PF_FALSE = 1'b0,
        PF_TRUE  = 1'b1
    } pf_bool_t;
  
endpackage

`default_nettype wire