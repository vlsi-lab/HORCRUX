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

#define NEWHOPE_Q     12289
#define NEWHOPE_QINV  12287   // -q^{-1} mod 2^16

static inline int16_t montgomery_reduce_newhope(int32_t a) {
  int16_t t;
  t = (int16_t)a * NEWHOPE_QINV;
  t = (a - (int32_t)t * NEWHOPE_Q) >> 16;
  return t;
}


int main(void) {

    // ---------------- NewHope ----------------
    const int32_t nh_inputs[20] = {
      0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 12289, 2*12289, -12289, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t nh_golden[20] = {
      0, -2304, 2303, -5813, 5812, -30465, -32768, 2304, -1, 0, 0, -1, -30465, -32768, -2608, 2607, 2304, -2305, -6134, 4406
    };


    int16_t got_sw[N_TESTS] = {0};

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_TESTS; i++) {
        got_sw[i] = montgomery_reduce_newhope(nh_inputs[i]);
    }
    vcd_disable();

    return 0;
}
