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


#define NTESTS 25

void gf_carryless_mul(uint8_t *c, uint8_t a, uint8_t b) {
    uint16_t       h = 0, l = 0, g, u[4];
    u[0] = 0;
    u[1] = b & ((1UL << 7) - 1UL);
    u[2] = u[1] << 1;
    u[3] = u[2] ^ u[1];
  
    g = 0;
    uint16_t tmp1 = a & 3;
  
    for(int i =0; i < 4; i++) {
      uint32_t tmp2 = tmp1 - i;
      g ^= (u[i] & -(1 - ((tmp2 | -tmp2) >> 31)));
    }
  
    l = g;
    h = 0;
  
    for (uint8_t i = 2; i < 8; i+=2) {
      g = 0;
      uint16_t tmp1 = (a >> i) & 3;
      for (int j = 0; j < 4; ++j) {
        uint32_t tmp2 = tmp1 - j;
        g ^= (u[j] & -(1 - ((tmp2 | -tmp2) >> 31)));
      }
      
      l ^= g << i;
      h ^= g >> (8 - i);
    }
  
    uint16_t mask = (-((b >> 7) & 1));
    l ^= ((a << 7) & mask);
    h ^= ((a >> (1)) & mask);
  
    c[0] = l;
    c[1] = h;
  
}


void gf_carryless_mul_HW(uint8_t *c, uint8_t a, uint8_t b) {
    uint16_t h = 0;
    uint16_t l = 0;
    uint16_t g;
    uint16_t gx[3];


    asm volatile (".insn r 0x3b, 0x01, 0x9, %[dst], %[rs1], %[rs2]\r\n" : [dst] "=&r" (l) : [rs1] "r" (a), [rs2] "r" (b) : );
    asm volatile (".insn r 0x3b, 0x01, 0xa, %[dst], %[rs1], %[rs2]\r\n" : [dst] "=&r" (gx[0]) : [rs1] "r" (a), [rs2] "r" (b) : ); 
    l ^= gx[0] << 2;
    h ^= gx[0] >> 6;
    asm volatile (".insn r 0x3b, 0x1, 0xb, %[dst], %[rs1], %[rs2]\r\n" : [dst] "=&r" (gx[1]) : [rs1] "r" (a), [rs2] "r" (b) : ); 
    l ^= gx[1] << 4;
    h ^= gx[1] >> 4;
    asm volatile (".insn r 0x3b, 0x1, 0xc, %[dst], %[rs1], %[rs2]\r\n" : [dst] "=&r" (gx[2]) : [rs1] "r" (a), [rs2] "r" (b) : ); 
    l ^= gx[2] << 6;
    h ^= gx[2] >> 2;

    uint16_t mask = (-((b >> 7) & 1));
    l ^= ((a << 7) & mask);
    h ^= ((a >> (1)) & mask);

    c[0] = l;
    c[1] = h;
}

int main() {

    uint8_t a[NTESTS] = {
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B
    };
    
    uint8_t b[NTESTS] = {
        0x75, 0x6F, 0x4B, 0x49, 0xE9, 0xF2, 0xE9, 0x41,
        0xD2, 0x15, 0x8B, 0x67, 0xAD, 0x43, 0x76, 0x69,
        0xD2, 0xAE, 0x6E, 0x4A, 0x45, 0xE4, 0x52, 0xFF,
        0xB5, 0x01
    };
    
    uint8_t c0_expected[NTESTS] = {
        0x37, 0xC9, 0x85, 0x93, 0x73, 0x86, 0x73, 0xCB,
        0xE6, 0x97, 0xC5, 0x91, 0x9F, 0xDD, 0x2A, 0xF3,
        0xE6, 0x82, 0xC2, 0x8E, 0xE7, 0x0C, 0x66, 0xF9,
        0x77, 0x0B
    };
    
    uint8_t c1_expected[NTESTS] = {
        0x03, 0x03, 0x02, 0x02, 0x06, 0x06, 0x06, 0x02,
        0x07, 0x00, 0x05, 0x03, 0x04, 0x02, 0x03, 0x03,
        0x07, 0x04, 0x03, 0x02, 0x02, 0x06, 0x02, 0x06,
        0x04, 0x00
    };

    uint16_t result_sw[NTESTS];
    uint16_t result_hw[NTESTS];
    int all_sw_tests_passed = 1;
    int all_hw_tests_passed = 1;

    uint8_t c[NTESTS][2];
    uint8_t c_hw[NTESTS][2];
    

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_gf_carryless_sw, cycles_gf_carryless_hw;
    #endif

    
    printf("gf_carryless Tests (SW).\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS; i++) {
        gf_carryless_mul(c[i], a[i], b[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_gf_carryless_sw);
        printf("gf_carryless (SW): %d\n", cycles_gf_carryless_sw);
    #endif

    for (int j = 1; j < NTESTS; j++) {
        if (c[j][0] != c0_expected[j] || c[j][1] != c1_expected[j]) {
            printf("Mismatch at %d: got (0x%02X,0x%02X), expected (0x%02X,0x%02X)\n",
                j, c[j][0], c[j][1], c0_expected[j], c1_expected[j]);
            all_sw_tests_passed = 0;
        }
    }
    if (all_sw_tests_passed) {
        printf("gf_carryless (SW) tests passed!\n");
    }
    

    printf("gf_carryless Tests (HW).\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS; i++) {
        gf_carryless_mul_HW(c_hw[i], a[i], b[i]);
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_gf_carryless_hw);
        printf("gf_carryless (HW): %d\n", cycles_gf_carryless_hw);
    #endif

    for (int j = 1; j < NTESTS; j++) {
        if (c_hw[j][0] != c0_expected[j] || c_hw[j][1] != c1_expected[j]) {
            printf("Mismatch at %d: got (0x%02X,0x%02X), expected (0x%02X,0x%02X)\n",
                j, c_hw[j][0], c_hw[j][1], c0_expected[j], c1_expected[j]);
            all_hw_tests_passed = 0;
        }
    }
    if (all_hw_tests_passed) {
        printf("gf_carryless (HW) tests passed!\n");
    }

    return 0;
}