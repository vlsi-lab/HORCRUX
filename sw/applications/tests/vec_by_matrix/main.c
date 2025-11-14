#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "vcd_util.h"

#include "core_v_mini_mcu.h"
#include "csr.h"

#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))

#define K 2
#define N_K 50

#define ENABLE_fp_vec_by_fp_matrix 1

static inline __attribute__((always_inline))
void fp_vec_by_fp_matrix(FP_DOUBLEPREC a, FP_DOUBLEPREC b, FP_DOUBLEPREC c, FP_DOUBLEPREC *result) {
        
    asm volatile (
        "addi t0, %1, 0\n\t" 
        ".insn r 0x6b, 0x03, 0x0, %0, t0, %2, %3\n\t"
        : [rd] "=r" (*result)
        : [e] "r" (a), [V_tr] "r" (b), [res] "r" (c)
        : "cc");
}

int main() {
    FP_DOUBLEPREC e[K] = {2, 110};

    FP_DOUBLEPREC V_tr[K][N_K] = {
        {
            110, 57, 104, 41, 28, 71, 59, 119, 52, 31,
            33, 40, 122, 23, 50, 7, 26, 88, 109, 20,
            35, 97, 39, 3, 2, 33, 41, 54, 123, 104,
            97, 47, 37, 99, 19, 125, 111, 65, 56, 4,
            121, 69, 45, 21, 30, 43, 112, 34, 88, 1
        },
        {
            14, 33, 84, 38, 37, 119, 101, 101, 55, 19,
            82, 72, 46, 108, 84, 16, 38, 22, 53, 66,
            38, 26, 78, 37, 125, 43, 77, 80, 81, 10,
            17, 37, 104, 85, 111, 108, 98, 79, 73, 87,
            34, 4, 59, 15, 1, 67, 85, 89, 84, 105
        }
    };

    FP_DOUBLEPREC res[K][N_K] = {
        {
            108, 47, 5, 6, 77, 63, 95, 19, 41, 11,
            43, 8, 115, 41, 42, 124, 37, 4, 105, 64,
            18, 38, 91, 36, 98, 91, 28, 95, 5, 26,
            64, 8, 107, 82, 40, 83, 34, 112, 122, 2,
            77, 53, 13, 7, 119, 27, 80, 91, 25, 116
        },
        {
            74, 34, 86, 88, 6, 78, 86, 3, 18, 73,
            109, 88, 105, 87, 15, 11, 89, 53, 69, 104,
            88, 105, 42, 42, 102, 30, 110, 76, 124, 107,
            4, 102, 54, 26, 78, 79, 2, 115, 107, 10,
            65, 64, 103, 49, 52, 113, 50, 32, 74, 118
        }
    };

    FP_DOUBLEPREC golden_res[K][N_K] = {
        {
            74, 34, 86, 88, 6, 78, 86, 3, 18, 73,
            109, 88, 105, 87, 15, 11, 89, 53, 69, 104,
            88, 105, 42, 42, 102, 30, 110, 76, 124, 107,
            4, 102, 54, 26, 78, 79, 2, 115, 107, 10,
            65, 64, 103, 49, 52, 113, 50, 32, 74, 118
        },
        {
            90, 108, 55, 77, 12, 87, 20, 64, 99, 4,
            112, 7, 85, 29, 111, 120, 78, 60, 57, 125,
            77, 44, 113, 48, 9, 61, 71, 113, 17, 64,
            96, 108, 64, 105, 96, 21, 114, 42, 9, 55,
            122, 123, 116, 48, 35, 117, 2, 43, 43, 111
        }
    };

    FP_DOUBLEPREC computed_res[K][N_K];
    FP_DOUBLEPREC computed_res_hw[K][N_K];

    unsigned int cycles, cycles_hw;
    CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
    CSR_WRITE(CSR_REG_MCYCLE, 0);
    

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            FP_DOUBLEPREC mul = (FP_DOUBLEPREC)e[i] * (FP_DOUBLEPREC)V_tr[i][j];
            FP_DOUBLEPREC sum = (FP_DOUBLEPREC)res[i][j] + mul;
            FP_DOUBLEPREC reduced = FPRED_DOUBLE(sum);
            computed_res[i][j] = reduced;
        }
    }

    CSR_READ(CSR_REG_MCYCLE, &cycles);
    printf("Number of clock cycles for SW : %d\n", cycles);

    bool success = true;
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            if (computed_res[i][j] != golden_res[i][j]) {
                printf("❌ Mismatch at [%d][%d]: got %u, expected %u\n", i, j, computed_res[i][j], golden_res[i][j]);
                success = false;
        }}
    }


    CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
    CSR_WRITE(CSR_REG_MCYCLE, 0);

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            fp_vec_by_fp_matrix((FP_DOUBLEPREC)e[i],  (FP_DOUBLEPREC)V_tr[i][j], (FP_DOUBLEPREC)res[i][j], &computed_res_hw[i][j]);
            //asm volatile (
            //    "mv a3, %[res]\n\t"
            //    "mv a5, %[V_tr]\n\t"
            //    ".insn r 0x6b, 0x03, 0x0, %[rd], %[e], a5, a3\n\t"
            //    : [rd] "=r" (computed_res[i][j])
            //    : [e] "r" (e[i]), [V_tr] "r" (V_tr[i][j]), [res] "r" (res[i][j])
            //    : "cc", "a3", "a5");
            //printf("e[%d]=%u, V_tr[%d][%d]=%u, res[%d][%d]=%u → mul=%u, sum=%u, RED=%u\n",
            //       i, e[i], i, j, V_tr[i][j], i, j, res[i][j], mul, sum, reduced);
        }
    }

    CSR_READ(CSR_REG_MCYCLE, &cycles_hw);
    printf("Number of clock cycles for HW : %d\n", cycles_hw);


    success = true;
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            if (computed_res_hw[i][j] != golden_res[i][j]) {
                printf("❌ Mismatch at [%d][%d]: got %u, expected %u\n", i, j, computed_res_hw[i][j], golden_res[i][j]);
                success = false;
            } 
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
