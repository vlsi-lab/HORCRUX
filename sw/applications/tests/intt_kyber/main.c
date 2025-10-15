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

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif

#include <stdint.h>
#define KYBER_N 256
#define KYBER_Q 3329

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
        -98,  -206,  -341, -1967, -1890, -2095, -1940,  -703,
       -2115, -1750,    21, -1092, -2231,  -223, -2972, -3111,
       -2432, -3266, -2936, -3056, -3133,  -570, -1613,  -763,
       -1298, -2913,  -357, -1366,  -732, -2653,  -116, -1867,
        -497, -2820,  -758, -2735, -1780, -1940, -2239, -1668,
       -1891,  -265,   -87,  -486,   -61, -3245, -1676,  -741,
        -698, -2161, -2050,  -172,  -586, -3235,  -135, -1486,
       -2660, -3216, -1983, -1004, -1658,  -815,  -229, -2251,
       -2802,  -945, -2317, -2300, -1205, -2584,  -563,  -756,
       -1396, -1659, -1445,  -113, -1229, -2654, -2504,  -498,
        -742,  -506, -2151, -1736,  -656,  -543, -2498, -1944,
       -3221, -2605,  -777,  -944,  -341, -3118, -2078, -1386,
        -113, -1178,  -821, -1103, -2307, -1150, -3239, -1635,
        -291,  -215, -3227, -1631,  -556,  -866, -2925, -2472,
       -1927, -3092, -2160, -1300,  -473, -2540, -1166, -1871,
       -3017, -2752, -1050, -2203, -3231,  -223,  -573, -1192,
       -2623, -2816, -1445, -3168,  -680, -1724, -2429,  -820,
        -790, -1712,  -559, -3018,  -141, -1606, -1382, -2912,
        -282, -1608, -2904, -1319, -2527,  -503, -2102,  -897,
       -2574, -2076, -1581, -2275, -1650, -2460, -2338,  -908,
       -3338, -2488,  -200, -2239, -1971,  -535, -2078, -1369,
       -2551,  -333, -1751, -3171, -2599, -2906, -1766, -2534,
       -1461, -1354,  -714,  -545, -1905, -1057,  -826,   -93,
       -2172,  -907, -1343,  -460, -1437, -2870, -3332,  -506,
       -1564,  -945, -3349, -1176, -3264, -3332, -2281, -2359,
       -1626,  -171, -1056, -1077, -2973, -2616, -2122, -2969,
       -2936, -2836,  -202, -2159, -1480, -3379,  -243,  -224,
       -3195,  -270,  -722,  -313, -3040, -1614,  -824, -1047,
       -2207, -3187, -2181, -2392, -1214, -2009, -2123, -2655,
        -815,  -691, -3096, -1237, -2619, -1954,  -271, -2231,
       -2087, -1658, -1626, -2660,  -672, -2732,  -812, -1703,
       -1926, -3294,  -573, -2832,  -867, -2931,  -960, -1271
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

void barrett_reduce_asm(int32_t a, int32_t *t) {
    asm volatile (
        "mv a3, %[rs1]\n\t"
        ".insn r 0x3b, 0x01, 0x10, %[rd], a3, x0\n\t"
        : [rd] "=r" (*t)
        : [rs1] "r" (a)
        : "a3", "cc" );
}

static int16_t fqmul(int16_t a, int16_t b) {
    return montgomery_reduce((int32_t)a * b);
}

