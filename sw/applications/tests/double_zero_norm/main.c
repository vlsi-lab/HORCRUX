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


static inline __attribute__((always_inline))
FP_ELEM double_zero_norm_hw(FP_ELEM a)
{
    FP_ELEM out;
    asm volatile (
        ".insn r 0x6b, 0x3, 2, %0, %1, x0, x0\n\t"
        : "=&r"(out)                        // rd (early-clobber)
        : "r"(a)                            // rs1 (via t0)
        : "t0", "cc"                        // we modify t0
    );
    return out;
}


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
    FP_ELEM computed_res_hw[N_K];


    unsigned int cycles;
    CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
    CSR_WRITE(CSR_REG_MCYCLE, 0);

    for (int i = 0; i < N_K; i++){  
        computed_res[i] = FP_DOUBLE_ZERO_NORM(v[i]);
    }

    CSR_READ(CSR_REG_MCYCLE, &cycles);
    printf("Number of clock cycles for SW : %d\n", cycles);   

    bool success = true;
    for (int i = 0; i < N_K; i++) {
        if (computed_res[i]!= golden_res[i]) {
            printf("❌ Mismatch at [%d]: got %u, expected %u\n",
                    i, computed_res[i], golden_res[i]);
            success = false;
        } 
    }

    CSR_WRITE(CSR_REG_MCYCLE, 0);
    for (int i = 0; i < N_K; i++){
        computed_res_hw[i] = double_zero_norm_hw(v[i]);
        //asm volatile (
        //    ".insn r 0x6b, 0x03, 0x2, %[rd], %[vi], x0, x0\n\t"
        //    : [rd] "=r" (computed_res_hw[i])
        //    : [vi] "r" (v[i])
        //    : "cc");
    }
    CSR_READ(CSR_REG_MCYCLE, &cycles);
    printf("Number of clock cycles for HW : %d\n", cycles);

    for (int i = 0; i < N_K; i++) {
        if (computed_res_hw[i]!= golden_res[i]) {
            printf("❌ Mismatch at [%d]: got %u, expected %u\n",
                    i, computed_res_hw[i], golden_res[i]);
            success = false;
        } 
    }

    if (success) {
        printf("\nAll tests passed ✅\n");
        return 0;
    } else {
        printf("\nSome tests failed ❌\n");
        return 1;
    }
}
