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


#define QINV_DILITHIUM   58728449
#define DILITHIUM_Q      8380417


#define NTESTS_BARRETT   100


int32_t reduce32(int32_t a) {
    int32_t z;

    int32_t hi =  a >> 23;
    int32_t lo = (a >> 22) & 1;
    int32_t t  = hi + lo;

    // 2) u = t * Q    with Q = 8 380 417 = 2^23 - 2^13 + 1
    int32_t u  = (t << 23)
               - (t << 13)
               +  t;
    z = a - u;

    /* 4. z in [0, Q), reduce once more */
    if (z >= DILITHIUM_Q) {
        z -= DILITHIUM_Q;
    } 

    return z;
}

// Prototype for hardware-accelerated Dilithium Barrett (via inline ASM)
void barrett_d_reduce_asm(int32_t a, int32_t *t) {
    asm volatile (
        "mv a3, %[rs1]\n\t"
        ".insn r 0x3b, 0x01, 0x11, %[rd], a3, x0\n\t"
        : [rd] "=r" (*t)
        : [rs1] "r" (a)
        : "a4", "a5", "a3", "cc" );
}

int main() {
    int i, j;


    // Dilithium Barrett test vectors
    static const int32_t barrett_inputs[] = { -6249078, -4731596, -8746172, -11473713, -16632820, -13809575, -16099273, -25489918, -20701098, -10493492, -5847695, -7942701, -15230171, -7845086, -13524555, -13279255, -20439821, -22117554, -18214320, -19654792, -19916093, -18776789, -9849519, -9262420, -16233593, -13561569, -8758200, -15539437, -9016633, -13143890, -8508973, -6870200, -6452685, -5264930, -13479628, -9043610, -8764474, -16503079, -14984277, -13417879, -13590264, -11468412, -2857511, -19137631, -18042256, -8029959, -9798593, -6111221, -5688505, -19774488, -13490979, -2749875, -5567610, -16810695, -8608751, -16589497, -20320556, -17528421, -7511725, -5370502, -11001957, -11130337, -19624574, -12485554, -21018473, -11428675, -17100452, -6701830, -14632724, -12761770, -7869569, -14513367, -10837724, -17612207, -22255872, -12328582, -12786257, -12550571, -20143287, -19234565, -6240677, -16596409, -3755889, -12347478, -6695927, -11681002, -11560761, -6981939, -20014532, -12836121, -5340041, -14188759, -11788618, -18831536, -9117406, -6098714, -9043218, -16140702, -9652120, -8056862, -5297363, -1449839, -12528457, -10032142, -5635580, -14377814, -12699637, -10078550, -8328970, -5486511, -18355895, -16542846, -14456989, -19093354, -17972626, -6677475, -8104106, -10939909, -13217673, -16952313, -11018875, -16080866, -6480842, -21895588, -5654951, -7307086, -17144998, -13929718, -11719971, 4668144, -14893999, -17851068, -7750111, -13440955, -14311789, -16918252, -16682804, -17858170, -4192531, -13755216, -6509959, -6505501, -7461922, -14910406, -14867514, -13116876, -18370729, -13999819, -5331119, -13701980, -6032811, -7980338, -9888751, -24186219, -290418, -19405662, -10660007, -12909502, -19466306, -4811967, -21389951, -13688121, -10719755, -10036396, -12225202, -18756559, -22233209, -10094614, -10333530, -10990038, -11527430, -22044422 };

    static const int32_t barrett_exp[] = { 2131339, 3648821, -365755, -3093296, 128014, 2951259, 661561, -348667, -3940264, -2113075, 2532722, 437716, 1530663, 535331, 3236279, 3481579, -3678987, 3023697, -1453486, -2893958, -3155259, -2015955, -1469102, -882003, 527241, 3199265, -377783, 1221397, -636216, 3616944, -128556, 1510217, 1927732, 3115487, 3281206, -663193, -384057, 257755, 1776557, 3342955, 3170570, -3087995, -2857511, -2376797, -1281422, 350458, -1418176, 2269196, 2691912, -3013654, 3269855, -2749875, 2812807, -49861, -228334, 171337, -3559722, -767587, 868692, 3009915, -2621540, -2749920, -2863740, -4105137, 4122778, -3048258, -339618, 1678587, 2128110, 3999064, 510848, 2247467, -2457307, -851373, 2885379, -3948165, 3974577, -4170154, -3382453, -2473731, 2139740, 164425, -3755889, -3967061, 1684490, -3300585, -3180344, 1398478, -3253698, 3924713, 3040376, 2572075, -3408201, -2070702, -736989, 2281703, -662801, 620132, -1271703, 323555, 3083054, -1449839, -4148040, -1651725, 2744837, 2383020, 4061197, -1698133, 51447, 2893906, -1595061, 217988, 2303845, -2332520, -1211792, 1702942, 276311, -2559492, 3543161, -191479, -2638458, 679968, 1899575, 3245663, 2725466, 1073331, -384164, 2831116, -3339554, -3712273, 1866835, -1090234, 630306, 3319879, 2449045, -157418, 78030, -1097336, -4192531, 3005618, 1870458, 1874916, 918495, 1850428, 1893320, 3643958, -1609895, 2761015, 3049298, 3058854, 2347606, 400079, -1508334, 955032, -290418, -2644828, -2279590, 3851332, -2705472, 3568450, 3751300, 3072713, -2339338, -1655979, -3844785, -1995725, 2908042, -1714197, -1953113, -2609621, -3147013, 3096829};


    int32_t barrett_res[NTESTS_BARRETT];
    int32_t barrett_res_hw[NTESTS_BARRETT];
    int all_barrett_pass = 1;
    int all_barrett_pass_hw = 1;

     #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
        unsigned cycles_barrett, cycles_barrett_hw;
    #endif


    printf("Dilithium Barrett Reduction Tests (SW):\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif


    for (i = 0; i < NTESTS_BARRETT; i++) {
        barrett_res[i] = reduce32(barrett_inputs[i]);
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

    printf("Dilithium Barrett Reduction Tests (HW)\n");
     #if PERF_CNT_CYCLES == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    for (int i = 0; i < NTESTS_BARRETT; i++) {
        barrett_d_reduce_asm(barrett_inputs[i], &barrett_res_hw[i]);
        //asm volatile (
        //    "mv a3, %[rs1]\n\t"
        //    ".insn r 0x3b, 0x1, 0x11, %[rd], a3, x0\n\t"
        //    : [rd] "=r" (barrett_res_hw[i])
        //    : [rs1] "r" (barrett_inputs[i])
        //    : "a3","cc" );
    }

     #if PERF_CNT_CYCLES == 1
        CSR_READ(CSR_REG_MCYCLE, &cycles_barrett_hw);
        printf("cycles_barrett_hw: %d\n", cycles_barrett_hw);
    #endif

    for (int i = 0; i < NTESTS_BARRETT; i++) {
        if (barrett_res_hw[i] != barrett_res[i]) {
            printf("Test %d failed: expected %d, got %d\n", i, barrett_res[i], barrett_res_hw[i]);
            all_barrett_pass_hw = 0;
        }
    }

    if (all_barrett_pass_hw) {
        printf("HW-barrett tests passed!\n");
    }



    
    return 0;
}