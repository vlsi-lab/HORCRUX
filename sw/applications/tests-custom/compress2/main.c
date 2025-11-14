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


uint8_t compress2_hw(int32_t d) {

    uint8_t res;
    asm volatile (
        "addi t0, %1, 0\n\t"               
        ".insn r 0x3b, 0x1, 0x26, %0, t0, x0\n\t"
        : "=&r"(res)  : "r"(d) : "t0", "cc"                        
    );
    return res;
}


int main(void) {
  

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

  
    int8_t compress2_out[NTESTS];


    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < NTESTS; i++) {
       compress2_out[i] = compress2_hw(d_inputs2[i]);
    }
    vcd_disable();


    return 0;
}
