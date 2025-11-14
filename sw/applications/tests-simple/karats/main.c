///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "vcd_util.h"


#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif

#define NUM_TESTS 10



static inline void gf2_mul64_hw(uint64_t c[2], uint64_t a, uint64_t b) {
    uint64_t lo = 0, hi = 0;
    for (int k = 0; k < 64; ++k) {
        uint64_t mask = -((a >> k) & 1ULL);
        if (k == 0) {
            lo ^= b & mask;
        } else {
            lo ^= (b << k) & mask;
            hi ^= (b >> (64 - k)) & mask;
        }
    }
    c[0] = lo; c[1] = hi;
}



typedef struct {
    uint64_t o[2];
} ref_out_t;

int main() {

    static const uint64_t a[NUM_TESTS] = {
        5497574916096ULL,
        5497574916096ULL,
        5497574916096ULL,
        71485435807752ULL,
        71485435807752ULL,
        71485435807752ULL,
        71485435807752ULL,
        71485435807752ULL,
        687194898688ULL,
        687194898688ULL,
        687194898688ULL,
        687194898688ULL,
        687194898688ULL,
        687194898688ULL,
        687194898688ULL,
        687194898688ULL,
        97953309142286360ULL,
        97953309142286360ULL,
        97953309142286360ULL,
        97953309142286360ULL,
        97953309142286360ULL,
        97953309142286360ULL
    };

    static const uint64_t b[NUM_TESTS] = {
        15406822031752743863ULL,
        15598386587604540168ULL,
        4008006062614233429ULL,
        4518150788292776356ULL,
        6451270313248065824ULL,
        10374921086043284589ULL,
        266275880106683577ULL,
        3327477710794529831ULL,
        15406822031752743863ULL,
        15598386587604540168ULL,
        4008006062614233429ULL,
        2817406080985604755ULL,
        4518150788292776356ULL,
        6451270313248065824ULL,
        10374921086043284589ULL,
        266275880106683577ULL,
        15406822031752743863ULL,
        15598386587604540168ULL,
        4008006062614233429ULL,
        2817406080985604755ULL,
        4518150788292776356ULL,
        6451270313248065824ULL
    };

    static const ref_out_t out_ref[NUM_TESTS] = {
        { { 7617033702622625792ULL, 3859310107414ULL } },
        { { 2058227334300827648ULL, 4095690037539ULL } },
        { { 1411802347721457664ULL, 1004523930961ULL } },
        { { 11007503173114506528ULL, 17122523712031ULL } },
        { { 6501131721456306432ULL, 24434078404812ULL } },
        { { 17949928197797071720ULL, 38976878686786ULL } },
        { { 7843678005763026376ULL, 1027519063669ULL } },
        { { 6598212823198718264ULL, 12807356743702ULL } },
        { { 17545759861175072512ULL, 482412251671ULL } },
        { { 1604793170553145344ULL, 511962936350ULL } },
        { { 12491402087724963072ULL, 125565912138ULL } },
        { { 17668878068842009344ULL, 100656628063ULL } },
        { { 495537825888445440ULL, 105489834440ULL } },
        { { 8937192465690599424ULL, 171583873934ULL } },
        { { 11985729875605351680ULL, 369416275482ULL } },
        { { 8276023098216462592ULL, 7235196721ULL } },
        { { 2969637718231541448ULL, 65303043975790862ULL } },
        { { 14104068656480348352ULL, 64812997247430501ULL } },
        { { 10107005684548771832ULL, 15872464487117202ULL } },
        { { 4674098182003834280ULL, 12677423774487064ULL } },
        { { 8480183769108944736ULL, 14631434117763316ULL } },
        { { 14040804483968850688ULL, 20781036951154191ULL } }
    };
        

    uint64_t out[NUM_TESTS][2] = {0};

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < NUM_TESTS; i++) {
        gf2_mul64_hw(out[i], a[i], b[i]);
    }
    vcd_disable();


    return 0;
}
