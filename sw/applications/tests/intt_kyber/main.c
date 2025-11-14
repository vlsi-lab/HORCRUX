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
#include <assert.h>
#include <string.h>
#include "vcd_util.h"

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif


#define KYBER_N 256
#define KYBER_Q 3329
#define KYBER_QINV  62209    // matches Kyber reference

typedef struct {
    int16_t coeffs[KYBER_N];
} poly;

static const poly tv_in = {
    .coeffs = {
        1991, 2580,  101,  735, 1804,  592,  386,  867,
        2429, 2355, 2200, 1765, 1537, 1716, 2256, 1145,
        3085,  232, 1789, 1389, 1999, 2629, 2410, 1619,
        2065,  319, 3048, 1529, 3185, 1540, 2602,  474,
        1390,  976, 1169, 1961, 1996, 1304,  424, 2533,
        1662,  282, 1315, 1734, 1611, 2554, 3328, 2743,
        1142, 2225, 2017,  659,  469,  928,  741, 2036,
        1675,  620, 3044, 2653, 2203, 2788, 1051, 3055,
        1584,  226,  886,  756, 1754,  758, 2843, 2821,
        2775, 2722, 1721,  839, 1380, 2147,  489, 2278,
        1651,  695, 3306, 1605, 1951,  593, 2695, 2120,
         493, 2978, 2579, 2859, 1935, 3256,  546, 1725,
        2502, 2192,  394, 1895, 3084, 2992, 2553, 2625,
        1829, 2216, 2846,  360, 2325,  719, 2731, 1479,
         288, 2326, 3237, 3251,  558,  476, 2309, 2842,
         389, 2237, 2876,  573,  110,  288, 3068, 1218,
        2845,  280, 1319, 1667,  582, 1237, 1013, 2936,
        2788,  559,  481, 1857, 2773,  969, 1364, 1630,
         726, 2799,  120, 1929, 2888, 1559, 2814,  723,
         278,  511, 1986, 1740, 1501, 2156,  185,  582,
        1214, 1227, 1573, 1779,   23,  181,  587, 2558,
        1142, 2981, 2663,  302, 3150, 3262, 1541,  142,
        1869, 3271,  289, 1610, 2545,  891, 2245, 2767,
        1973, 3170, 3038, 2656, 1826, 2616, 1834,  248,
        1415, 1213, 2846, 2372, 3326, 2964, 2354, 1236,
        2137, 3058,  757, 1672,  764, 2471,  856, 2403,
        2655, 3327,  101, 1508, 2429,  296, 2904, 1603,
         136, 3015, 1343, 1005, 2346,   86, 2555, 1442,
         746,  135,  736,  572,  528,  363,  540,  732,
         865,  559, 2190, 1104, 3155, 2116, 1877,  810,
        1813, 1879, 2413, 2902, 2521, 1383,  655, 3153,
        1578, 3257, 3013,  476, 2190,  909,  950, 1646
    }
};

static const poly tv_out_after_red = {
    .coeffs = {
        -98,   -206,   -341,   1362,   1439,   1234,   1389,   -703,   1214,   1579,     21,  -1092,   1098,   -223,    357,    218,    897,     63,    393,    273,    196,   -570,  -1613,   -763,  -1298,    416,   -357,  -1366,   -732,    676,   -116,   1462,   -497,    509,   -758,    594,   1549,   1389,   1090,   1661,   1438,   -265,    -87,   -486,    -61,     84,   1653,   -741,   -698,   1168,   1279,   -172,   -586,     94,   -135,  -1486,    669,    113,   1346,  -1004,   1671,   -815,   -229,   1078,    527,   -945,   1012,   1029,  -1205,    745,   -563,   -756,  -1396,  -1659,  -1445,   -113,  -1229,    675,    825,   -498,   -742,   -506,   1178,   1593,   -656,   -543,    831,   1385,    108,    724,   -777,   -944,   -341,    211,   1251,  -1386,   -113,  -1178,   -821,  -1103,   1022,  -1150,     90,  -1635,   -291,   -215,    102,  -1631,   -556,   -866,    404,    857,   1402,    237,   1169,  -1300,   -473,    789,  -1166,   1458,    312,    577,  -1050,   1126,     98,   -223,   -573,  -1192,    706,    513,  -1445,    161,   -680,   1605,    900,   -820,   -790,   1617,   -559,    311,   -141,  -1606,  -1382,    417,   -282,  -1608,    425,  -1319,    802,   -503,   1227,   -897,    755,   1253,  -1581,   1054,   1679,    869,    991,   -908,     -9,    841,   -200,   1090,   1358,   -535,   1251,  -1369,    778,   -333,   1578,    158,    730,    423,   1563,    795,  -1461,  -1354,   -714,   -545,   1424,  -1057,   -826,    -93,   1157,   -907,  -1343,   -460,  -1437,    459,     -3,   -506,  -1564,   -945,    -20,  -1176,     65,     -3,   1048,    970,  -1626,   -171,  -1056,  -1077,    356,    713,   1207,    360,    393,    493,   -202,   1170,  -1480,    -50,   -243,   -224,    134,   -270,   -722,   -313,    289,  -1614,   -824,  -1047,   1122,    142,   1148,    937,  -1214,   1320,   1206,    674,   -815,   -691,    233,  -1237,    710,   1375,   -271,   1098,   1242,   1671,  -1626,    669,   -672,    597,   -812,   1626,   1403,     35,   -573,    497,   -867,    398,   -960,  -1271
    }
};

