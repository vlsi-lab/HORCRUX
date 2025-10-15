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



#define NTESTS 10
#define NLANES 8

/* 
 * For each j=0..7:
 *   a = (d >> (4*j + 0)) & 0x3;
 *   b = (d >> (4*j + 2)) & 0x3;
 *   out[j] = a - b;
 */
void nib_diff(int32_t d, int8_t out[NTESTS][NLANES], int i) {
    for (int j = 0; j < NLANES; j++) {
        int32_t a = (d >> (4 * j + 0)) & 0x3;
        //printf("a = %d\n", a);
        int32_t b = (d >> (4 * j + 2)) & 0x3;
        //printf("b = %d\n", b);
        out[i][j] = (int8_t)(a - b);
    }
}

int main(void) {
    /* your 12 test inputs */
    static const int32_t d_inputs[NTESTS] = {
        1343628325, -1437313772,   38830161,
        1417980328, -1869176699,  1344636249,  1147406601,
        446211349,   352866393,  1159043162
    };

    /* golden outputs you logged, in the same order */
    static const int8_t golden[NTESTS][NLANES] = {
        /*  1343628325 */ {  0,  2, -1,  2,  1,  1,  0,  0 },
        /* -1437313772 */ { -1,  1,  0,  0, -1,  0,  0,  0 },
        /*    38830161 */ {  1,  0,  0, -2,  0,  0,  2,  0 },
        /*  1417980328 */ { -2,  0, -1,  0, -1, -2, -1,  0 },
        /* -1869176699 */ {  0, -2,  0,  0,  1, -1,  0, -1 },
        /*  1344636249 */ { -1,  0,  0, -2,  0,  2,  0,  0 },
        /*  1147406601 */ { -1,  0, -1,  0, -1,  1, -1, -1 },
        /*   446211349 */ {  0,  1,  0,  0, -2, -1,  0,  1 },
        /*   352866393 */ { -1,  0,  0,  0, -2,  0,  0,  1 },
        /*  1159043162 */ {  0,  0, -2, -1,  0,  1,  0, -1 }
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
            ".insn r 0x3b, 0x1, 0x1D, %[dst], a3, %[x]\r\n"
            : [dst] "=r" (out_hw[i][0])
            : [src] "r" (d_inputs[i])
            , [x] "r" (0) : );
        asm volatile (".insn r 0x3b, 0x1, 0x1E, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][1]) : [src] "r" (d_inputs[i]), [x] "r" (1) : );
        asm volatile (".insn r 0x3b, 0x1, 0x1F, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][2]) : [src] "r" (d_inputs[i]), [x] "r" (2) : );
        asm volatile (".insn r 0x3b, 0x1, 0x20, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][3]) : [src] "r" (d_inputs[i]), [x] "r" (3) : );
        asm volatile (".insn r 0x3b, 0x1, 0x21, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][4]) : [src] "r" (d_inputs[i]), [x] "r" (4) : );
        asm volatile (".insn r 0x3b, 0x1, 0x22, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][5]) : [src] "r" (d_inputs[i]), [x] "r" (5) : );
        asm volatile (".insn r 0x3b, 0x1, 0x23, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][6]) : [src] "r" (d_inputs[i]), [x] "r" (6) : );
        asm volatile (".insn r 0x3b, 0x1, 0x24, %[dst], %[src], %[x]\r\n" : [dst] "=r" (out_hw[i][7]) : [src] "r" (d_inputs[i]), [x] "r" (7) : );
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
