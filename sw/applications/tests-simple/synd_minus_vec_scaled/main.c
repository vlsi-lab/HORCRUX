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
#define FPRED_OPPOSITE(x) ((x) ^ 0x7F)

#define N_K 50

#define ENABLE_fp_synd_minus_fp_vec_scaled 1

int main() {

    FP_ELEM s[50] = {
        24, 43, 38, 51, 5, 22, 67, 119, 107, 2,
        24, 2, 82, 37, 85, 114, 124, 9, 29, 90,
        124, 6, 22, 121, 75, 88, 105, 109, 126, 1,
        94, 31, 34, 96, 9, 75, 106, 76, 60, 53,
        91, 124, 86, 53, 118, 117, 65, 84, 123, 64
    };

    FP_ELEM synd[50] = {
        120, 71, 58, 66, 99, 66, 121, 82, 82, 47,
        99, 65, 65, 46, 32, 38, 85, 60, 57, 100,
        4, 55, 89, 91, 19, 93, 66, 13, 80, 54,
        57, 18, 61, 71, 16, 6, 9, 23, 91, 95,
        101, 31, 112, 83, 77, 94, 6, 26, 99, 68
    };

    FP_ELEM golden_res[50] = {
        69, 59, 9, 37, 9, 51, 58, 99, 61, 11,
        48, 29, 113, 15, 26, 18, 12, 25, 43, 4,
        58, 74, 74, 72, 66, 33, 81, 83, 98, 36,
        16, 95, 84, 121, 108, 53, 6, 52, 27, 30,
        114, 85, 88, 18, 112, 20, 106, 38, 44, 59
    };

    FP_ELEM chall_1 = 18;

    FP_ELEM computed_res[N_K];
    
    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for(int j = 0; j < N_K; j++){
        FP_ELEM tmp = FPRED_DOUBLE( (FP_DOUBLEPREC) s[j] * (FP_DOUBLEPREC) chall_1);
        tmp = FP_DOUBLE_ZERO_NORM(tmp);
        computed_res[j] = FPRED_SINGLE( (FP_DOUBLEPREC) synd[j] + FPRED_OPPOSITE(tmp) );
    }
    vcd_disable();

    
    return 0;

}
