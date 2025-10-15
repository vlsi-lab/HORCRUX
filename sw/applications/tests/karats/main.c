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

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif

#define NUM_TESTS 1


static void karats_assembly(uint32_t  * const res, const uint32_t  * const a, const uint32_t  * const b)
{
    asm inline (
      ".insn r 0x3b, 0x001, 1, %[res0], %[a0], %[b0]\n\t"  // a0 * b0
      ".insn r 0x3b, 0x001, 2, %[res3], %[a1], %[b1]\n\t"  // a0 * b0
      ".insn r 0x3b, 0x001, 3, %[res1], %[a0], %[b0]\n\t"  // a0 * b0
      ".insn r 0x3b, 0x001, 4, %[res2], x0, x0\n\t"  // a0 * b0
      : [res0] "=&r" (res[0]), [res1] "=&r" (res[1]), [res2] "=&r" (res[2]), [res3] "=&r" (res[3])  
      : [a0] "r" (a[0]), [b0] "r" (b[0]), [a1] "r" (a[1]), [b1] "r" (b[1])     
      : "cc"
   );

}

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
    uint64_t out_hw[NUM_TESTS][2] = {0};

    uint32_t error = 0;
    uint32_t error_hw = 0;


    #if PERF_CNT_CYCLES
        unsigned int cycles, cycles_hw;
    #endif

    printf("Test started!\n");
     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NUM_TESTS; i++) {
        //printf("  a[%zu] = %" PRIu64 " (0x%016" PRIx64 ")\n", i,  tests[i].a,  tests[i].a);
        //printf("  b[%zu] = %" PRIu64 " (0x%016" PRIx64 ")\n", i,  tests[i].b, tests[i].b);
        gf2_mul64_hw(out[i], a[i], b[i]);
    }

    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("Number of clock cycles for test-1 : %d\n", cycles);
    #endif

    // Comparison
    for (int i = 0; i < NUM_TESTS; i++) {
        if (out[i][0] != out_ref[i].o[0] && out[i][1] != out_ref[i].o[1]) {
            printf("Test %d: Expected out[%d] = 0x%08X-0x%08X, but got 0x%08X-0x%08X\n",
                    i, i, out_ref[i].o[0], out_ref[i].o[1], out[i][0], out[i][1]);
            error += 1;
        }
    }
    
    if (error == 0){
        printf("All tests passed!\n");
    } else {
        printf("Test failed with %d errors.\n", error);
    }

    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    // Hardware test
    for (int i = 0; i < NUM_TESTS; i++) {
        uint32_t a2[2], b2[2];
        uint32_t c2[4];
        a2[0] = a[i] & 0xFFFFFFFF;
        a2[1] = a[i] >> 32;
        b2[0] = b[i] & 0xFFFFFFFF;
        b2[1] = b[i] >> 32;
        karats_assembly(c2, a2, b2);
        out_hw[i][0] = ((uint64_t)c2[1] << 32) | c2[0];
        out_hw[i][1] = ((uint64_t)c2[3] << 32) | c2[2];
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_hw);
        printf("Number of clock cycles for test-hw : %d\n", cycles_hw);
    #endif
    // Comparison
    for (int i = 0; i < NUM_TESTS; i++) {
        if (out_hw[i][0] != out_ref[i].o[0] && out_hw[i][1] != out_ref[i].o[1]) {
            printf("Test %d: Expected out[%d] = 0x%08X-0x%08X, but got 0x%08X-0x%08X\n",
                    i, i, out_ref[i].o[0], out_ref[i].o[1], out_hw[i][0], out_hw[i][1]);
            error_hw += 1;
        }
    }
    
    if (error_hw == 0){
        printf("All tests passed!\n");
    } else {
        printf("Test failed with %d errors.\n", error_hw);
    }


    return 0;
}
