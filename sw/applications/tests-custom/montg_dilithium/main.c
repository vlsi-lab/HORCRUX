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



int main(void) {

    // ---------------- ML-DSA (Dilithium) ----------------
    int64_t dsa_inputs[20] = {
        123456789LL, 0LL, 1LL, -1LL,  -123456789LL, 2147483647LL, -2147483648LL, 1099511640121LL, -1099511620987LL, 8380417LL, 16760834LL, -8380417LL,              9223372036854775807LL,   (-9223372036854775807LL - 1LL),  999999999999LL, -999999999999LL, 36028797018963973LL, -36028797018963963LL,   -8589934587LL,          8589934587LL          
    };
    
    int32_t dsa_golden[20] = {
        -2438631, 0, -114592, 114592, 2438631, -4075616, 4190208, 1652489, 1413437, 0, 0, 0, -2147369056, -2147483648, -3769131, 3769131, 7815648, -8961568, -572962, 572962
    };
    

    int32_t got_hw[N_TESTS] = {};

    
    if (vcd_init() != 0)
    return 1;

    vcd_enable();
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
    }
    vcd_disable();

    return 0;
}
