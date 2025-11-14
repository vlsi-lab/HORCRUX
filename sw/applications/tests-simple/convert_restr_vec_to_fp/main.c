#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "core_v_mini_mcu.h"
#include "csr.h"
#include "vcd_util.h"

#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))

#define RESTR_TO_VAL(x) ( (FP_ELEM) (RESTR_G_TABLE >> (8*(uint64_t)(x))) )
#define RESTR_G_TABLE ((uint64_t) (0x0140201008040201))
#define RESTR_G_GEN 2
#define FP_ELEM uint8_t
#define N_K 50

int main() {

    FP_ELEM in[50] = {
        6, 6, 6, 3, 3, 7, 5, 1, 2, 7,
        2, 5, 5, 1, 5, 7, 7, 2, 5, 5,
        7, 5, 1, 2, 6, 6, 4, 7, 6, 3,
        2, 3, 1, 4, 5, 2, 5, 4, 5, 6,
        5, 7, 5, 2, 2, 4, 3, 4, 4, 1
    };

    FP_ELEM golden_res[50] = {
        64, 64, 64, 8, 8, 1, 32, 2, 4, 1,
        4, 32, 32, 2, 32, 1, 1, 4, 32, 32,
        1, 32, 2, 4, 64, 64, 16, 1, 64, 8,
        4, 8, 2, 16, 32, 4, 32, 16, 32, 64,
        32, 1, 32, 4, 4, 16, 8, 16, 16, 2
    };
    FP_ELEM computed_res[N_K];


    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_K; i++){  
        computed_res[i] =  RESTR_TO_VAL(in[i]);
    }
    vcd_disable();



    return 0;

}
