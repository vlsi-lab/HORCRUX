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
#include <inttypes.h>
#include <string.h>
#include <keccak32.h>
#include "vcd_util.h"


#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif



#define SIZE 50

int main(){

	static uint32_t D[SIZE] __attribute__ ((aligned (4)));
    static uint32_t D1[SIZE] __attribute__ ((aligned (4)));
	
    static uint32_t D_expected[SIZE];
	int i = 0;
    #if PERF_CNT_CYCLES
        unsigned int cycles;
    #endif

	memset(D, 0, sizeof(D));
	memset(D_expected, 0, sizeof(D_expected));
	 
    D[0] = 0x7369C667;
	D[1] = 0xEC4AFF51;
	D[2] = 0xABBACD29;
	D[3] = 0x00000010;
	D[31] = 0x80000000;

    D1[0] = 0x7369C667;
	D1[1] = 0xEC4AFF51;
	D1[2] = 0xABBACD29;
	D1[3] = 0x00000010;
	D1[31] = 0x80000000;

    unsigned char temp[1600];

	if (vcd_init() != 0)
    return 1;

    vcd_enable();
    Keccak32(D1);
    vcd_disable();
	
	
	return 0;

}
