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


static inline __attribute__((always_inline))
int16_t montgomery_reduce_kem_hw(int32_t a)
{
    int32_t out;
    asm volatile (
        "addi t0, %1, 0\n\t"                // t0 := rs1 copy
        ".insn r 0x3b, 0x7, 0, %0, t0, x0\n\t"
        : "=&r"(out)                        // rd (early-clobber)
        : "r"(a)                            // rs1 (via t0)
        : "t0", "cc"                        // we modify t0
    );
    return (int16_t)out;
}


int main(void) {

    // ---------------- ML-KEM (Kyber) --------------------
    const int32_t kem_inputs[N_TESTS] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF,      0xFFFF0000,      3329,            2*3329,         -3329, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t kem_golden[N_TESTS] = {
        0, 169, -169, 77, -77, 32599, -32768, -168, -1, 0, 0, 0, 32599, -32768, -309, 309, -168, 168, -978, 1133
    };


    int16_t got_hw[N_TESTS] = {0};

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_TESTS; i++) {
        //asm volatile (
        //    "mv a3, %[a]\n\t"
        //    ".insn r 0x3b, 0x7, 0, %[res], %[a], x0\n\t"
        //    : [res] "=r" (got_hw[i])
        //    : [a] "r" (kem_inputs[i])
        //    : "a3", "cc" );
        got_hw[i] = montgomery_reduce_kem_hw(kem_inputs[i]);
    }
    vcd_disable();
  
    return 0;
}
