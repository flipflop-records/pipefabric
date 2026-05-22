// =============================================================================
// PipeFabric
// =============================================================================
// File    : pf_pipeline_stage.sv
// Author  : Vladislav Temnyakov
// Version : 0.1.0
// Created : 2026-05-22
//
// Description:
// -----------------------------------------------------------------------------
// Single-stage elastic streaming pipeline register with valid/ready handshake.
//
// Features:
//   - 1-cycle pipeline stage
//   - Backpressure propagation
//   - Frame-aware transport
//   - AXI-stream-like semantics
//
// Notes:
//   - Designed for FPGA-first streaming architectures
//   - Vendor-agnostic RTL
//   - Part of PipeFabric streaming infrastructure
//
// License:
//   MIT License
// =============================================================================

module pf_pipeline_stage #(
    parameter int WIDTH = 32
)(
    input logic i_clk,
    input logic i_rst_n,

    // Input stream
    input  pf_stream_if.slave  i_s,

    // Output stream
    output pf_stream_if.master o_m
);
    import pf_logic_pkg::*;

    // Internal registers
    logic             r_valid_q;
    logic [WIDTH-1:0] r_data_q;

    logic             r_last_q;
    logic             r_error_q;
    logic [3:0]       r_keep_q;
    logic [7:0]       r_user_q;

    logic w_fire;

    assign w_fire = i_s.valid && i_s.ready;

    // sequential logic
    always_ff @(posedge i_clk or negedge i_rst_n) begin
        if (!i_rst_n) begin
            r_valid_q <= 1'b0;
        end else begin
            
            if (w_fire) begin
                r_valid_q <= 1'b1;

                r_data_q  <= i_s.data;
                r_last_q  <= i_s.last;
                r_error_q <= i_s.error;
                r_keep_q  <= i_s.keep;
                r_user_q  <= i_s.user;
            
            end else if (r_valid_q && o_m.ready) begin
                r_valid_q <= 1'b0;
            end

        end
    end

    // Output assignments
    assign o_m.valid = r_valid_q;
    assign o_m.data  = r_data_q;
    assign o_m.last  = r_last_q;
    assign o_m.error = r_error_q;
    assign o_m.keep  = r_keep_q;
    assign o_m.user  = r_user_q;

    // backpressure
    assign i_s.ready = !r_valid_q || o_m.ready;

endmodule