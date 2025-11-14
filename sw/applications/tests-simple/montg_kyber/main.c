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
// ---------------------- ML-KEM (Kyber) test --------------------------
#define KYBER_Q     3329
#define KYBER_QINV  62209    // matches Kyber reference

// Exact body as requested (for ML-KEM)
static inline int16_t montgomery_reduce_kem(int32_t a) {
  int16_t t;
  t = (int16_t)a * KYBER_QINV;
  t = (a - (int32_t)t * KYBER_Q) >> 16;
  return t;
}


int main(void) {

    // ---------------- ML-KEM (Kyber) --------------------
    const int32_t kem_inputs[N_TESTS] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF,      0xFFFF0000,      3329,            2*3329,         -3329, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t kem_golden[N_TESTS] = {
        0, 169, -169, 77, -77, 32599, -32768, -168, -1, 0, 0, 0, 32599, -32768, -309, 309, -168, 168, -978, 1133
    };

    int16_t got_sw[N_TESTS] = {0};

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_TESTS; i++) {
        got_sw[i] = montgomery_reduce_kem(kem_inputs[i]);
    }
    vcd_disable();

    return 0;
}
