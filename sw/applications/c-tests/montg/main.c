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

// ---------------- NewHope (q = 12289) ----------------
#define NEWHOPE_Q     12289
#define NEWHOPE_QINV  12287   // -q^{-1} mod 2^16

static inline int16_t montgomery_reduce_newhope(int32_t a) {
  int16_t t;
  t = (int16_t)a * NEWHOPE_QINV;
  t = (a - (int32_t)t * NEWHOPE_Q) >> 16;
  return t;
}

// ---------------- Falcon (q = 12289) ----------------
#define FALCON_Q     122
#define FALCON_QINV  12287    // -q^{-1} mod 2^16

static inline int16_t montgomery_reduce_falcon(int32_t a) {
  int16_t t;
  t = (int16_t)a * FALCON_QINV;
  t = (a - (int32_t)t * FALCON_Q) >> 16;
  return t;
}

// ---------------- NTRU Prime (q = 4591) ----------------
#define NTRUP_Q     4591
#define NTRUP_QINV  49905   // -q^{-1} mod 2^16

static inline int16_t montgomery_reduce_ntrup(int32_t a) {
  int16_t t;
  t = (int16_t)a * NTRUP_QINV;
  t = (a - (int32_t)t * NTRUP_Q) >> 16;
  return t;
}

int main(void) {

    // ---------------- ML-DSA (Dilithium) ----------------
    const int64_t dsa_inputs[20] = {
        0LL, 1LL, -1LL, 123456789LL, -123456789LL, 2147483647LL, -2147483648LL, 1099511640121LL,         -1099511620987LL,        8380417LL,               16760834LL,              -8380417LL,              9223372036854775807LL,   (-9223372036854775807LL - 1LL),  999999999999LL, -999999999999LL, 36028797018963973LL, -36028797018963963LL,   -8589934587LL,          8589934587LL          
    };
    const int32_t dsa_golden[20] = {
        0, -114592, 114592, -2438631, 2438631, -4075616, 4190208, 1652489, 1413437, 0, 0, 0, -2147369056, -2147483648, -3769131, 3769131, 7815648, -8961568, -572962, 572962
    };

    // ---------------- ML-KEM (Kyber) --------------------
    const int32_t kem_inputs[20] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF,      0xFFFF0000,      3329,            2*3329,         -3329, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t kem_golden[20] = {
        0, 169, -169, 77, -77, 32599, -32768, -168, -1, 0, 0, 0, 32599, -32768, -309, 309, -168, 168, -978, 1133
    };

    // ---------------- NewHope ----------------
    const int32_t nh_inputs[20] = {
      0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 12289, 2*12289, -12289, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t nh_golden[20] = {
      0, -2304, 2303, -5813, 5812, -30465, -32768, 2304, -1, 0, 0, -1, -30465, -32768, -2608, 2607, 2304, -2305, -6134, 4406
    };

    // ---------------- Falcon  ----------------
    const int32_t fal_inputs[20] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 12289, 2*12289, -12289, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t fal_golden[20] = { // identical to NewHope
        0, -2304, 2303, -5813, 5812, -30465, -32768, 2304, -1, 0, 0, -1, -30465, -32768, -2608, 2607, 2304, -2305, -6134, 4406
    };

    
    // ---------------- NTRU Prime ----------------
    const int32_t ntrup_inputs[20] = {
        0, 1, -1, 1234567, -1234567, 2147483647, -2147483648, 0x0000FFFF, 0xFFFF0000, 4591, 2*4591, -4591, 2147483647, -2147483648, 9999999, -9999999, 65535, -65535, 12345, -54321
    };
    const int16_t ntrup_golden[20] = {
        0, 1095, -1096, -1185, 1184, 31672, -32768, -1095, -1, 0, 0, -1, 31672, -32768, 519, -520, -1095, 1094, 1871, -501
    };

    int dsa_ok = 1;
    for (int i = 0; i < 20; i++) {
        int32_t r = montgomery_reduce_dsa(dsa_inputs[i]);
        if (r != dsa_golden[i]) {
        printf("[DSA] Test %2d FAIL: a=%lld  exp=%d  got=%d\n",
                i, (long long)dsa_inputs[i], dsa_golden[i], r);
        dsa_ok = 0;
        }
    }
    if (dsa_ok) printf("[DSA] All 20 Montgomery tests PASSED.\n");

    int kem_ok = 1;
    for (int i = 0; i < 20; i++) {
        int16_t r = montgomery_reduce_kem(kem_inputs[i]);
        if (r != kem_golden[i]) {
        printf("[KEM] Test %2d FAIL: a=%d  exp=%d  got=%d\n",
                i, kem_inputs[i], kem_golden[i], r);
        kem_ok = 0;
        }
    }
    if (kem_ok) printf("[KEM] All 20 Montgomery tests PASSED.\n");

    int nh_ok = 1;
    for (int i = 0; i < 20; i++) {
        int16_t r = montgomery_reduce_newhope(nh_inputs[i]);
        if (r != nh_golden[i]) {
        printf("[NEWHOPE] Test %2d FAIL: a=%d  exp=%d  got=%d\n",
                i, nh_inputs[i], nh_golden[i], r);
        nh_ok = 0;
        }
    }
    if (nh_ok) printf("[NEWHOPE] All 20 Montgomery tests PASSED.\n");

    int fal_ok = 1;
    for (int i = 0; i < 20; i++) {
        int16_t r = montgomery_reduce_falcon(fal_inputs[i]);
        if (r != fal_golden[i]) {
        printf("[FALCON]  Test %2d FAIL: a=%d  exp=%d  got=%d\n",
                i, fal_inputs[i], fal_golden[i], r);
        fal_ok = 0;
        }
    }
    if (fal_ok) printf("[FALCON]  All 20 Montgomery tests PASSED.\n");


    int ntrup_ok = 1;
    for (int i = 0; i < 20; i++) {
        int16_t r = montgomery_reduce_ntrup(ntrup_inputs[i]);
        if (r != ntrup_golden[i]) {
        printf("[NTRU-P]  Test %2d FAIL: a=%d  exp=%d  got=%d\n",
                i, ntrup_inputs[i], ntrup_golden[i], r);
        ntrup_ok = 0;
        }
    }
    if (ntrup_ok) printf("[NTRU-P]  All 20 Montgomery tests PASSED.\n");

    return 0;
}
