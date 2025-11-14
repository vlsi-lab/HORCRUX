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


    int16_t compress4_out[NTESTS];
    
    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < NTESTS; i++) {
        compress4_out[i] = compress4(d_inputs4[i]);
    }
    vcd_disable();

    return 0;
}
