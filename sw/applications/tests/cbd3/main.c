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

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif



#define NTESTS 100
#define NLANES 4

/* 
 * For each j=0..7:
 *   a = (d >> (4*j + 0)) & 0x3;
 *   b = (d >> (4*j + 2)) & 0x3;
 *   out[j] = a - b;
 */
void nib_diff(int32_t d, int8_t out[NTESTS][NLANES], int i) {
    for (int j = 0; j < NLANES; j++) {
        int32_t a = (d >> (6 * j + 0)) & 0x7;
        //printf("a = %d\n", a);
        int32_t b = (d >> (6 * j + 3)) & 0x7;
        //printf("b = %d\n", b);
        out[i][j] = (int8_t)(a - b);
    }
}

int main(void) {
    /* your 12 test inputs */
    static const int32_t d_inputs[NTESTS] = {
        2658953,  304195,  2430538,  6858378,  2433738,  598730,  2695769,  6369937,  4301521,  2664073,
        4531394,  336577,  2692744,  2630217,  6919312,  2695818,  4494482,  4527745,  2658882, 2433561,
        4761282,  2364498,  2625617,  327833,  594570,  271041,  308250,  271955,  4760282,  2692187,
        4497930,  6628937,  295576,  4756106,  2434705,  2695881,  4756041,  4818064,  4793435,  4534929,
        2404930,  2396753,  4527258,  2397322,  4723849,  4759569,  4461642,  6592209,  4560082,  4563083,
        2691675,  2627216,  6656154,  6922945,  2400906,  2368138,  566938,  4497562,  2626634,  4563547, 
        4264986,  2659473,  2700370,  304793,  4792850,  4793362,  6919315,  4759641,  2466378,  4494482,
        332874,  2397266,  2429585,  2958026,  4502154,  2429586,  2434130,  6886985,  4237011,  2404953,
        4235858,  2430154,  340562,  4232338,  4240522,  2200656,  2953817,  2397186,  4236418,  2622978, 
        6595153,  2954378,  4502089,  4494985,  2134610,  4756618,  4461266,  565969,  2696267,  2400842
    };

    /* golden outputs you logged, in the same order */
    static const int8_t golden[NTESTS][NLANES] = {
        /* group 0: d = 2658953 */
        {  0,   1,    0,    1 },
        /* group 1: d = 304195  */
        {   3,   -1,    1,    1 },
        /* group 2: d = 2430538 */
        {   1,   -2,   -1,    0 },
        /* group 3: d = 6858378 */
        {   1,   -1,    1,   -1 },
        /* group 4: d = 2433738 */
        {   1,    2,    0,    0 },
        /* group 5: d = 598730  */
        {   1,    2,    0,    2 },
        /* group 6: d = 2695769 */
        {  -2,    0,    0,    1 },
        /* group 7: d = 6369937 */
        {  -1,    1,    1,   -3 },
        /* group 8: d = 4301521 */
        {  -1,    2,   -1,   -2 },
        /* group 9: d = 2664073 */
        {   0,   -1,    1,    1 },
        /* group 10: d = 4531394 */
        {   2,    1,    0,   -1 },
        /* group 11: d = 336577  */
        {   1,    2,    0,    1 },
        /* group 12: d = 2692744 */
        {  -1,   -1,   -1,    1 },
        /* group 13: d = 2630217 */
        {   0,    0,    2,    1 },
        /* group 14: d = 6919312 */
        {  -2,    0,   -2,   -1 },
        /* group 15: d = 2695818 */
        {   1,    1,    0,    1 },
        /* group 16: d = 4494482 */
        {   0,    0,    0,   -1 },
        /* group 17: d = 4527745 */
        {   1,   -1,   -1,   -1 },
        /* group 18: d = 2658882 */
        {   2,    0,    0,    1 },
        /* group 19: d = 2433561 */
        {  -2,   -1,    0,    0 },
        /* group 20: d = 4761282 */
        {   2,    0,    1,    0 },
        /* group 21: d = 2364498 */
        {   0,   -1,    1,    0 },
        /* group 22: d = 2625617 */
        {  -1,    1,    1,    1 },
        /* group 23: d = 327833  */
        {  -2,    2,   -2,    1 },
        /* group 24: d = 594570  */
        {   1,    1,   -1,    2 },
        /* group 25: d = 271041  */
        {   1,    2,    2,    1 },
        /* group 26: d = 308250  */
        {  -1,   -2,    2,    1 },
        /* group 27: d = 271955  */
        {   1,   -2,    2,    1 },
        /* group 28: d = 4760282 */
        {  -1,    2,    1,    0 },
        /* group 29: d = 2692187 */
        {   0,   -1,   -1,    1 },
        /* group 30: d = 4497930 */
        {   1,   -1,    1,   -1 },
        /* group 31: d = 6628937 */
        {   0,   -2,    0,   -2 },
        /* group 32: d = 295576  */
        {  -3,    1,   -1,    1 },
        /* group 33: d = 4756106 */
        {   1,    1,    0,    0 },
        /* group 34: d = 2434705 */
        {  -1,   -1,    0,    0 },
        /* group 35: d = 2695881 */
        {   0,    2,    0,    1 },
        /* group 36: d = 4756041 */
        {   0,    0,    0,    0 },
        /* group 37: d = 4818064 */
        {  -2,    0,   -3,    0 },
        /* group 38: d = 4793435 */
        {   0,   -1,    0,    0 },
        /* group 39: d = 4534929 */
        {  -1,    1,    1,   -1 },
        /* group 40: d = 2404930 */
        {   2,    0,    2,    0 },
        /* group 41: d = 2396753 */
        {  -1,    0,    0,    0 },
        /* group 42: d = 4527258 */
        {  -1,    0,   -1,   -1 },
        /* group 43: d = 2397322 */
        {   1,    0,    0,    0 },
        /* group 44: d = 4723849 */
        {   0,    0,    1,    0 },
        /* group 45: d = 4759569 */
        {  -1,    0,    1,    0 },
        /* group 46: d = 4461642 */
        {   1,   -1,    1,   -1 },
        /* group 47: d = 6592209 */
        {  -1,    0,    0,   -2 },
        /* group 48: d = 4560082 */
        {   0,    1,   -2,   -1 },
        /* group 49: d = 4563083 */
        {   2,    2,   -1,   -1 },
        /* group 50: d = 2691675 */
        {   0,    0,   -1,    1 },
        /* group 51: d = 2627216 */
        {  -2,   -1,    1,    1 },
        /* group 52: d = 6656154 */
        {  -1,    2,   -2,   -2 },
        /* group 53: d = 6922945 */
        {   1,    2,   -1,   -1 },
        /* group 54: d = 2400906 */
        {   1,    1,    1,    0 },
        /* group 55: d = 2368138 */
        {   1,    1,    2,    0 },
        /* group 56: d = 566938  */
        {  -1,   -1,    1,    2 },
        /* group 57: d = 4497562 */
        {  -1,    2,    1,   -1 },
        /* group 58: d = 2626634 */
        {   1,   -1,    1,    1 },
        /* group 59: d = 4563547 */
        {   0,    0,   -1,   -1 },
        /* group 60: d = 4264986 */
        {  -1,   -2,   -1,   -2 },
        /* group 61: d = 2659473 */
        {  -1,    0,    0,    1 },
        /* group 62: d = 2700370 */
        {   0,   -1,    1,    1 },
        /* group 63: d = 304793  */
        {  -2,   -1,    1,    1 },
        /* group 64: d = 4792850 */
        {   0,   -1,    0,    0 },
        /* group 65: d = 4793362 */
        {   0,   -2,    0,    0 },
        /* group 66: d = 6919315 */
        {   1,    0,   -2,   -1 },
        /* group 67: d = 4759641 */
        {  -2,    1,    1,    0 },
        /* group 68: d = 2466378 */
        {   1,    0,   -1,    0 },
        /* group 69: d = 4494482 */ 
        {   0,    0,    0,   -1 },
        /* group 70: d = 332874  */
        {   1,   -1,   -1,    1 },
        /* group 71: d = 2397266 */
        {   0,   -1,    0,    0 },
        /* group 72: d = 2429585 */
        {  -1,    1,   -1,    0 },
        /* group 73: d = 2958026 */
        {   1,    2,    0,    2 },
        /* group 74: d = 4502154 */
        {   1,    1,    2,   -1 },
        /* group 75: d = 2429586 */
        {   0,    1,   -1,    0 },
        /* group 76: d = 2434130 */
        {   0,   -1,    0,    0 },
        /* group 77: d = 6886985 */
        {   0,   -2,   -1,   -1 },
        /* group 78: d = 4237011 */
        {   1,    0,    1,   -2 },
        /* group 79: d = 2404953 */
        {  -2,    0,    2,    0 },
        /* group 80: d = 4235858 */
        {   0,    0,    1,   -2 },
        /* group 81: d = 2430154 */
        {   1,    1,   -1,    0 },
        /* group 82: d = 340562  */
        {   0,    0,    1,    1 },
        /* group 83: d = 4232338 */
        {   0,    0,    0,   -2 },
        /* group 84: d = 4240522 */
        {   1,    0,    2,   -2 },
        /* group 85: d = 2200656 */
        {  -2,   -1,   -2,   -1 },
        /* group 86: d = 2953817 */
        {  -2,    0,   -1,    2 },
        /* group 87: d = 2397186 */
        {   2,   -2,    0,    0 },
        /* group 88: d = 4236418 */
        {   2,    0,    1,   -2 },
        /* group 89: d = 2622978 */
        {   2,   -3,    0,    1 },
        /* group 90: d = 6595153 */
        {  -1,    0,    1,   -2 },
        /* group 91: d = 2954378 */
        {   1,    0,   -1,    2 },
        /* group 92: d = 4502089 */
        {   0,    0,    2,   -1 },
        /* group 93: d = 4494985 */
        {   0,   -1,    0,   -1 },
        /* group 94: d = 2134610 */
        {   0,    0,    0,   -1 },
        /* group 95: d = 4756618 */
        {   1,    0,    0,    0 },
        /* group 96: d = 4461266 */
        {   0,    2,    1,   -1 },
        /* group 97: d = 565969  */
        {  -1,    2,    1,    2 },
        /* group 98: d = 2696267 */
        {   2,   -1,    0,    1 },
        /* group 99: d = 2400842 */
        {   1,    0,    1,    0 }
    };

    int all_pass = 1;
    int all_pass_hw = 1;
    int8_t out[NTESTS][NLANES];
    int8_t out_hw[NTESTS][NLANES];

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_cbd, cycles_cbd_hw;
    #endif

    printf("Kyber cbd tests (SW)\n");

     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS; i++) {
        nib_diff(d_inputs[i], out, i);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_cbd);
        printf("cycles_cbd_sw: %d\n", cycles_cbd);
    #endif

    for (int i = 0; i < NTESTS; i++) {
        for (int j = 0; j < NLANES; j++) {
            if (out[i][j] != golden[i][j]) {
                printf("  [%2d][%d] FAIL: d=%d, exp=%d, got=%d\n",
                       i, j, d_inputs[i], golden[i][j], out[i][j]);
                all_pass = 0;
            }
        }
    }

    if (all_pass) {
        printf("  All %d nib‐difference tests passed!\n", NTESTS);
    }

    printf("Kyber cbd tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS; i++) {
       asm volatile (
                "mv a3, %[src]\n\t"
                ".insn r 0x3b, 0x1, 0x19, %[dst], a3, %[x]\r\n"
                : [dst] "=r" (out_hw[i][0]) 
                : [src] "r" (d_inputs[i]), [x] "r" (0) : );
            asm volatile (".insn r 0x3b, 0x1, 0x1A, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][1]) : [src] "r" (d_inputs[i]), [x] "r" (1) : );
            asm volatile (".insn r 0x3b, 0x1, 0x1B, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][2]) : [src] "r" (d_inputs[i]), [x] "r" (2) : );
            asm volatile (".insn r 0x3b, 0x1, 0x1C, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][3]) : [src] "r" (d_inputs[i]), [x] "r" (3) : );
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_cbd_hw);
        printf("cycles_cbd_hw: %d\n", cycles_cbd_hw);
    #endif

    for (int i = 0; i < NTESTS; i++) {
        for (int j = 0; j < NLANES; j++) {
            if (out_hw[i][j] != golden[i][j]) {
                printf("  [%2d][%d] FAIL: d=%d, exp=%d, got=%d\n",
                       i, j, d_inputs[i], golden[i][j], out_hw[i][j]);
                all_pass_hw = 0;
            }
        }
    }

    if (all_pass_hw) {
        printf("  All %d nib‐difference HW tests passed!\n", NTESTS);
    }

    return 0;
}