int16_t kyber_barrett_reduce(int16_t x) {

    int32_t t1 = x;        //   1·x
    int32_t t2 = x << 1;   //   2·x
    int32_t t3 = x << 2;   //   4·x
    int32_t t4 = x << 3;   //   8·x
    int32_t t5 = x << 5;   //  32·x
    int32_t t6 = x << 7;   // 128·x
    int32_t t7 = x << 8;   // 256·x
    int32_t t8 = x << 9;   // 512·x
    int32_t t9 = x << 12;  // 4096·x

    int32_t t = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9;

    // 2. First reduction: >> 24
    t >>= 24;
    //printf("t after >>24 = 0x%X (%d)\n", (uint32_t)t, (int)t);

    // 3. Reconstruction: t*(2^11 + 2^10 + 2^8 + 1)
    t = (t << 11) + (t << 10) + (t << 8) + t;
    //printf("t after reconstruction = 0x%X (%d)\n", (uint32_t)t, (int)t);

    // 4. Compute z before conditional subtract
    int16_t z = x - (int16_t)t;
    //printf("z before reduction = 0x%04X (%d)\n", (uint16_t)z, (int)z);

    // 5. Conditional subtract of KYBER_Q
    if (z >= KYBER_Q) {
        z -= KYBER_Q;
    } 

    return z;
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
                //t = r[j];
                ////r[j] = kyber_barrett_reduce(t + r[j + len]);
                //asm volatile (
                //    "mv a3, %[rs1]\n\t"
                //    ".insn r 0x3b, 0x01, 0x10, %[rd], a3, x0\n\t"
                //    : [rd] "=r" (r[j])
                //    : [rs1] "r" (t + r[j + len])
                //    : "a3", "cc" );
                ////barrett_reduce_asm(t + r[j + len], &r[j]);
                //r[j + len] = r[j + len] - t;
                //t32 = (int32_t)zeta * r[j + len];
                //asm volatile (
                //  "mv a3, %[rs1]\n\t"
                //  ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
                //  : [rd] "=r" (r[j + len])
                //  : [rs1] "r" (t32)
                //  : "a3", "cc" );
                ////r[j + len] = fqmul(zeta, r[j + len]);
                int32_t rj   = r[j];
                int32_t rlen = r[j + len];
                int32_t sum, diff, prod;

                asm volatile(
                    "add   %[sum],  %[rj],   %[rlen]\n\t"
                    ".insn r 0x3b, 0x01, 0x10, %[sum], %[sum], x0\n\t"
                    "sub   %[diff], %[rlen], %[rj]\n\t"
                    "mul   %[prod], %[z],    %[diff]\n\t"
                    ".insn r 0x3b, 0x01, 0x5, %[prod], %[prod], x0\n\t"
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
        ".insn r 0x3b, 0x1, 0x5, %[out], %[prod], x0\n\t"
        : [out] "=&r"(out), [prod] "=&r"(prod_lo)
        : [x] "r"((int32_t)r[j]), [f] "r"((int32_t)f)
        : "cc", "a3"
    );
    r[j] = out;
}
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
                r[j] = kyber_barrett_reduce(t + r[j + len]);
                r[j + len] = r[j + len] - t;
                r[j + len] = fqmul(zeta, r[j + len]);
            }
        }
    }

    for (j = 0; j < 256; j++) {
        r[j] = fqmul(r[j], f);
    }
}

int main() {

    int all_tests_passed = 1;
    int all_tests_passed_hw = 1;

    int32_t result_kyber[] = {};
    int32_t result_kyber_hw[] = {};
    poly a = tv_in;
    poly a_hw = tv_in;
    
    size_t i;
    int16_t coeffs;
    poly out_hw = { .coeffs = {0} };

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_kyber_intt_hw, cycles_kyber_intt;
    #endif 

    //printf("kyber NTT Reduction Tests.\n");
    //printf("coeffs before NTT:\n");
    //for (int i = 0; i < N; i++) {
    //    printf("%d", a.coeffs[i]);
    //    if (i == N-1)       printf("\n");
    //    else if ((i+1) % 8) printf(" ");
    //    else                printf("  ");  // two spaces at 8-elem boundary
    //}
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    invntt(a.coeffs);
    
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_kyber_intt);
        printf("cycles_kyber_intt_red (SW): %d\n", cycles_kyber_intt);
    #endif


    //assert(memcmp(a.coeffs, tv1_out.coeffs, sizeof a.coeffs) == 0);

    for (int i = 0; i < 256; i++) {
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

     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    kyber_intt_hw(a_hw.coeffs);


     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_kyber_intt_hw);
        printf("cycles_kyber_intt_red (HW): %d\n", cycles_kyber_intt_hw);
    #endif

    for (int i = 0; i < 256; i++) {
        if (a_hw.coeffs[i] != tv_out_after_red.coeffs[i]) {
            printf(" MISMATCH @ idx %3d: got %6d, expected %6d\n",
                   i, a_hw.coeffs[i], tv_out_after_red.coeffs[i]);
            all_tests_passed_hw = 0;
        }
    }

    if (all_tests_passed_hw) {
        printf("All 256 outputs match the golden tv_out_after_red vector.\n");
    } else {
        printf("Test FAILED: see mismatch list above.\n");
    }

    return 0;
}