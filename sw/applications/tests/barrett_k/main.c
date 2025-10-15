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


// Placeholder values; replace these with actual values from Kyber and Dilithium specifications
#define QINV_KYBER       62209
#define KYBER_Q          3329
#define QINV_DILITHIUM   58728449
#define DILITHIUM_Q      8380417


#define NTESTS_BARRETT   150
#define HORCRUX_CUSTOM   1



// Kyber Barrett reduction (software)
int32_t KYBER_barrett_reduce(int32_t a) {
    int32_t t;
    const int32_t v = ((1 << 26) + KYBER_Q/2) / KYBER_Q;
    // Multiply by v, add rounding, shift down
    t = ((int32_t)v * a + (1 << 25)) >> 26;
    t *= KYBER_Q;
    return a - t;
}

int16_t KYBER_barrett_reduce_v2(int16_t x) {

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

    // 3. Reconstruction: t*(2^11 + 2^10 + 2^8 + 1)
    t = (t << 11) + (t << 10) + (t << 8) + t;

    // 4. Compute z before conditional subtract
    int16_t z = (int16_t)(x - t);


    // 5. Conditional subtract of KYBER_Q
    if (z >= KYBER_Q) {
        z -= KYBER_Q;
    } 

    

    return z;
}

// Prototype for hardware-accelerated Kyber Barrett (via inline ASM)
void KYBER_barrett_reduce_asm(int32_t a, int32_t *t) {
    asm volatile (
        "mv a3, %[rs1]\n\t"
        ".insn r 0x3b, 0x01, 0x10, %[rd], a3, x0\n\t"
        //".insn r 0x3b, 0x01, 0x12, a5, a4, x0\n\t"
        //".insn r 0x3b, 0x01, 0x11, %[rd], a3, a5\n\t"
        : [rd] "=r" (*t)
        : [rs1] "r" (a)
        : "a4", "a5", "a3", "cc" );
}

