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
#include <string.h>
#include "vcd_util.h"

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif

#define KYBER_Q 3329

#define NTESTS 100

uint8_t compress1(int32_t d) {
    uint32_t d0;
    int32_t u;
    uint8_t res;

    u  = d;
    u += (u >> 15) & KYBER_Q;
    d0 = u << 4;
    d0 += 1665;
    d0 *= 80635;
    d0 >>= 28;
    res = d0 & 0xf;

    return res;
}

uint8_t compress2(int32_t d) {
    uint32_t d0;
    int32_t u;
    uint8_t res;

    u = d;
    u += (u >> 15) & KYBER_Q;
    d0 = u << 5;
    d0 += 1664;
    d0 *= 40318;
    d0 >>= 27;
    res = d0 & 0x1f;

    return res;
}

uint16_t compress3(int32_t d) {
    uint64_t d0;
    int16_t u;
    uint16_t res;

    u = d;
    u += ((int16_t)d >> 15) & KYBER_Q;
    /*      t[k]  = ((((uint32_t)t[k] << 10) + KYBER_Q/2)/ KYBER_Q) & 0x3ff; */
    d0 = u;
    d0 <<= 10;
    d0 += 1665;
    d0 *= 1290167;
    d0 >>= 32;
    res = d0 & 0x3ff;

    return res;
}
          
uint16_t compress4(int32_t d) {
    uint64_t d0;
    int16_t u;
    uint16_t res;

    u = d;
    u += ((int16_t)d >> 15) & KYBER_Q;
    d0 = u;
    d0 <<= 11;
    d0 += 1664;
    d0 *= 645084;
    d0 >>= 31;
    res = d0 & 0x7ff;

    return res;
}
      

