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
          

int main(void) {

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



    int16_t compress3_out[NTESTS];

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < NTESTS; i++) {
        compress3_out[i] = compress3(d_inputs3[i]);
    }
    vcd_disable();
    



    return 0;
}
