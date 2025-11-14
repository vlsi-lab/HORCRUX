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


#define N_TESTS  20
// ---------------------- ML-DSA (Dilithium) test ----------------------
#define DSA_Q     8380417
#define DSA_QINV  58728449  // matches Dilithium reference

// Exact body as requested (for ML-DSA)
static inline int32_t montgomery_reduce_dsa(int64_t a) {
  int32_t t;
  int32_t temp;
  uint32_t temp2;
  temp2 = a >> 32;
  
  printf("montgomery_reduce_dsa input a:%d\n", a);
  printf("montgomery_reduce_dsa input a: %d, temp2:%d\n", (int64_t) a, temp2);

  t = (int64_t)(int32_t)a * DSA_QINV;
  printf("montgomery_reduce_dsa intermediate t (after mult): %d\n", t);
  temp = (int64_t)t * DSA_Q;
printf("montgomery_reduce_dsa intermediate temp (t*Q): %d\n", temp);
  t = (a - (int64_t)t * DSA_Q) >> 32;
  
  printf("montgomery_reduce_dsa output t: %d\n", t);
  return t;
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

    int dsa_ok_sw = 1;
    unsigned cycles;


    printf("Dilithium Montgomery Reduction Tests.\n");


    for (int i = 0; i < N_TESTS; i++) {
        got_sw[i] = montgomery_reduce_dsa((int64_t) dsa_inputs[i]);
    }

    for (int i = 0; i < N_TESTS; i++) {
        if (got_sw[i] != dsa_golden[i]) {
            printf("[DSA] Test %2d FAIL: a=%d  exp=%d  got=%d\n", i, dsa_inputs[i], dsa_golden[i], got_sw[i]);
            dsa_ok_sw = 0;
            }
        }
        
    if (dsa_ok_sw) printf("[DSA] All Montgomery tests PASSED.\n");


    return 0;
}
