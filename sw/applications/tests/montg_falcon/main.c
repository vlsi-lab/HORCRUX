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

#define FALCON_Q     12289
#define FALCON_QINV  12287    // -q^{-1} mod 2^16

static inline int16_t montgomery_reduce_falcon(int32_t a) {
  int16_t t;
  t = (int16_t)a * FALCON_QINV;
  t = (a - (int32_t)t * FALCON_Q) >> 16;
  return t;
}

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



int main(void) {

    // ---------------- Falcon  ----------------
    const int32_t fal_inputs[20] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 12289, 2*12289, -12289, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t fal_golden[20] = { 
        0, -2304, 2303, -5813, 5812, -30465, -32768, 2304, -1, 0, 0, -1, -30465, -32768, -2608, 2607, 2304, -2305, -6134, 4406
    };


    int16_t got_sw[N_TESTS] = {0};
    int16_t got_hw[N_TESTS] = {0};
    int kem_ok_sw = 1;
    int kem_ok_hw = 1;
    unsigned cycles;

    #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        got_sw[i] = montgomery_reduce_falcon(fal_inputs[i]);
    }

    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("montg_falcon [Executed %d tests]: %d\n", N_TESTS, cycles);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        if (got_sw[i] != fal_golden[i]) {
            printf("[FALCON] Test %2d FAIL: a=%d  exp=%d  got=%d\n",
                    i, fal_inputs[i], fal_golden[i], got_sw[i]);
            kem_ok_sw = 0;
            }
        }
        
    if (kem_ok_sw) printf("[FALCON] All 20 Montgomery tests PASSED.\n");


    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        //asm volatile (
        //    "mv a3, %[a]\n\t"
        //    ".insn r 0x3b, 0x7, 2, %[res], %[a], x0\n\t"
        //    : [res] "=r" (got_hw[i])
        //    : [a] "r" (fal_inputs[i])
        //    : "a3", "cc" );
        got_hw[i] = montgomery_reduce_falcon_hw(fal_inputs[i]);
    }

    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("montg_falcon_hw [Executed %d tests]: %d\n", N_TESTS, cycles);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        if (got_hw[i] != fal_golden[i]) {
            printf("[FALCON] Test %2d FAIL: a=%d  exp=%d  got=%d\n",
                    i, fal_inputs[i], fal_golden[i], got_hw[i]);
            kem_ok_hw = 0;
            }
        }
        
    if (kem_ok_hw) printf("[FALCON] All 20 Montgomery tests PASSED.\n");


    return 0;
}
