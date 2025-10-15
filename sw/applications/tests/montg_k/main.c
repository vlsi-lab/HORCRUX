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


#define QINV_KYBER 62209
#define KYBER_Q 3329

#define NTESTS 100


void KYBER_montgomery_reduce_asm(int32_t a, int32_t *t) {

    asm volatile (
            "mv a3, %[rs1]\n\t"
            ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
            : [rd] "=r" (*t)
            : [rs1] "r" (a)
            : "a3", "cc" );
}

// Kyber Montgomery reduction function
int16_t KYBER_montgomery_reduce(int32_t a) {
    int16_t t;

    t = (int16_t)a * QINV_KYBER;
    t = (a - (int32_t)t * KYBER_Q) >> 16;
    return t;
}

int16_t montgomery_reduce(int32_t a) {

    uint32_t lo = (uint32_t)a & 0xFFFF;

    // QINV = 0xF301 = (1<<15)+(1<<14)+(1<<13)+(1<<12)+(1<<9)+(1<<8)+(1<<0)
    uint32_t t32 = (lo << 15)
                 + (lo << 14)
                 + (lo << 13)
                 + (lo << 12)
                 + (lo <<  9)
                 + (lo <<  8)
                 +  lo;
    // Mask back down to 16 bits
    uint16_t t = (uint16_t)t32;

    // Step 2: m = t * q using only shifts and adds
    //    t*q = t*(2^11 + 2^10 + 2^8 + 1)
    int32_t m  = ( (int32_t)t << 11 )
               + ( (int32_t)t << 10 )
               + ( (int32_t)t <<  8 )
               +  (int32_t)t;
    // Step 3: (a + m) >> 16
    int16_t result = (int16_t)((a - m) >> 16);

    return result;
}

int main() {
    // Kyber test vectors
    int32_t kyber_inputs[100] = {
        1854963, -584496, 2053854, -144771, 1178463, 1274526, -818565, 324720, -1434180, -1669602, 2068737, 2171565, -543906, -18942, -1883376, 254364, -257070, 722502, -798270, 1144638, 821271, -987690, -2182389, -2248686, -1007985, 388311, 36531, 296307, -868626, 1384119, 717090, -212421, 2172918, -167772, -37884, -823977, -966042, 550671, -1373295, -1928025, -837507, -2049795, 1388178, 161007, -159654, 1070223, -730620, 1921260, -734679, 676500, 198891, -2102562, 2204037, 1365177, 2118798, 1316469, 560142, -924099, 1779195, 354486, 2061972, -556083, -1003926, -856449, 1842786, 1156815, -2075502, -1488300, -519552, -17589, -64944, -1254231, -1734546, 1181169, -1290762, 1070223, 1815726, -920040, 1189287, 886215, -1485594, 1459887, 124476, 749562, 1075635, -1831962, -1212288, 1486947, -177243, 1523478, 1135167, 776622, -322014, -945747, 81180, 119064, -1125696, -217833, -106887, 1560009
    };

    int16_t kyber_golden_outputs[100] = {
        146, 1593, -188, -1478, -507, -1393, -890, -885, 1412, -27, 1644, -1133, 234, 1300, -1525, 139, -1380, -1553, 95, -1039, -1198, -221, -502, 719, -1206, -18, -1556, 1065, 1119, 597, -706, 787, 1152, -375, -729, -43, -280, 1204, 1038, -363, 410, 385, 794, -1063, 19, -212, 1159, -1075, 962, 653, -334, 1153, 443, -1432, -365, -467, 554, 646, -1312, -550, 206, -357, -1009, -1619, -445, -448, 247, -105, 1416, 256, 177, -951, 150, 734, 605, -212, 461, 843, 1128, -1375, 1136, -1274, 493, 870, -1059, -1249, -25, 1149, 275, -407, -389, -36, -1203, 705, 611, 1340, -261, 1634, -749, 1366
    };

    int all_tests_passed = 1;
    int all_tests_passed_hw = 1;

    int16_t result_kyber[NTESTS] = {};
    int16_t result_kyber_hw[NTESTS] = {};

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_kyber_montg, cycles_kyber_montg_hw;
    #endif

    // Kyber tests
    printf("Kyber Montgomery Reduction Tests.\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif


    for (int i = 0; i < NTESTS; i++) {
            result_kyber[i] = montgomery_reduce(kyber_inputs[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_kyber_montg);
        printf("cycles_kyber_montg: %d\n", cycles_kyber_montg);
    #endif

    //for (int j = 1; j < NTESTS; j++) {
    //    if (result_kyber[j] != kyber_golden_outputs[j]) {
    //        printf("Test %d failed: expected %d, got %d\n", j, kyber_golden_outputs[j], result_kyber[j]);
    //        all_tests_passed = 0;
    //    }
    //}
    //if (all_tests_passed) {
    //    printf("Kyber (SW) tests passed!\n");
    //}
    

    // Kyber tests
    printf("Kyber Montgomery Reduction Tests HW.\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        KYBER_montgomery_reduce_asm(kyber_inputs[i], &result_kyber_hw[i]);
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_kyber_montg_hw);
        printf("cycles_kyber_montg_hw: %d\n", cycles_kyber_montg_hw);
    #endif
    for (int j = 1; j < NTESTS; j++) {
        if (result_kyber_hw[j] != result_kyber[j]) {
            printf("Test %d failed: expected %d, got %d\n", j, result_kyber[j], result_kyber_hw[j]);
            all_tests_passed_hw = 0;
        }
    }
    if (all_tests_passed_hw) {
        printf("Kyber HW-tests passed!\n");
    }

    
    return 0;
}