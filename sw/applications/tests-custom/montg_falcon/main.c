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

static inline __attribute__((always_inline))
int16_t montgomery_reduce_falcon_hw(int32_t a)
{
    int32_t out;
    asm volatile (
        "addi t0, %1, 0\n\t"                // t0 := rs1 copy
        ".insn r 0x3b, 0x7, 2, %0, t0, x0\n\t"
        : "=&r"(out)                        // rd (early-clobber)
        : "r"(a)                            // rs1 (via t0)
        : "t0", "cc"                        // we modify t0
    );
    return (int16_t)out;
}

#define N_TESTS  20


int main(void) {

    // ---------------- Falcon  ----------------
    const int32_t fal_inputs[20] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 12289, 2*12289, -12289, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t fal_golden[20] = { 
        0, -2304, 2303, -5813, 5812, -30465, -32768, 2304, -1, 0, 0, -1, -30465, -32768, -2608, 2607, 2304, -2305, -6134, 4406
    };


    int16_t got_hw[N_TESTS] = {0};


    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_TESTS; i++) {
        //asm volatile (
        //    "mv a3, %[a]\n\t"
        //    ".insn r 0x3b, 0x7, 2, %[res], %[a], x0\n\t"
        //    : [res] "=r" (got_hw[i])
        //    : [a] "r" (fal_inputs[i])
        //    : "a3", "cc" );
        got_hw[i] = montgomery_reduce_falcon_hw(fal_inputs[i]);
    }
    vcd_disable();
    return 0;
}
