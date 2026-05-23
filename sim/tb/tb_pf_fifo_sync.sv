`timescale 1ns / 1ps
`default_nettype none

// =============================================================================
// PipeFabric
// =============================================================================
// File    : tb_pf_fifo_sync.sv
// Author  : flipflop-records
// Version : 0.1.0
//
// Description:
// -----------------------------------------------------------------------------
// Testbench wrapper for pf_fifo_sync.
// =============================================================================

module tb_pf_fifo_sync (
    input  logic        i_clk,
    input  logic        i_rst_n,

    // Input stream
    input  logic        i_s_valid,
    input  logic [31:0] i_s_data,
    input  logic        i_s_last,
    input  logic        i_s_error,
    input  logic [3:0]  i_s_keep,
    input  logic [7:0]  i_s_user,
    output logic        o_s_ready,

    // Output stream
    output logic        o_m_valid,
    output logic [31:0] o_m_data,
    output logic        o_m_last,
    output logic        o_m_error,
    output logic [3:0]  o_m_keep,
    output logic [7:0]  o_m_user,
    input  logic        i_m_ready
);

    localparam int WIDTH = 32;
    localparam int DEPTH = 8;

    pf_stream_if #(.WIDTH(WIDTH)) s_in();
    pf_stream_if #(.WIDTH(WIDTH)) s_out();

    assign s_in.valid = i_s_valid;
    assign s_in.data  = i_s_data;
    assign s_in.last  = i_s_last;
    assign s_in.error = i_s_error;
    assign s_in.keep  = i_s_keep;
    assign s_in.user  = i_s_user;

    assign o_s_ready  = s_in.ready;

    assign o_m_valid  = s_out.valid;
    assign o_m_data   = s_out.data;
    assign o_m_last   = s_out.last;
    assign o_m_error  = s_out.error;
    assign o_m_keep   = s_out.keep;
    assign o_m_user   = s_out.user;

    assign s_out.ready = i_m_ready;

    pf_fifo_sync #(
        .WIDTH (WIDTH),
        .DEPTH (DEPTH)
    ) u_dut (
        .i_clk   (i_clk),
        .i_rst_n (i_rst_n),
        .i_s     (s_in),
        .o_m     (s_out)
    );

endmodule

`default_nettype wire
