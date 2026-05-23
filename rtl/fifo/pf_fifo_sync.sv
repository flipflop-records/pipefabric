`timescale 1ps / 1ps
`default_nettype none

// =============================================================================
// PipeFabric
// =============================================================================
// File    : pf_fifo_sync.sv
// Author  : flipflop-records
// Version : 0.1.0
// Created : 2026-05-23
//
// Description:
// -----------------------------------------------------------------------------
// Synchronous streaming FIFO with valid/ready handshake.
//
// Features:
//   - Single-clock FIFO
//   - Valid/ready stream interface
//   - Frame-aware transport
//   - Sideband metadata storage
//   - Vendor-independent RTL
//
// Notes:
//   - Designed for FPGA streaming architectures
//   - This v0.1 implementation uses generic RTL memory
//   - Not intended for clock-domain crossing
//
// License:
//   MIT License
// =============================================================================

module pf_fifo_sync #(
    parameter int WIDTH = 32,
    parameter int DEPTH = 16
)(
    input  logic i_clk,
    input  logic i_rst_n,

    // Input stream
    pf_stream_if.slave  i_s,

    // Output stream
    pf_stream_if.master o_m
);

    localparam int KEEP_WIDTH  = 4;
    localparam int USER_WIDTH  = 8;
    localparam int META_WIDTH  = 1 + 1 + KEEP_WIDTH + USER_WIDTH;
    localparam int WORD_WIDTH  = WIDTH + META_WIDTH;

    localparam int ADDR_WIDTH  = (DEPTH <= 1) ? 1 : $clog2(DEPTH);
    localparam int COUNT_WIDTH = $clog2(DEPTH + 1);

    // FIFO memory
    logic [WORD_WIDTH-1:0] r_mem [0:DEPTH-1];

    // Pointers and counters
    logic [ADDR_WIDTH-1:0]  r_wr_ptr_q, r_rd_ptr_q;
    logic [COUNT_WIDTH-1:0] r_count_q;

    // Control wires
    logic w_full, w_empty;
    logic w_push, w_pop;

    logic [WORD_WIDTH-1:0] w_write_word;
    logic [WORD_WIDTH-1:0] w_read_word;

    assign w_full    = (r_count_q == DEPTH[COUNT_WIDTH-1:0]);
    assign w_empty   = (r_count_q == '0);

    assign i_s.ready = !w_full;
    assign o_m.valid = !w_empty;

    assign w_push    = i_s.valid && i_s.ready;
    assign w_pop     = o_m.valid && o_m.ready;

    assign w_write_word = {
        i_s.user,
        i_s.keep,
        i_s.error,
        i_s.last,
        i_s.data
    };

    assign w_read_word = r_mem[r_rd_ptr_q];

    assign {
        o_m.user,
        o_m.keep,
        o_m.error,
        o_m.last,
        o_m.data
    } = w_read_word;

    always_ff @(posedge i_clk or negedge i_rst_n) begin
        if (!i_rst_n) begin
            r_wr_ptr_q <= '0;
            r_rd_ptr_q <= '0;
            r_count_q  <= '0;
        end else begin
            
            if (w_push) begin
                r_mem[r_wr_ptr_q] <= w_write_word;

                if (r_wr_ptr_q == DEPTH[ADDR_WIDTH-1:0] - 1'b1) begin
                    r_wr_ptr_q <= '0;
                end else begin
                    r_wr_ptr_q <= r_wr_ptr_q + 1;
                end
            end

            if (w_pop) begin
                if (r_rd_ptr_q == DEPTH[ADDR_WIDTH-1:0] - 1'b1) begin
                    r_rd_ptr_q <= '0;
                end else begin
                    r_rd_ptr_q <= r_rd_ptr_q + 1;
                end
            end

            case ({w_push, w_pop})
                2'b10:   r_count_q <= r_count_q + 1'b1;
                2'b01:   r_count_q <= r_count_q - 1'b1;
                default: r_count_q <= r_count_q;
            endcase

        end
    end

endmodule

`default_nettype wire
