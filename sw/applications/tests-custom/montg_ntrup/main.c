///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// Generic Montgomery reduction tests for ML-DSA (Dilithium) and ML-KEM (Kyber)
//
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include "vcd_util.h"

#ifdef PERF_CNT_CYCLES
  #include "core_v_mini_mcu.h"
  #include "csr.h"
#endif


#define N_TESTS  20

// ---------------- NTRU Prime (q = 4591) ----------------
#define NTRUP_Q     4591
#define NTRUP_QINV  49905   // -q^{-1} mod 2^16

static inline int16_t montgomery_reduce_ntrup(int32_t a) {
    int32_t out;
    asm volatile (
        "addi t0, %1, 0\n\t"                // t0 := rs1 copy
        ".insn r 0x3b, 0x7, 3, %0, t0, x0\n\t"
        : "=&r"(out)                        // rd (early-clobber)
        : "r"(a)                            // rs1 (via t0)
        : "t0", "cc"                        // we modify t0
    );
    return (int16_t)out;
}


int main(void) {

    // ---------------- NTRU Prime ----------------
    const int32_t ntrup_inputs[20] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 4591, 2*4591, -4591, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t ntrup_golden[20] = {
        0, 1095, -1096, -1185, 1184, 31672, -32768, -1095, -1, 0, 0, -1, 31672, -32768, 519, -520, -1095, 1094, 1871, -501
    };

    int16_t got_hw[N_TESTS] = {0};

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_TESTS; i++) {
        //asm volatile (
        //    "mv a3, %[a]\n\t"
        //    ".insn r 0x3b, 0x7, 3, %[res], %[a], x0\n\t"
        //    : [res] "=r" (got_hw[i])
        //    : [a] "r" (ntrup_inputs[i])
        //    : "a3", "cc" );
        got_hw[i] = montgomery_reduce_ntrup(ntrup_inputs[i]);
    }
    vcd_disable();

    return 0;
}