const int16_t kyber_zetas[128] = {
    -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
    -171,   622,  1577,   182,   962, -1202, -1474,  1468,
    573, -1325,   264,   383,  -829,  1458, -1602,  -130,
    -681,  1017,   732,   608, -1542,   411,  -205, -1571,
    1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
    516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
    -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
    -398,   961, -1508,  -725,   448, -1065,   677, -1275,
    -1103,   430,   555,   843, -1251,   871,  1550,   105,
    422,   587,   177,  -235,  -291,  -460,  1574,  1653,
    -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
    -1590,   644,  -872,   349,   418,   329,  -156,   -75,
    817,  1097,   603,   610,  1322, -1285, -1465,   384,
    -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
    -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
    -108,  -308,   996,   991,   958, -1460,  1522,  1628
};


static inline int16_t montgomery_reduce(int32_t a) {
  int16_t t;
  t = (int16_t)a * KYBER_QINV;
  t = (a - (int32_t)t * KYBER_Q) >> 16;
  return t;
}


int32_t KYBER_barrett_reduce(int32_t a) {
    int32_t t;
    const int32_t v = ((1 << 26) + KYBER_Q/2) / KYBER_Q;
    // Multiply by v, add rounding, shift down
    t = ((int32_t)v * a + (1 << 25)) >> 26;
    t *= KYBER_Q;
    return a - t;
}


static int16_t fqmul(int16_t a, int16_t b) {
    return montgomery_reduce((int32_t)a * b);
}


void invntt(int16_t r[256]) {
    unsigned int start, len, j, k;
    int16_t t, zeta;
    const int16_t f = 1441; // mont^2/128

    k = 127;
    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < 256; start = j + len) {
            zeta = kyber_zetas[k--];
            for (j = start; j < start + len; j++) {
                t = r[j];
                r[j] = KYBER_barrett_reduce(t + r[j + len]);
                r[j + len] = r[j + len] - t;
                r[j + len] = fqmul(zeta, r[j + len]);
            }
        }
    }

    for (j = 0; j < 256; j++) {
        r[j] = fqmul(r[j], f);
    }
}


void kyber_intt_hw(int16_t r[256]) {
    unsigned int start, len, j, k;
    int16_t t, zeta;
    const int16_t f = 1441; // mont^2/128
    int32_t t32;

    k = 127;
    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < 256; start = j + len) {
            zeta = kyber_zetas[k--];
            for (j = start; j < start + len; j++) {
                int32_t rj   = r[j];
                int32_t rlen = r[j + len];
                int32_t sum, diff, prod;

                asm volatile(
                    "add   %[sum],  %[rj],   %[rlen]\n\t"
                    ".insn r 0x3b, 0x7, 0x5, %[sum], %[sum], x0\n\t"
                    "sub   %[diff], %[rlen], %[rj]\n\t"
                    "mul   %[prod], %[z],    %[diff]\n\t"
                    ".insn r 0x3b, 0x7, 0, %[prod], %[prod], x0\n\t"
                    : [sum]  "=&r"(sum),
                    [diff] "=&r"(diff),
                    [prod] "=&r"(prod)
                    : [rj]   "r"(rj),
                    [rlen] "r"(rlen),
                    [z]    "r"((int32_t)zeta)
                    : "cc"
                );

                r[j]       = (int16_t)sum;   
                r[j + len] = (int16_t)prod;  
            }
        }
    }

for (j = 0; j < 256; j++) {
    int32_t out, prod_lo;
    asm volatile(
        "mul    %[prod], %[x], %[f]\n\t"
        ".insn r 0x3b, 0x7, 0, %[out], %[prod], x0\n\t"
        : [out] "=&r"(out), [prod] "=&r"(prod_lo)
        : [x] "r"((int32_t)r[j]), [f] "r"((int32_t)f)
        : "cc", "a3"
    );
    r[j] = out;
}
}

int main() {

    int all_tests_passed = 1;
    int all_tests_passed_hw = 1;

    poly a = tv_in;
    poly a_hw = tv_in;
    
    size_t i;
    int16_t coeffs;

    #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_kyber_intt_hw, cycles_kyber_intt;
    #endif 


    printf("INTT-Kyber (SW)\n");

    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    invntt(a.coeffs);
        
    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_kyber_intt);
        printf("cycles_kyber_intt (SW): %d\n", cycles_kyber_intt);
    #endif
    
    //assert(memcmp(a.coeffs, tv1_out.coeffs, sizeof a.coeffs) == 0);
    for (int i = 0; i < 256; i++) {
        //printf("%6d, ", a.coeffs[i]);
        if (a.coeffs[i] != tv_out_after_red.coeffs[i]) {
            printf(" MISMATCH @ idx %3d: got %6d, expected %6d\n",
                   i, a.coeffs[i], tv_out_after_red.coeffs[i]);
            all_tests_passed = 0;
        }
    }

    if (all_tests_passed) {
        printf("All 256 outputs match the golden tv1_out vector.\n");
    } else {
        printf("Test FAILED: see mismatch list above.\n");
    }

    printf("INTT-Kyber (HW)\n");

    #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    kyber_intt_hw(a_hw.coeffs);

    #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_kyber_intt_hw);
        printf("cycles_kyber_intt (HW): %d\n", cycles_kyber_intt_hw);
    #endif

    for (int i = 0; i < 256; i++) {
        //printf("%6d, ", a.coeffs[i]);
        if (a_hw.coeffs[i] != tv_out_after_red.coeffs[i]) {
            printf(" MISMATCH @ idx %3d: got %6d, expected %6d\n",
                   i, a_hw.coeffs[i], tv_out_after_red.coeffs[i]);
            all_tests_passed = 0;
        }
    }

    if (all_tests_passed) {
        printf("All 256 outputs match the golden tv1_out vector.\n");
    } else {
        printf("Test FAILED: see mismatch list above.\n");
    }

    return 0;
}