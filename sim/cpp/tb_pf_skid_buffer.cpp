#include "Vtb_pf_skid_buffer.h"
#include "verilated.h"

#include <cstdint>
#include <iostream>
#include <queue>

struct StreamWord {
    uint32_t data;
    uint8_t  last;
    uint8_t  error;
    uint8_t  keep;
    uint8_t  user;
};

static vluint64_t sim_time = 0;

static void eval(Vtb_pf_skid_buffer* dut) {
    dut->eval();
}

static void tick(Vtb_pf_skid_buffer* dut) {
    dut->i_clk = 0;
    eval(dut);
    sim_time++;

    dut->i_clk = 1;
    eval(dut);
    sim_time++;
}

static void reset_dut(Vtb_pf_skid_buffer* dut) {
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

static void drive_word(Vtb_pf_skid_buffer* dut, const StreamWord& word) {
    dut->i_s_valid = 1;
    dut->i_s_data  = word.data;
    dut->i_s_last  = word.last;
    dut->i_s_error = word.error;
    dut->i_s_keep  = word.keep;
    dut->i_s_user  = word.user;
}

static void idle_input(Vtb_pf_skid_buffer* dut) {
    dut->i_s_valid = 0;
    dut->i_s_data  = 0;
    dut->i_s_last  = 0;
    dut->i_s_error = 0;
    dut->i_s_keep  = 0;
    dut->i_s_user  = 0;
}

static int check_output(Vtb_pf_skid_buffer* dut, const StreamWord& exp) {
    if (!dut->o_m_valid) {
        std::cerr << "ERROR: output valid is low\n";
        return 1;
    }

    if (dut->o_m_data != exp.data) {
        std::cerr << "ERROR: data mismatch: expected 0x"
                  << std::hex << exp.data << ", got 0x"
                  << dut->o_m_data << std::dec << "\n";
        return 1;
    }

    if (dut->o_m_last != exp.last) {
        std::cerr << "ERROR: last mismatch\n";
        return 1;
    }

    if (dut->o_m_error != exp.error) {
        std::cerr << "ERROR: error mismatch\n";
        return 1;
    }

    if (dut->o_m_keep != exp.keep) {
        std::cerr << "ERROR: keep mismatch\n";
        return 1;
    }

    if (dut->o_m_user != exp.user) {
        std::cerr << "ERROR: user mismatch\n";
        return 1;
    }

    return 0;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    auto* dut = new Vtb_pf_skid_buffer;

    reset_dut(dut);

    std::cout << "TB: pf_skid_buffer started\n";

    // -------------------------------------------------------------------------
    // Test 1: simple transfer
    // -------------------------------------------------------------------------
    {
        StreamWord word {
            .data  = 0xDEADBEEF,
            .last  = 1,
            .error = 0,
            .keep  = 0xF,
            .user  = 0x11
        };

        dut->i_m_ready = 1;
        drive_word(dut, word);

        tick(dut);

        if (check_output(dut, word)) {
            delete dut;
            return 1;
        }

        idle_input(dut);
        tick(dut);

        std::cout << "TB: simple transfer passed\n";
    }

    // -------------------------------------------------------------------------
    // Test 2: backpressure hold
    // -------------------------------------------------------------------------
    {
        StreamWord word {
            .data  = 0x12345678,
            .last  = 0,
            .error = 0,
            .keep  = 0xF,
            .user  = 0x22
        };

        dut->i_m_ready = 0;
        drive_word(dut, word);

        tick(dut);

        idle_input(dut);

        for (int i = 0; i < 5; ++i) {
            tick(dut);

            if (check_output(dut, word)) {
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
    }

    // -------------------------------------------------------------------------
    // Test 3: burst transfer with always-ready sink
    // -------------------------------------------------------------------------
    {
        dut->i_m_ready = 1;

        for (uint32_t i = 0; i < 16; ++i) {
            StreamWord word {
                .data  = i,
                .last  = static_cast<uint8_t>(i == 15),
                .error = 0,
                .keep  = 0xF,
                .user  = static_cast<uint8_t>(i)
            };

            drive_word(dut, word);
            tick(dut);

            if (check_output(dut, word)) {
                delete dut;
                return 1;
            }
        }

        idle_input(dut);
        tick(dut);

        std::cout << "TB: burst transfer passed\n";
    }

        // -------------------------------------------------------------------------
    // Test 4: skid behavior under sudden downstream stall
    // -------------------------------------------------------------------------
    {
        StreamWord w0 {
            .data  = 0x000000A0,
            .last  = 0,
            .error = 0,
            .keep  = 0xF,
            .user  = 0xA0
        };

        StreamWord w1 {
            .data  = 0x000000A1,
            .last  = 1,
            .error = 0,
            .keep  = 0xF,
            .user  = 0xA1
        };

        // Cycle 1: output is ready, first word goes to main register
        dut->i_m_ready = 1;
        drive_word(dut, w0);
        tick(dut);

        if (check_output(dut, w0)) {
            delete dut;
            return 1;
        }

        // Cycle 2: downstream suddenly stalls.
        // Main register still holds w0, input w1 should be saved into skid.
        dut->i_m_ready = 0;
        drive_word(dut, w1);
        tick(dut);

        // During stall, output must still hold w0
        if (check_output(dut, w0)) {
            delete dut;
            return 1;
        }

        idle_input(dut);

        // Cycle 3: downstream resumes and consumes w0
        dut->i_m_ready = 1;
        tick(dut);

        // After consuming w0, skid data w1 moves to output
        if (check_output(dut, w1)) {
            delete dut;
            return 1;
        }

        // Cycle 4: consume w1
        tick(dut);

        if (dut->o_m_valid) {
            std::cerr << "ERROR: output valid was not cleared after skid consume\n";
            delete dut;
            return 1;
        }

        std::cout << "TB: skid stall behavior passed\n";
    }
    

    std::cout << "TB: pf_skid_buffer PASSED\n";

    delete dut;
    return 0;
}