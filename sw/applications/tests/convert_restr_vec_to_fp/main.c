#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "core_v_mini_mcu.h"
#include "csr.h"
#include "vcd_util.h"

#define ENABLE_convert_restr_vec_to_fp 1

#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))

#define RESTR_TO_VAL(x) ( (FP_ELEM) (RESTR_G_TABLE >> (8*(uint64_t)(x))) )
#define RESTR_G_TABLE ((uint64_t) (0x0140201008040201))
#define RESTR_G_GEN 2
#define FP_ELEM uint8_t
#define N_K 50

static inline __attribute__((always_inline))
FP_ELEM convert_restr(FP_ELEM a, FP_ELEM b)
{
    FP_ELEM out;
    asm volatile (
        "addi t0, %1, 0\n\t"              
        ".insn r 0x6b, 0x3, 1, %0, t0, %2, x0\n\t"
        : "=&r"(out)   : "r" (a),  "r" (b) : "t0", "cc"  
    );
    return out;
}

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
    FP_ELEM computed_res_hw[N_K];

    unsigned int cycles;
    CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
    CSR_WRITE(CSR_REG_MCYCLE, 0);


    for (int i = 0; i < N_K; i++){  
        computed_res[i] =  RESTR_TO_VAL(in[i]);
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
    for (int j = 0; j < N_K; j++) {
        //asm volatile (
        //    "mv a3, %[uno]\n\t"
        //    ".insn r 0x6b, 0x03, 0x1, %[rd], %[in], a3, x0\n\t"
        //    : [rd] "=r" (computed_res_hw[j])
        //    : [in] "r" (in[j]), [uno] "r" (1)
        //    : "cc", "a3"
        //);
        computed_res_hw[j] = convert_restr(in[j], 1);
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
