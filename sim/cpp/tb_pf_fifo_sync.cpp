#include "Vtb_pf_fifo_sync.h"
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

static void eval(Vtb_pf_fifo_sync* dut) {
    dut->eval();
}

static void tick(Vtb_pf_fifo_sync* dut) {
    dut->i_clk = 0;
    eval(dut);
    sim_time++;

    dut->i_clk = 1;
    eval(dut);
    sim_time++;
}

static void reset_dut(Vtb_pf_fifo_sync* dut) {
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

static void drive_word(Vtb_pf_fifo_sync* dut, const StreamWord& word) {
    dut->i_s_valid = 1;
    dut->i_s_data  = word.data;
    dut->i_s_last  = word.last;
    dut->i_s_error = word.error;
    dut->i_s_keep  = word.keep;
    dut->i_s_user  = word.user;
}

static void idle_input(Vtb_pf_fifo_sync* dut) {
    dut->i_s_valid = 0;
    dut->i_s_data  = 0;
    dut->i_s_last  = 0;
    dut->i_s_error = 0;
    dut->i_s_keep  = 0;
    dut->i_s_user  = 0;
}

static int check_output(Vtb_pf_fifo_sync* dut, const StreamWord& exp) {
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

    auto* dut = new Vtb_pf_fifo_sync;

    reset_dut(dut);

    std::cout << "TB: pf_fifo_sync started\n";

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
    // Test 2: fill FIFO while output is stalled
    // -------------------------------------------------------------------------
    {
        std::queue<StreamWord> expected;

        dut->i_m_ready = 0;

        for (uint32_t i = 0; i < 8; ++i) {
            StreamWord word {
                .data  = 0x1000 + i,
                .last  = static_cast<uint8_t>(i == 7),
                .error = 0,
                .keep  = 0xF,
                .user  = static_cast<uint8_t>(0x20 + i)
            };

            drive_word(dut, word);
            expected.push(word);

            tick(dut);

            if (!dut->o_s_ready && i != 7) {
                std::cerr << "ERROR: FIFO became full too early\n";
                delete dut;
                return 1;
            }
        }

        idle_input(dut);
        tick(dut);

        if (dut->o_s_ready) {
            std::cerr << "ERROR: FIFO is full but input ready is high\n";
            delete dut;
            return 1;
        }

        std::cout << "TB: fill/full backpressure passed\n";

        // Drain FIFO
        dut->i_m_ready = 1;
        dut->eval();

        while (!expected.empty()) {
            if (check_output(dut, expected.front())) {
                delete dut;
                return 1;
            }

            expected.pop();

            tick(dut);
        }

        dut->eval();

        if (dut->o_m_valid) {
            std::cerr << "ERROR: FIFO output valid is high after drain\n";
            delete dut;
            return 1;
        }

        std::cout << "TB: drain/order check passed\n";
    }

    // -------------------------------------------------------------------------
    // Test 3: simultaneous push/pop burst
    // -------------------------------------------------------------------------
    {
        dut->i_m_ready = 1;

        for (uint32_t i = 0; i < 16; ++i) {
            StreamWord word {
                .data  = 0x2000 + i,
                .last  = static_cast<uint8_t>(i == 15),
                .error = 0,
                .keep  = 0xF,
                .user  = static_cast<uint8_t>(0x40 + i)
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

        std::cout << "TB: simultaneous push/pop burst passed\n";
    }

    std::cout << "TB: pf_fifo_sync PASSED\n";

    delete dut;
    return 0;
}
