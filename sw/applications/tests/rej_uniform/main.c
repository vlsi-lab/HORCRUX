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


#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif


#define Q 8380417
#define NTESTS   25


int32_t rej_uniform(uint8_t a, uint8_t b, uint8_t c) {

    uint32_t t;
    t = (uint32_t)a | ((uint32_t)b << 8) | ((uint32_t)c << 16);
    t &= 0x7FFFFF;

    //if (t < Q) {
    //    return t;
    //}
    return t;
}


static void rej_uniform_asm(uint32_t a, uint32_t b, uint32_t c, uint32_t *result1) {
     
    asm volatile (
        "mv a3, %[rs3]\n\t"
         ".insn r 0x6b, 0x02, 0x1, %[rd1],  %[rs1], %[rs2], a3\n\t"
        : [rd1] "=r" (*result1)
        : [rs1] "r" (a), [rs2] "r" (b), [rs3] "r" (c)
        : "a3", "cc");
}

int main() {
    int i, j;

    uint8_t inputs[][3] = {
        {244, 149, 16},
        {247, 156, 193},
        {10, 157, 73},
        {182, 234, 66},
        {56, 97, 56},
        {78, 191, 12},
        {209, 180, 230},
        {35, 60, 89},
        {97, 55, 144},
        {212, 248, 203},
        {118, 120, 223},
        {42, 104, 199},
        {183, 62, 250},
        {77, 221, 2},
        {166, 226, 62},
        {63, 209, 207},
        {243, 223, 146},
        {193, 110, 255},
        {245, 145, 209},
        {190, 89, 34},
        {99, 239, 108},
        {121, 237, 162},
        {46, 108, 8},
        {215, 241, 28},
        {153, 185, 220}
    };

    int32_t after_vals[] = {
        1086964,
        4300023,
        4824330,
        4385462,
        3694904,
        835406,
        6730961,
        5848099,
        1062753,
        4978900,
        6256758,
        4679722,
        8011447,
        187725,
        4121254,
        5230911,
        1236979,
        8351425,
        5345781,
        2251198,
        7139171,
        2289017,
        551982,
        1896919,
        6076825
    };

    int32_t rej_uniform_res[NTESTS];
    int32_t rej_uniform_res_hw[NTESTS];
    int all_rej_uniform_pass = 1;
    int all_rej_uniform_pass_hw = 1;

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_rej_uniform, cycles_rej_uniform_hw;
    #endif

    
    printf("rej_uniform in Dilithium Reduction Tests (SW):\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (i = 0; i < NTESTS; i++) {
        uint8_t a = inputs[i][0];
        uint8_t b = inputs[i][1];
        uint8_t c = inputs[i][2];
        //printf("  [%2d] in=(%3d, %3d, %3d)\n ", i, a, b, c);
        rej_uniform_res[i] = rej_uniform(a, b, c);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_rej_uniform);
        printf("cycles_rej_uniform_sw: %d\n", cycles_rej_uniform);
    #endif

    //for (i = 0; i < NTESTS; i++) {
    //    if (rej_uniform_res[i] != after_vals[i]) {
    //        printf("  [%2d] FAIL: in=(%3d, %3d, %3d), exp=%8d, got=%8d\n",
    //            i, inputs[i][0], inputs[i][1], inputs[i][2], after_vals[i], rej_uniform_res[i]);
    //        all_rej_uniform_pass = 0;
    //    }
    //}
    //if (all_rej_uniform_pass) {
    //    printf("  All %d SW rej_uniform tests passed!\n\n", NTESTS);
    //} else {
    //    printf("\n");
    //}

    printf("rej_uniform in Dilithium Reduction Tests (HW).\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    for (int i = 0; i < NTESTS; i++) {
        //rej_uniform_asm((uint32_t)(inputs[i][0]), (uint32_t)(inputs[i][1]), (uint32_t)(inputs[i][2]), &rej_uniform_res_hw[i]);
        asm volatile (
            "mv a3, %[rs3]\n\t"
            ".insn r 0x6b, 0x02, 0x1, %[rd],  %[rs1], %[rs2], a3\n\t"
            : [rd] "=&r" (rej_uniform_res_hw[i])
            : [rs1] "r" ((inputs[i][0])), [rs2] "r" ((inputs[i][1])), [rs3] "r" ((inputs[i][2]))
            :  "a3","cc" );
    }
     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_rej_uniform_hw);
        printf("cycles_rej_uniform (HW): %d\n", cycles_rej_uniform_hw);
    #endif
    for (i = 0; i < NTESTS; i++) {
        if (rej_uniform_res_hw[i] != after_vals[i]) {
            printf("  [%2d] FAIL: in=(%3d, %3d, %3d), exp=%8d, got=%8d\n",
                i, inputs[i][0], inputs[i][1], inputs[i][2], after_vals[i], rej_uniform_res_hw[i]);
            all_rej_uniform_pass = 0;
        }
    }
    if (all_rej_uniform_pass_hw) {
        printf("rej_uniform (HW) tests passed!\n");
    }

    
    return 0;
}