int main(void) {
    /* your 12 test inputs */
    static const int32_t d_inputs[NTESTS] = {
        3232, 3123, 1323, 1363, 3104, 1233, 1388, 2627,
        1215, 1579, 1688,  574, 2765, 1440, 2023,  218,
        2564, 1729,  392,  273,  196, 2760, 1716, 2566,
        367, 2082, 1308, 1964,  931, 2340, 1548, 3129,
        1168,  508,  906,  594, 1549, 3054, 1091, 3327,
        1437, 1400, 1577, 1180, 1604, 1749, 1654,  925,
        968, 2832, 1279, 1493, 1080, 1758, 3193,  180,
        669,  115, 3010, 2324, 1670,  849, 3099, 2745,
        527,  719, 2677, 1029, 2122,  744, 2766, 2573,
        1931, 1669, 1885, 1550, 2101,  674, 2490, 1167,
        2588, 1158, 1178, 3258, 1008, 1123, 2494, 3050,
        1773,  723, 2552, 2386, 2987, 1875, 2917,  280,
        1552,  485,  844, 2225
    };

    /* golden outputs you logged, in the same order */
    static const int8_t golden[NTESTS] = {
        0, 15,  6,  7, 15,  6,  7, 13,
        6,  8,  8,  3, 13,  7, 10,  1,
        12,  8,  2,  1,  1, 13,  8, 12,
        2, 10,  6,  9,  4, 11,  7, 15,
        6,  2,  4,  3,  7, 15,  5,  0,
        7,  7,  8,  6,  8,  8,  8,  4,
        5, 14,  6,  7,  5,  8, 15,  1,
        3,  1, 14, 11,  8,  4, 15, 13,
        3,  3, 13,  5, 10,  4, 13, 12,
        9,  8,  9,  7, 10,  3, 12,  6,
        12,  6,  6,  0,  5,  5, 12, 15,
        9,  3, 12, 11, 14,  9, 14,  1,
        7,  2,  4, 11
    };

    static const int32_t d_inputs2[NTESTS] = {
        1559, 2223,  515, 2102,  933, 1157, 1037,  965,
        2666, 2022,  106,  293, 1058, 1693, 1055, 2917,
        230,  893, 2355,  734, 2299, 2544,   56,  779,
        1,  915, 3143, 1619, 1550, 2930, 1290,  457,
        1059, 1906, 2669, 3273,  784, 2844, 1175, 2906,
        2500,  515,  487,   74, 1458,  483,  902, 2018,
        2106, 2572, 1954, 2566, 2701, 2226,  628, 1709,
        1714,  548, 2942, 2328,  373, 1016, 2868, 2705,
        1810, 1405,  169,  879,  918, 1507,  255, 1335,
        466,  364, 1949,  221, 1667, 1304,  920, 2449,
        1865, 2363, 2721, 2488, 2609, 2078,  872,  859,
        697,  393, 2801,  309, 1577,  906,   59, 2794,
        591, 1009, 3138,  915
    };

    static const int16_t  golden2[NTESTS] = {
        15,  21,   5,  20,   9,  11,  10,   9,
        26,  19,   1,   3,  10,  16,  10,  28,
        2,   9,  23,   7,  22,  24,   1,   7,
        0,   9,  30,  16,  15,  28,  12,   4,
        10,  18,  26,  31,   8,  27,  11,  28,
        24,   5,   5,   1,  14,   5,   9,  19,
        20,  25,  19,  25,  26,  21,   6,  16,
        16,   5,  28,  22,   4,  10,  28,  26,
        17,  14,   2,   8,   9,  14,   2,  13,
        4,   3,  19,   2,  16,  13,   9,  24,
        18,  23,  26,  24,  25,  20,   8,   8,
        7,   4,  27,   3,  15,   9,   1,  27,
        6,  10,  30,   9
    };

    static const int32_t d_inputs3[NTESTS] = {
        1214, 1930,  704,  917, 2179, 1493,  844,   32,
        2653, 1943, 2704, 1007, 3102,  592,  300, 1655,
        1654, 1078, 2814, 2752, 2584, 1465, 2919,   45,
        1525, 1559, 1455,  376,  645,  227, 1149, 3310,
        2881, 1843, 1584,  426, 2384, 2006,  780,  362,
        2900,  772,  969, 2552,  717, 2624, 3128,  321,
        2912,  648, 2444, 2967, 2547, 3027, 2468,  798,
        3196, 1714, 2557,  629,  619, 2334, 1218,  247,
        2708,  738, 1328,  803,  648, 1658,  519,   41,
        950,  900, 2485, 1733, 2901, 2372, 2777,  859,
        1341,   96, 1925,  862,   43,  510,  944, 3099,
        1101, 3318, 2403, 1305, 3046, 2227,  494, 2447,
        134, 3064,  247, 1711
    };

    static const int16_t  golden3[NTESTS] = {
        373,  594,  217,  282,  670,  459,  260,   10,
        816,  598,  832,  310,  954,  182,   92,  509,
        509,  332,  866,  847,  795,  451,  898,   14,
        469,  480,  448,  116,  198,   70,  353, 1018,
        886,  567,  487,  131,  733,  617,  240,  111,
        892,  237,  298,  785,  221,  807,  962,   99,
        896,  199,  752,  913,  783,  931,  759,  245,
        983,  527,  787,  193,  190,  718,  375,   76,
        833,  227,  408,  247,  199,  510,  160,   13,
        292,  277,  764,  533,  892,  730,  854,  264,
        412,   30,  592,  265,   13,  157,  290,  953,
        339, 1021,  739,  401,  937,  685,  152,  753,
        41,  942,   76,  526
    };

    static const int32_t d_inputs4[NTESTS] = {
        2785, 1161, 2786,  762, 3067,  649,  948, 1248,
        1400,  881, 1620, 1988,  258,  379, 1920,  856,
        3236,  933,  726, 1972, 2506,  218, 1700, 1720,
        559, 2370, 1952,  523, 1854, 2511, 1538,  174,
        267, 1362,  112, 2174, 1118, 1954, 2564,  335,
        1273, 1859,  896, 1227,  789, 1236, 2429, 1862,
        368, 1544, 2427, 2198, 1345,  125, 2168, 2945,
        2584,  329, 1181, 2495, 3180, 1447,  561, 2185,
        2396, 1530, 1612, 2227,  371, 2357, 3247, 3328,
        250, 1631, 1341, 2189, 1555, 1869, 1242,  404,
        1186, 2855, 3035,  485, 2646, 1836, 2388, 1925,
        2010, 2549, 1568, 2880,  605, 2000,  857,  407,
        959,  650, 2512
    };

    static const int16_t  golden4[NTESTS] = {
        1713,  714, 1714,  469, 1887,  399,  583,  768,
        861,  542,  997, 1223,  159,  233, 1181,  527,
        1991,  574,  447, 1213, 1542,  134, 1046, 1058,
        344, 1458, 1201,  322, 1141, 1545,  946,  107,
        164,  838,   69, 1337,  688, 1202, 1577,  206,
        783, 1144,  551,  755,  485,  760, 1494, 1146,
        226,  950, 1493, 1352,  827,   77, 1334, 1812,
        1590,  202,  727, 1535, 1956,  890,  345, 1344,
        1474,  941,  992, 1370,  228, 1450, 1998, 2047,
        154, 1003,  825, 1347,  957, 1150,  764,  249,
        730, 1756, 1867,  298, 1628, 1130, 1469, 1184,
        1237, 1568,  965, 1772,  372, 1230,  527,  250,
        590,  400, 1545
    };

    int compress1_all_pass = 1;
    int compress1_all_pass_hw = 1;
    int8_t compress1_out[NTESTS];
    int8_t compress1_out_hw[NTESTS];

    int compress2_all_pass = 1;
    int compress2_all_pass_hw = 1;
    int8_t compress2_out[NTESTS];
    int8_t compress2_out_hw[NTESTS];

    int compress3_all_pass = 1;
    int compress3_all_pass_hw = 1;
    int16_t compress3_out[NTESTS];
    int16_t compress3_out_hw[NTESTS];

    int compress4_all_pass = 1;
    int compress4_all_pass_hw = 1;
    int16_t compress4_out[NTESTS];
    int16_t compress4_out_hw[NTESTS];
    
    
     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_compress1, cycles_compress1_hw;
    #endif

    printf("Kyber-compress1 tests (SW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS; i++) {
        compress1_out[i] = compress1(d_inputs[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress1);
        printf("compress1: %d\n", cycles_compress1);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress1_out[i] != golden[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs[i], golden[i], compress1_out[i]);
            compress1_all_pass = 0;
        }
    }

    if (compress1_all_pass) { printf("  All %d compress1 tests passed!\n", NTESTS);}

    printf("Kyber-compress1 tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
       asm volatile (
                "mv a3, %[src]\n\t"
                ".insn r 0x3b, 0x1, 0x25, %[dst], a3, %[x]\r\n"
                : [dst] "=r" (compress1_out_hw[i]) 
                : [src] "r" (d_inputs[i]), [x] "r" (0) : );
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress1_hw);
        printf("cycles_cbd_hw: %d\n", cycles_compress1_hw);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress1_out_hw[i] != golden[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs[i], golden[i], compress1_out_hw[i]);
            compress1_all_pass_hw = 0;
        }
    }
    if (compress1_all_pass_hw) { printf("  All %d compress1 tests passed!\n", NTESTS);}

    //******************COMPRESS2 ***************************************/
     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_compress2, cycles_compress2_hw;
    #endif

    printf("Kyber-compress2 tests (SW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    
    for (int i = 0; i < NTESTS; i++) {
        compress2_out[i] = compress2(d_inputs2[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress2);
        printf("compress2: %d\n", cycles_compress2);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress2_out[i] != golden2[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs2[i], golden2[i], compress2_out[i]);
            compress2_all_pass = 0;
        }
    }

    if (compress2_all_pass) { printf("  All %d compress2 tests passed!\n", NTESTS);}

    
    printf("Kyber-compress2 tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
       asm volatile (
                "mv a3, %[src]\n\t"
                ".insn r 0x3b, 0x1, 0x26, %[dst], a3, %[x]\r\n"
                : [dst] "=r" (compress2_out_hw[i]) 
                : [src] "r" (d_inputs2[i]), [x] "r" (0) : );
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress2_hw);
        printf("cycles_cbd_hw: %d\n", cycles_compress2_hw);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress2_out_hw[i] != golden2[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs2[i], golden2[i], compress2_out_hw[i]);
            compress2_all_pass_hw = 0;
        }
    }
    if (compress2_all_pass_hw) { printf("  All %d compress2 tests passed!\n", NTESTS);}

    
   #ifdef PERF_CNT_CYCLES
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_compress3, cycles_compress3_hw;
    #endif

    printf("Kyber-compress3 tests (SW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    
    for (int i = 0; i < NTESTS; i++) {
        compress3_out[i] = compress3(d_inputs3[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress3);
        printf("compress2: %d\n", cycles_compress3);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress3_out[i] != golden3[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs3[i], golden3[i], compress3_out[i]);
            compress3_all_pass = 0;
        }
    }

    if (compress3_all_pass) { printf("  All %d compress3 tests passed!\n", NTESTS);}

    
    printf("Kyber-compress3 tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
       asm volatile (
                "mv a3, %[src]\n\t"
                ".insn r 0x3b, 0x1, 0x27, %[dst], a3, %[x]\r\n"
                : [dst] "=r" (compress3_out_hw[i]) 
                : [src] "r" (d_inputs3[i]), [x] "r" (0) : );
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress3_hw);
        printf("cycles_cbd_hw: %d\n", cycles_compress3_hw);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress3_out_hw[i] != golden3[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs3[i], golden3[i], compress3_out_hw[i]);
            compress3_all_pass_hw = 0;
        }
    }
    if (compress3_all_pass_hw) { printf("  All %d compress3 tests passed!\n", NTESTS);}

    
     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_compress4, cycles_compress4_hw;
    #endif

    printf("Kyber-compress4 tests (SW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    
    for (int i = 0; i < NTESTS; i++) {
        compress4_out[i] = compress4(d_inputs4[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress4);
        printf("compress2: %d\n", cycles_compress4);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress4_out[i] != golden4[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs4[i], golden4[i], compress4_out[i]);
            compress4_all_pass = 0;
        }
    }

    if (compress4_all_pass) { printf("  All %d compress4 tests passed!\n", NTESTS);}

    
    printf("Kyber-compress4 tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
       asm volatile (
                "mv a3, %[src]\n\t"
                ".insn r 0x3b, 0x1, 0x28, %[dst], a3, %[x]\r\n"
                : [dst] "=r" (compress4_out_hw[i]) 
                : [src] "r" (d_inputs4[i]), [x] "r" (0) : );
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_compress4_hw);
        printf("cycles_cbd_hw: %d\n", cycles_compress4_hw);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        if (compress4_out_hw[i] != golden4[i]) {
            printf("  [%2d] FAIL: d=%d, exp=%d, got=%d\n",
                    i, d_inputs4[i], golden4[i], compress4_out_hw[i]);
            compress4_all_pass_hw = 0;
        }
    }
    if (compress4_all_pass_hw) { printf("  All %d compress4 tests passed!\n", NTESTS);}
    
    
    return 0;
}