int main() {
    int i, j;

    int16_t barrett_inputs[300] = {
        -10904, -9291, -4864, -3945, -2106, -3003, -1134, 2015, -5203, -9168, 
        -1555, -2820, -2298, -1698, 1568, 822, -4740, -5621, -3516, -3461, 
        -6631, -2311, -1745, 25, -1790, -2832, -388, 1342, 2983, 3602, 
        7267, 4920, -10502, -7937, -7532, -6685, -3220, -3963, -1698, -3315, 
        -5910, -4132, -1666, -750, 877, 435, 2699, 5859, -1252, -3575, 
        1168, 727, -2321, -778, 3569, 3302, -3406, -3492, -86, -1824, 
        -172, 2426, 5052, 2678, -11788, -6393, -8102, -4733, -9432, -4272, 
        -5834, 1070, -8942, -4046, -5568, -3200, -2086, -2146, 784, -2104, 
        -5694, -8081, -1714, -4769, -3331, -5889, 1903, -5289, -4771, -3773, 
        -1363, 2669, -839, -2038, 5033, 4290, -9590, -4399, -5912, -1451, 
        -1766, -3909, -1596, -833, -1275, -4358, 51, 608, -1120, 3434, 
        2976, 5748, -4187, -4940, -683, -2788, 2946, -442, 3304, 3390, 
        3858, -444, 5660, 680, 2462, 2777, 7160, 5743, -5060, -8586, 
        -4474, -6320, -1714, -4862, 4404, 1688, -481, -3763, 4793, -2241, 
        5608, -627, 8988, 4927, -4915, -7744, 155, -5768, 4009, -3878, 
        4195, 702, 366, -3436, 4638, -26, 2560, 2869, 3344, 5785, 
        -2656, -4131, -2438, -3243, -77, 109, 5091, 2053, 187, -881, 
        303, 2039, 486, 3677, 2216, 4385, -208, 1948, 3550, 3560, 
        422, 4010, 6088, 10210, -366, 2919, 5032, 8295, 6692, 11126, 
        8718, 12468, -6312, -1703, -452, -963, -3973, -828, 549, 474, 
        -6255, -1014, -141, 3904, 413, 1336, 1875, 5754, -2106, -580, 
        1122, 2518, -651, 1370, 1591, 6036, -2744, -2625, 1314, 4029, 
        3246, 4879, 3932, 9957, -355, -4301, 3461, 857, -681, 2524, 
        5995, 6308, 656, -2333, 3566, 3787, 2452, 5554, 5914, 8884, 
        -1624, -2497, 3480, -167, 4098, 115, 5598, 1597, 7764, 1814, 
        9126, 5726, 9509, 4383, 9089, 10405, -13401, -12388, -7083, -8242, 
        -8035, -9187, -4381, -2883, -6884, -4491, -1638, -4305, -3649, -4879, 
        2463, 791, -5536, -9329, -2588, -7183, -4290, -2779, -2258, -1969, 
        -6590, -6709, -2738, -5415, -1213, -3342, 4653, 2406, -10255, -6518, 
        -7073, -6382, -6269, -9043, -6039, -3201, -4208, -4931, -2082, -3877
    };

    int16_t barrett_exp[300] = {
        2412, 696, 1794, 2713, 1223, 326, 2195, 2015, 1455, 819, 
        1774, 509, 1031, 1631, 1568, 822, 1918, 1037, 3142, 3197, 
        27, 1018, 1584, 25, 1539, 497, 2941, 1342, 2983, 273, 
        609, 1591, 2814, 2050, 2455, 3302, 109, 2695, 1631, 14, 
        748, 2526, 1663, 2579, 877, 435, 2699, 2530, 2077, 3083, 
        1168, 727, 1008, 2551, 240, 3302, 3252, 3166, 3243, 1505, 
        3157, 2426, 1723, 2678, 1528, 265, 1885, 1925, 555, 2386, 
        824, 1070, 1045, 2612, 1090, 129, 1243, 1183, 784, 1225, 
        964, 1906, 1615, 1889, 3327, 769, 1903, 1369, 1887, 2885, 
        1966, 2669, 2490, 1291, 1704, 961, 397, 2259, 746, 1878, 
        1563, 2749, 1733, 2496, 2054, 2300, 51, 608, 2209, 105, 
        2976, 2419, 2471, 1718, 2646, 541, 2946, 2887, 3304, 61, 
        529, 2885, 2331, 680, 2462, 2777, 502, 2414, 1598, 1401, 
        2184, 338, 1615, 1796, 1075, 1688, 2848, 2895, 1464, 1088, 
        2279, 2702, 2330, 1598, 1743, 2243, 155, 890, 680, 2780, 
        866, 702, 366, 3222, 1309, 3303, 2560, 2869, 15, 2456, 
        673, 2527, 891, 86, 3252, 109, 1762, 2053, 187, 2448, 
        303, 2039, 486, 348, 2216, 1056, 3121, 1948, 221, 231, 
        422, 681, 2759, 223, 2963, 2919, 1703, 1637, 34, 1139, 
        2060, 2481, 346, 1626, 2877, 2366, 2685, 2501, 549, 474, 
        403, 2315, 3188, 575, 413, 1336, 1875, 2425, 1223, 2749, 
        1122, 2518, 2678, 1370, 1591, 2707, 585, 704, 1314, 700, 
        3246, 1550, 603, 3299, 2974, 2357, 132, 857, 2648, 2524, 
        2666, 2979, 656, 996, 237, 458, 2452, 2225, 2585, 2226, 
        1705, 832, 151, 3162, 769, 115, 2269, 1597, 1106, 1814, 
        2468, 2397, 2851, 1054, 2431, 418, 3244, 928, 2904, 1745, 
        1952, 800, 2277, 446, 3103, 2167, 1691, 2353, 3009, 1779, 
        2463, 791, 1122, 658, 741, 2804, 2368, 550, 1071, 1360, 
        68, 3278, 591, 1243, 2116, 3316, 1324, 2406, 3061, 140, 
        2914, 276, 389, 944, 619, 128, 2450, 1727, 1247, 2781
    };



    int16_t barrett_res[NTESTS_BARRETT];
    int16_t barrett_res_hw[NTESTS_BARRETT];
    int all_barrett_pass = 1;
    int all_barrett_pass_hw = 1;

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_barrett, cycles_barrett_hw;
    #endif


    printf("Kyber Barrett Reduction Tests (SW):\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif


    for (i = 0; i < NTESTS_BARRETT; i++) {
        barrett_res[i] = KYBER_barrett_reduce_v2(barrett_inputs[i]);
    }

    
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_barrett);
        printf("cycles_barrett_sw: %d\n", cycles_barrett);
    #endif

    for (i = 0; i < NTESTS_BARRETT; i++) {
        if (barrett_res[i] != barrett_exp[i]) {
            printf("  [%2d] FAIL: in=%6d, exp=%6d, got=%6d\n",
                   i, barrett_inputs[i], barrett_exp[i], barrett_res[i]);
            all_barrett_pass = 0;
        }
    }
    if (all_barrett_pass) {
        printf("  All %d SW Barrett tests passed!\n\n", NTESTS_BARRETT);
    } else {
        printf("\n");
    }

    printf("Kyber Barrett Reduction Tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS_BARRETT; i++) {
        KYBER_barrett_reduce_asm(barrett_inputs[i], &barrett_res_hw[i]);
        //asm volatile (
        //    "mv a3, %[rs1]\n\t"
        //    ".insn r 0x3b, 0x1, 0x7, %[rd], a3, x0\n\t"
        //    : [rd] "=r" (barrett_res_hw[i])
        //    : [rs1] "r" (barrett_inputs[i])
        //    : "a3","cc" );
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_barrett_hw);
        printf("cycles_barrett_hw: %d\n", cycles_barrett_hw);
    #endif

    for (int i = 0; i < NTESTS_BARRETT; i++) {
        if (barrett_res_hw[i] != barrett_exp[i]) {
            printf("Test %d failed: expected %d, got %d\n", i, barrett_exp[i], barrett_res_hw[i]);
            all_barrett_pass_hw = 0;
        }
    }

    if (all_barrett_pass_hw) {
        printf("HW-barrett tests passed!\n");
    }
    
    return 0;
}