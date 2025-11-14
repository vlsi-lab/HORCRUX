#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "vcd_util.h"

#include "core_v_mini_mcu.h"
#include "csr.h"

#define FP_ELEM uint8_t
#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))
#define FP_DOUBLE_ZERO_NORM(x) (((x) + (((x) + 1) >> 7)) & 0x7F)

#define N_K 50


int main() {

    FP_ELEM v[N_K] = {
        24, 43, 38, 51,  5, 22, 67,119,107,  2,
        24,  2, 82, 37, 85,114,124,  9, 29, 90,
        124,  6, 22,121, 75, 88,105,109,126,  1,
        94, 31, 34, 96,  9, 75,106, 76, 60, 53,
        91,124, 86, 53,118,117, 65, 84,123, 127
    };

    FP_ELEM golden_res[N_K] = {
        24, 43, 38, 51,  5, 22, 67,119,107,  2,
        24,  2, 82, 37, 85,114,124,  9, 29, 90,
        124,  6, 22,121, 75, 88,105,109,126,  1,
        94, 31, 34, 96,  9, 75,106, 76, 60, 53,
        91,124, 86, 53,118,117, 65, 84,123, 0
    };

    FP_ELEM computed_res[N_K];

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_K; i++){  
        computed_res[i] = FP_DOUBLE_ZERO_NORM(v[i]);
    }
    vcd_disable();

   
    return 0;

}
