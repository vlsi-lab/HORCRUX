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

uint8_t compress1_hw(int32_t d) {

    uint8_t res;
    asm volatile (
        "addi t0, %1, 0\n\t"               
        ".insn r 0x3b, 0x1, 0x25, %0, t0, x0\n\t"
        : "=&r"(res)  : "r"(d) : "t0", "cc"                        
    );
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

    int8_t compress1_out[NTESTS];


    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < NTESTS; i++) {
        compress1_out[i] = compress1_hw(d_inputs[i]);
    }
    vcd_disable();


    return 0;
}
