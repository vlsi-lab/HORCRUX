#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "vcd_util.h"


#include "core_v_mini_mcu.h"
#include "csr.h"

#define ENABLE_fp_vec_by_fp_vec_pointwise_G 1

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

#define N_K 41

int main() {

    const FP_ELEM in1[N_K] = {
        284,  99, 195, 445,  20, 256, 438, 350, 376,  21,
        498, 470, 408, 104, 460, 322, 113,  66, 319,  23,
        460, 104, 470,  36, 174, 123, 408, 371, 293, 272,
        333, 292, 284, 336, 103, 183, 383, 333, 391, 280,
        54
    };

    const FP_ELEM in2[N_K] = {
        446,  55, 428, 270,  91, 409, 196, 198, 474,  20,
        318, 479,  14, 428, 321, 495, 300,  57,  60, 467,
        293, 431, 454, 215,  80, 380, 334, 302, 189, 305,
        32, 246,  90,  64, 221,  29, 220, 494, 164, 386,
        471
    };

    const FP_ELEM golden_res[N_K] = {
        432, 355, 493,  26, 293, 359, 336,  76,  74, 420,
        65, 152, 113, 229,  50,  73, 306, 199, 307,  52,
        404,  32, 109, 105, 177, 421, 369,  62, 405, 502,
        476,  63, 110, 126, 367, 217, 275,  95, 499, 172,
        493
    };


    FP_ELEM computed_res[N_K];

    unsigned int cycles;
    CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
    CSR_WRITE(CSR_REG_MCYCLE, 0);

    #if ENABLE_fp_vec_by_fp_vec_pointwise_G
        for (int i = 0; i < N_K; i++){  
            asm volatile (
                "mv a3, %[in1]\n\t"
                "mv a5, %[in2]\n\t"
                ".insn r 0x6b, 0x05, 0x1, %[rd], a3, a5, x0\n\t"
                : [rd] "=r" (computed_res[i])
                : [in1] "r" (in1[i]), [in2] "r" (in2[i])
                : "cc", "a3", "a5"
                );
        }
    #else
        for (int i = 0; i < N_K; i++){  
            computed_res[i] = FPRED_DOUBLE( (FP_DOUBLEPREC) in1[i] * (FP_DOUBLEPREC) in2[i] );
        }
    #endif

    CSR_READ(CSR_REG_MCYCLE, &cycles);
    printf("Number of clock cycles for HW : %d\n", cycles);


    // Compare computed_res with golden_res
    printf("\n==== COMPARISON ====\n");
    bool success = true;
    for (int i = 0; i < N_K; i++) {
        if (computed_res[i]!= golden_res[i]) {
            printf("❌ Mismatch at [%d]: got %u, expected %u\n",i, computed_res[i], golden_res[i]);
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
