///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// Generic Montgomery reduction tests for ML-DSA (Dilithium) and ML-KEM (Kyber)
//
///////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "vcd_util.h"


#ifdef PERF_CNT_CYCLES
  #include "core_v_mini_mcu.h"
  #include "csr.h"
#endif


#define N_TESTS  20
// ---------------------- ML-DSA (Dilithium) test ----------------------
#define DSA_Q     8380417
#define DSA_QINV  58728449  // matches Dilithium reference

// Exact body as requested (for ML-DSA)
static inline int32_t montgomery_reduce_dsa(int64_t a) {
  int32_t t;
  t = (int64_t)(int32_t)a * DSA_QINV;
  t = (a - (int64_t)t * DSA_Q) >> 32;
  return t;
}


static inline __attribute__((always_inline))
int32_t dsa_custom_hw_u64(int64_t x)
{
    //uint32_t lo = (uint32_t)x;
    //uint32_t hi = (uint32_t)(x >> 32);
    uint32_t out;

    asm volatile (
        // .insn r <opcode>, <funct3>, <funct7>, rd, rs1, rs2
        ".insn r 0x3b, 0x7, 4, %0, %1, %2\n\t"
        : "=&r"(out)            // rd: early-clobber so it can't alias rs1/rs2
        : "r"((uint32_t)x), "r"((uint32_t)(x >> 32))      // rs1, rs2
        : "cc"
    );

    return out;
}


int main(void) {

    // ---------------- ML-DSA (Dilithium) ----------------
    int64_t dsa_inputs[20] = {
        123456789LL, 0LL, 1LL, -1LL,  -123456789LL, 2147483647LL, -2147483648LL, 1099511640121LL, -1099511620987LL, 8380417LL, 16760834LL, -8380417LL,              9223372036854775807LL,   (-9223372036854775807LL - 1LL),  999999999999LL, -999999999999LL, 36028797018963973LL, -36028797018963963LL,   -8589934587LL,          8589934587LL          
    };
    
    int32_t dsa_golden[20] = {
        -2438631, 0, -114592, 114592, 2438631, -4075616, 4190208, 1652489, 1413437, 0, 0, 0, -2147369056, -2147483648, -3769131, 3769131, 7815648, -8961568, -572962, 572962
    };
    

    int32_t got_sw[N_TESTS] = {};
    int32_t got_hw[N_TESTS] = {};

    int dsa_ok_sw = 1;
    int dsa_ok_hw = 1;
    unsigned cycles;

    #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    printf("Dilithium Montgomery Reduction Tests.\n");

    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        got_sw[i] = montgomery_reduce_dsa((int64_t) dsa_inputs[i]);
    }

    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("montg_dilithium [Executed %d tests]: %d\n", N_TESTS, cycles);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        if (got_sw[i] != dsa_golden[i]) {
            printf("[DSA] Test %2d FAIL: a=%d  exp=%d  got=%d\n", i, dsa_inputs[i], dsa_golden[i], got_sw[i]);
            dsa_ok_sw = 0;
            }
        }
        
    if (dsa_ok_sw) printf("[DSA] All Montgomery tests PASSED.\n");

    printf("Dilithium-HW Montgomery Reduction Tests.\n");
    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        uint32_t temp2;
        temp2 = dsa_inputs[i] >> 32;
        asm volatile (
            "mv a3, %[a]\n\t"
            "mv a4, %[b]\n\t"
            ".insn r 0x3b, 0x7, 4, %[res], a3, a4\n\t"
            : [res] "=r" (got_hw[i])
            : [a] "r" ((int64_t) dsa_inputs[i]), [b] "r" (temp2)
            : "a3", "a4", "cc" );
        //got_hw[i] = dsa_custom_hw_u64((int64_t)dsa_inputs[i]);
    }

    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("montg_dilithium_hw [Executed %d tests]: %d\n", N_TESTS, cycles);
    #endif

    for (int i = 0; i < N_TESTS; i++) {
        if (got_hw[i] != dsa_golden[i]) {
            printf("[DSA] Test %2d FAIL: exp=%d  got=%d\n", i, dsa_golden[i], got_hw[i]);
            dsa_ok_hw = 0;
            }
        }
        
    if (dsa_ok_hw) printf("[DSA] All Montgomery tests PASSED.\n");


    return 0;
}
