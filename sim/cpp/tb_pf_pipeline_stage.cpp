#include "Vtb_pf_pipeline_stage.h"
#include "verilated.h"

#include <cstdint>
#include <iostream>

static vluint64_t sim_time = 0;

static void eval(Vtb_pf_pipeline_stage* dut) {
    dut->eval();
}

static void tick(Vtb_pf_pipeline_stage* dut) {
    dut->i_clk = 0;
    eval(dut);
    sim_time++;

    dut->i_clk = 1;
    eval(dut);
    sim_time++;
}

static void reset_dut(Vtb_pf_pipeline_stage* dut) {
    dut->i_clk = 0;
    dut->i_rst_n = 0;

    dut->i_s_valid = 0;
    dut->i_s_data  = 0;
    dut->i_s_last  = 0;
    dut->i_s_error = 0;
    dut->i_s_keep  = 0;
    dut->i_s_user  = 0;

    dut->i_m_ready = 0;

    for (int i = 0; i < 5; ++i) {
        tick(dut);
    }

    dut->i_rst_n = 1;
    tick(dut);
}

static int check_word(
    Vtb_pf_pipeline_stage* dut,
    uint32_t exp_data,
    uint8_t  exp_last,
    uint8_t  exp_error,
    uint8_t  exp_keep,
    uint8_t  exp_user
) {
    if (!dut->o_m_valid) {
        std::cerr << "ERROR: output valid is low\n";
        return 1;
    }

    if (dut->o_m_data != exp_data) {
        std::cerr << "ERROR: data mismatch: expected 0x"
                  << std::hex << exp_data << ", got 0x"
                  << dut->o_m_data << std::dec << "\n";
        return 1;
    }

    if (dut->o_m_last != exp_last) {
        std::cerr << "ERROR: last mismatch\n";
        return 1;
    }

    if (dut->o_m_error != exp_error) {
        std::cerr << "ERROR: error mismatch\n";
        return 1;
    }

    if (dut->o_m_keep != exp_keep) {
        std::cerr << "ERROR: keep mismatch\n";
        return 1;
    }

    if (dut->o_m_user != exp_user) {
        std::cerr << "ERROR: user mismatch\n";
        return 1;
    }

    return 0;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    auto* dut = new Vtb_pf_pipeline_stage;

    reset_dut(dut);

    std::cout << "TB: pf_pipeline_stage started\n";

    // -------------------------------------------------------------------------
    // Test 1: simple transfer
    // -------------------------------------------------------------------------
    dut->i_s_valid = 1;
    dut->i_s_data  = 0xDEADBEEF;
    dut->i_s_last  = 1;
    dut->i_s_error = 0;
    dut->i_s_keep  = 0xF;
    dut->i_s_user  = 0x11;

    dut->i_m_ready = 1;

    tick(dut);

    if (check_word(dut, 0xDEADBEEF, 1, 0, 0xF, 0x11)) {
        delete dut;
        return 1;
    }

    dut->i_s_valid = 0;
    tick(dut);

    std::cout << "TB: simple transfer passed\n";

    // -------------------------------------------------------------------------
    // Test 2: backpressure hold
    // -------------------------------------------------------------------------
    dut->i_m_ready = 0;

    dut->i_s_valid = 1;
    dut->i_s_data  = 0x12345678;
    dut->i_s_last  = 0;
    dut->i_s_error = 0;
    dut->i_s_keep  = 0xF;
    dut->i_s_user  = 0x22;

    tick(dut);

    dut->i_s_valid = 0;

    for (int i = 0; i < 5; ++i) {
        tick(dut);

        if (check_word(dut, 0x12345678, 0, 0, 0xF, 0x22)) {
            delete dut;
            return 1;
        }
    }

    dut->i_m_ready = 1;
    tick(dut);

    dut->i_m_ready = 0;
    tick(dut);

    if (dut->o_m_valid) {
        std::cerr << "ERROR: output valid was not cleared after consume\n";
        delete dut;
        return 1;
    }

    std::cout << "TB: backpressure hold passed\n";

    // -------------------------------------------------------------------------
    // Test 3: burst transfer with always-ready sink
    // -------------------------------------------------------------------------
    dut->i_m_ready = 1;

    for (uint32_t i = 0; i < 16; ++i) {
        dut->i_s_valid = 1;
        dut->i_s_data  = i;
        dut->i_s_last  = (i == 15);
        dut->i_s_error = 0;
        dut->i_s_keep  = 0xF;
        dut->i_s_user  = static_cast<uint8_t>(i);

        tick(dut);

        if (check_word(dut, i, (i == 15), 0, 0xF, static_cast<uint8_t>(i))) {
            delete dut;
            return 1;
        }
    }

    dut->i_s_valid = 0;
    tick(dut);

    std::cout << "TB: burst transfer passed\n";

    std::cout << "TB: pf_pipeline_stage PASSED\n";

    delete dut;
    return 0;
}