#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "vcd_util.h"

#include "core_v_mini_mcu.h"
#include "csr.h"

#define ENABLE_fp_synd_minus_fp_vec_scaled_G 1

/* The same base field and restriction are employed for all categories of RSDP */
#define   P (509)
#define   Z (127)
#define RESTR_G_GEN 16
#define FZ_ELEM uint8_t
#define FZ_DOUBLEPREC uint16_t
#define FP_ELEM uint16_t
#define FP_DOUBLEPREC uint32_t
#define FP_TRIPLEPREC uint32_t

/* Restricted subgroup generator */
#define RESTR_G_GEN 16
#define NUM_BITS_P (BITS_TO_REPRESENT(P))
/* Reduction modulo P=509 as shown in:
 * Hacker's Delight, Second Edition, Chapter 10, Figure 10-4
 * Works for integers in the range [0,4294967295] i.e. all uint32_t */
#define FPRED_SINGLE(x) (((x) - (((uint64_t)(x) * 2160140723) >> 40) * P))
#define FPRED_DOUBLE(x) (FPRED_SINGLE(x))
#define FPRED_OPPOSITE(x) (FPRED_SINGLE(P - (x)))
/* no redundant zero notation in F_509 */
#define FP_DOUBLE_ZERO_NORM(x) (x)

/* for i in [0,1,2,4,8,16,32,64] RESTR_G_GEN**i mod 509 yields
 * [1, 16, 256, 384, 355, 302, 93, 505]
 * the following is a precomputed-squares S&M, to be optimized into muxed
 * register stored tables */

#define RESTR_G_GEN_1  ((FP_ELEM)RESTR_G_GEN)
#define RESTR_G_GEN_2  ((FP_ELEM) 256)
#define RESTR_G_GEN_4  ((FP_ELEM) 384)
#define RESTR_G_GEN_8  ((FP_ELEM) 355)
#define RESTR_G_GEN_16 ((FP_ELEM) 302)
#define RESTR_G_GEN_32 ((FP_ELEM) 93)
#define RESTR_G_GEN_64 ((FP_ELEM) 505)

#define FP_ELEM_CMOV(BIT,TRUE_V,FALSE_V)  ( (((FP_ELEM)0 - (BIT)) & (TRUE_V)) | (~((FP_ELEM)0 - (BIT)) & (FALSE_V)) )

/* log reduction, constant time unrolled S&M w/precomputed squares.
 * To be further optimized with muxed register-fitting tables */
static inline
FP_ELEM RESTR_TO_VAL(FP_ELEM x){
    uint32_t res1, res2, res3, res4;
    res1 = ( FP_ELEM_CMOV(((x >> 0) &1),RESTR_G_GEN_1 ,1)) *
           ( FP_ELEM_CMOV(((x >> 1) &1),RESTR_G_GEN_2 ,1)) ;
    res2 = ( FP_ELEM_CMOV(((x >> 2) &1),RESTR_G_GEN_4 ,1)) *
           ( FP_ELEM_CMOV(((x >> 3) &1),RESTR_G_GEN_8 ,1)) ;
    res3 = ( FP_ELEM_CMOV(((x >> 4) &1),RESTR_G_GEN_16,1)) *
           ( FP_ELEM_CMOV(((x >> 5) &1),RESTR_G_GEN_32,1)) ;
    res4 =   FP_ELEM_CMOV(((x >> 6) &1),RESTR_G_GEN_64,1);

    /* Two intermediate reductions necessary:
     *     RESTR_G_GEN_1*RESTR_G_GEN_2*RESTR_G_GEN_4*RESTR_G_GEN_8    < 2^32
     *     RESTR_G_GEN_16*RESTR_G_GEN_32*RESTR_G_GEN_64               < 2^32 */
    return FPRED_SINGLE( FPRED_SINGLE(res1 * res2) * FPRED_SINGLE(res3 * res4) );
}

#define N_K 19

int main() {

    FP_ELEM s[N_K] = {
        214,  72, 149,  24, 252, 103, 376, 452, 486, 402,
        195,   1, 182,  22, 406, 399, 426, 396, 119
    };

    FP_ELEM synd[N_K] = {
        459, 219, 479, 481, 138, 403, 348, 477,  44,  34,
        226, 202, 179, 220, 355,  66, 250,  33, 369
    };

    FP_ELEM golden_res[N_K] = {
        265, 168, 225, 464, 214, 224, 421,  72, 336, 131,
        406, 477, 347, 162,  25, 356, 330,   7,   9
    };

    FP_ELEM chall_1 = 234;

    FP_ELEM computed_res[N_K];

    unsigned int cycles;
    CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
    CSR_WRITE(CSR_REG_MCYCLE, 0);


    #if ENABLE_fp_synd_minus_fp_vec_scaled_G
        for (int j = 0; j < N_K; j++) {
                asm volatile (
                "mv a5, %[uprime]\n\t"
                ".insn r 0x6b, 0x05, 0x3, %[rd], %[e], %[chall_1], a5\n\t"
                : [rd] "=r" (computed_res[j])
                : [e] "r" (s[j]), [chall_1] "r" (chall_1), [uprime] "r" (synd[j])
                : "cc", "a3", "a5");
        }
    #else
        for(int j = 0; j < N_K; j++){
            FP_ELEM tmp = FPRED_DOUBLE( (FP_DOUBLEPREC) s[j] * (FP_DOUBLEPREC) chall_1);
            tmp = FP_DOUBLE_ZERO_NORM(tmp);
            computed_res[j] = FPRED_SINGLE( (FP_DOUBLEPREC) synd[j] + FPRED_OPPOSITE(tmp) );
        }
    #endif
    
    CSR_READ(CSR_REG_MCYCLE, &cycles);
    printf("Number of clock cycles for HW : %d\n", cycles);

    // Compare computed_res with golden_res
    printf("\n==== COMPARISON ====\n");
    bool success = true;
    for (int i = 0; i < N_K; i++) {
        if (computed_res[i]!= golden_res[i]) {
            printf("❌ Mismatch at [%d]: got %u, expected %u\n",
                    i, computed_res[i], golden_res[i]);
            success = false;
        } else {
            //printf("✅ Match at [%d]: %u\n", i, computed_res[i]);
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
