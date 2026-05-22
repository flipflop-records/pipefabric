`timescale 1ns / 1ps
`default_nettype none
// =============================================================================
// PipeFabric
// =============================================================================
// File    : pf_skid_buffer.sv
// Author  : Vladislav Temnyakov
// Version : 0.1.0
// Created : 2026-05-22
//
// Description:
// -----------------------------------------------------------------------------
// Two-stage elastic skid buffer for streaming pipelines.
//
// Features:
//   - Valid/ready handshake support
//   - Backpressure handling
//   - Temporary skid storage
//   - Frame-aware transport
//   - Zero-loss buffering during downstream stalls
//
// Architecture:
//   - Main output register
//   - Secondary skid register
//   - Automatic data forwarding between stages
//
// Notes:
//   - Designed for FPGA streaming architectures
//   - Prevents data loss during ready deassertion
//   - Intended for high-throughput streaming pipelines
//   - Vendor-independent RTL
//
// License:
//   MIT License
// =============================================================================

module pf_skid_buffer #(
    parameter int WIDTH = 32
)(
    input logic i_clk,
    input logic i_rst_n,

    // Input stream
    input  pf_stream_if.slave  i_s,

    // Output stream
    output pf_stream_if.master o_m
);

    // Main output registers
    logic             r_valid_q;
    logic [WIDTH-1:0] r_data_q;
    logic             r_last_q;
    logic             r_error_q;
    logic [3:0]       r_keep_q;
    logic [7:0]       r_user_q;

    // Skid registers
    logic             r_skid_valid_q;
    logic [WIDTH-1:0] r_skid_data_q;
    logic             r_skid_last_q;
    logic             r_skid_error_q;
    logic [3:0]       r_skid_keep_q;
    logic [7:0]       r_skid_user_q;

    logic             w_input_fire;
    logic             w_output_fire;

    assign w_input_fire  = i_s.valid && i_s.ready;
    assign w_output_fire = o_m.valid && o_m.ready;

    // Input can be acceptet while skid register is free
    assign i_s.ready = !r_skid_valid_q;

    always_ff @(posedge i_clk or negedge i_rst_n) begin
        if (!i_rst_n) begin
            r_valid_q      <= 1'b0;
            r_skid_valid_q <= 1'b0;
        end else begin

            // Output consumed
            if (w_output_fire) begin
                
                // If skid has saved data, move it to output
                if (r_skid_valid_q) begin
                    r_valid_q      <= 1'b1;

                    r_data_q       <= r_skid_data_q;
                    r_last_q       <= r_skid_last_q;
                    r_error_q      <= r_skid_error_q;
                    r_keep_q       <= r_skid_keep_q;
                    r_user_q       <= r_skid_user_q;

                    r_skid_valid_q <= 1'b0;
                end else begin
                    r_valid_q <= 1'b0;
                end
            end

            // Input accepted
            if (w_input_fire) begin
                
                // If output register if free of being consumed, load directly
                if (!r_valid_q || w_output_fire) begin
                    r_valid_q <= 1'b1;

                    r_data_q  <= i_s.data;
                    r_last_q  <= i_s.last;
                    r_error_q <= i_s.error;
                    r_keep_q  <= i_s.keep;
                    r_user_q  <= i_s.user;
                end

                // If output is occupied and not consumed, save into skid register
                else begin
                    r_skid_valid_q <= 1'b1;

                    r_skid_data_q  <= i_s.data;
                    r_skid_last_q  <= i_s.last;
                    r_skid_error_q <= i_s.error;
                    r_skid_keep_q  <= i_s.keep;
                    r_skid_user_q  <= i_s.user;
                end
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

endmodule

`default_nettype wire