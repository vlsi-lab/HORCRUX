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


#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif

#define QINV_DILITHIUM 58728449
#define DILITHIUM_Q 8380417

#define NTESTS 50
#define HORCRUX_CUSTOM  1

int32_t montgomery_reduce(int64_t a) {
    uint32_t lo = (uint32_t)a;
    uint32_t t  = (lo << 25)
                + (lo << 24)
                + (lo << 23)
                + (lo << 13)
                +  lo;            // all adds/wrap mod 2^32

    // 2) m = t * Q  via shifts: Q = (1<<23) - (1<<13) + 1
    int64_t m = ((int64_t)t << 23)
              - ((int64_t)t << 13)
              +  (int64_t)t;

    int32_t result = (int32_t)((a - m) >> 32);

    return result;
}

// Dilithium Montgomery reduction function
int32_t DILITHIUM_montgomery_reduce(int64_t a) {
    int32_t t;
    uint64_t temp;

    temp = (uint64_t)a * (uint64_t)QINV_DILITHIUM;
    t = (int32_t)(temp);  // Cast to int32_t
    temp = (a - (int64_t)t * DILITHIUM_Q);
    t = (temp >> 32);  // Right shift by 32 bits

    return t;
}



int main() {

    // Dilithium test vectors
    int64_t dilithium_inputs[] = {
        97804164398, 103136797650, 92766174728, -38357775302, 140478453484, -83436606206, -23853284874, -59488954788, -60623242326, -108898320128, -152833334488, -8120266298, 101277675986, -100191327324, 83696030246, 85025263616, 90842490900, 150581466656, 19072326498, -113807437338, 146044946174, -5389849266, 69152710168, 159617566980, -81824315182, 79537311764, 30339347632, -79783848558, 29417174928, -121342488338, -16838761074, -175228891334, -14519560530, 93937151038, -167382951266, 164627053588, -145565809282, 37834393598, -14605363562, -151787200750, -93332499926, -43080132390, -8248005352, 87391815344, -3880404342, 17544705100, -97692167094, 138628860826, -81103636878, 147080585412, -122861672158, -85030217020, -56451636598, 49043736960, 107213994856, 68353323114, -86510277344, 28864954338, -165229269976, -37174835262, -83244263010, 92420611832, -37167699002, 68628111102, 139342654738, -83076057164, 108485340564, -152407677568, -70150947008, -122637593594, -11731633638, -154056783298, -11978968014, 118465610064, 67932073884, 33858993042, -71066319276, 125041673654, 13765425760, 14188773890, 30861553952, 15905883980, 70847404006, -156834635470, 92206230186, -128958137098, -159923082864, -77850257922, 161204419336, 103336361062, 158180911908, -17354964540, -3991100328, 168839084130, 25134075632, 20691124112, -27150698752, -79404997108, 133967665684, -62988324824
    };

    int32_t dilithium_golden_outputs[] = {
        3855344, -1797501, -1212808, -1775176, -2685418, -3385469, 1850281, -1930682, -426603, -696625, -92283, -1152836, 1772762, 3156166, 4061225, 3211401, 2303190, 1240060, 1203966, 2625878, -878311, 85013, 2043252, -558941, -2716888, -1466652, -1537013, -2560247, -1058858, 3583245, -1113878, 770974, -1126178, 3220564, 3812861, 1766250, 3493285, -1345592, -1089318, -2476894, -1905912, -992908, -720760, -3463254, 2519779, 56211, -2050253, -3776858, 1975527, 1021995, -2652145, 3235940, 1750131, -1454188, -2942217, 156255, 3888157, 1326774, -2130746, -1053351, -3746898, -1982760, 1731589, -1552594, -1771091, 3232857, -1241568, -1587143, 2272387, 990801, -2586240, 138096, 1456020, -3314973, 3371225, -2376611, -2975991, -1400365, 138143, -2257727, 1125059, -2585377, 1348031, -2850579, -1841464, -1771577, -1343417, -2210093, -1437435, 658774, 271430, -1433324, 1222530, 2958016, 3536318, -3968660, -2768400, 2820214, 4113150, -1558995
    };

    /* Test vector: raw prod values */
    uint64_t test_prod[101] = {
        18446744073709499922ULL, 0ULL, 0ULL, 0ULL, 51694ULL, 51694ULL, 0ULL, 0ULL, 0ULL, 25847ULL,
        51694ULL, 25847ULL, 25847ULL, 51694ULL, 18446744073709499922ULL, 0ULL, 51694ULL, 18446744073709525769ULL, 51694ULL, 0ULL,
        18446744073709525769ULL, 18446744073709525769ULL, 51694ULL, 25847ULL, 25847ULL, 51694ULL, 0ULL, 51694ULL, 18446744073709525769ULL, 0ULL,
        18446744073709525769ULL, 18446744073709499922ULL, 0ULL, 25847ULL, 18446744073709525769ULL, 0ULL, 18446744073709525769ULL, 25847ULL, 18446744073709499922ULL, 51694ULL,
        18446744073709525769ULL, 18446744073709499922ULL, 25847ULL, 51694ULL, 25847ULL, 51694ULL, 18446744073709525769ULL, 51694ULL, 25847ULL, 0ULL,
        0ULL, 18446744073709525769ULL, 18446744073709499922ULL, 18446744073709499922ULL, 0ULL, 0ULL, 25847ULL, 18446744073709525769ULL, 18446744073709525769ULL, 18446744073709525769ULL,
        51694ULL, 25847ULL, 18446744073709525769ULL, 18446744073709499922ULL, 18446744073709499922ULL, 51694ULL, 25847ULL, 18446744073709499922ULL, 18446744073709525769ULL, 51694ULL,
        51694ULL, 18446744073709525769ULL, 0ULL, 18446744073709525769ULL, 18446744073709525769ULL, 25847ULL, 0ULL, 18446744073709499922ULL, 0ULL, 18446744073709525769ULL,
        25847ULL, 0ULL, 18446744073709525769ULL, 18446744073709525769ULL, 18446744073709525769ULL, 18446744073709499922ULL, 51694ULL, 51694ULL, 25847ULL, 51694ULL,
        51694ULL, 0ULL, 18446744073709499922ULL, 18446744073709499922ULL, 0ULL, 51694ULL, 0ULL, 25847ULL, 18446744073709499922ULL, 0ULL, 0ULL
    };

    /* Test vector: t values */
    int32_t test_t[101] = {
        -1235971, 0, 0, 0, -7144446, -7144446, 0, 0, 0, -3572223,
        -7144446, -3572223, -3572223, -7144446, -1235971, 0, -7144446, -4808194, -7144446, 0,
        -4808194, -4808194, -7144446, -3572223, -3572223, -7144446, 0, -7144446, -4808194, 0,
        -4808194, -1235971, 0, -3572223, -4808194, 0, -4808194, -3572223, -1235971, -7144446,
        -4808194, -1235971, -3572223, -1235971, -3572223, -7144446, -4808194, -7144446, -3572223, 0,
        0, -4808194, -1235971, -1235971, 0, 0, -3572223, -4808194, -4808194, -4808194,
        -7144446, -3572223, -4808194, -1235971, -1235971, -7144446, -3572223, -1235971, -4808194, -7144446,
        -7144446, -4808194, 0, -4808194, -4808194, -3572223, 0, -1235971, 0, -4808194,
        -3572223, 0, -4808194, -4808194, -4808194, -1235971, -7144446, -7144446, -3572223, -7144446,
        -7144446, 0, -1235971, -1235971, 0, -7144446, 0, -3572223, -1235971, 0, 0
    };


    int all_tests_passed = 1;
    int all_tests_passed_hw = 1;

    int32_t result_dilithium[NTESTS] = {};
    int32_t result_dilithium_hw[NTESTS] = {};

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_dilithium_montg_hw, cycles_dilithium_montg;
    #endif

    
    // Dilithium tests
    printf("Dilithium Montgomery Reduction Tests.\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        result_dilithium[i] = montgomery_reduce((int64_t) test_prod[i]);
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_dilithium_montg);
        printf("cycles_dilithium_montg: %d\n", cycles_dilithium_montg);
    #endif
    for (int i = 0; i < NTESTS; i++) {

        if (result_dilithium[i] != test_t[i]) {
            printf("Test %d failed: expected %d, got %d\n", i, test_t[i], result_dilithium[i]);
            all_tests_passed = 0;
        }
    }
    if (all_tests_passed) {
        printf("Dilithium tests passed!\n");
    }

    printf("Dilithium-HW Montgomery Reduction Tests.\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        uint32_t temp2;
        temp2 = test_prod[i] >> 32;
        asm volatile (
            "mv a3, %[rs1]\n\t"
            "mv a2, %[rs2]\n\t"
            ".insn r 0x3b, 0x1, 0x6, %[rd], a3, a2\n\t"
            : [rd] "=r" (result_dilithium_hw[i])
            : [rs1] "r" ((int64_t) test_prod[i]), [rs2] "r" (temp2)
            : "a2", "a3", "cc" );
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_dilithium_montg_hw);
        printf("cycles_dilithium_montg: %d\n", cycles_dilithium_montg_hw);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (result_dilithium_hw[i] != test_t[i]) {
            printf("Test %d failed: expected %d, got %d\n", i, test_t[i], result_dilithium_hw[i]);
            all_tests_passed_hw = 0;
        }
    }
    if (all_tests_passed_hw) {
        printf("Dilithium tests passed!\n");
    }
    
    return 0;
}