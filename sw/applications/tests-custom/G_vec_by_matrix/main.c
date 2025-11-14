#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "vcd_util.h"


#include "core_v_mini_mcu.h"
#include "csr.h"


#define ENABLE_G_fp_vec_by_fp_matrix 1

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

static inline __attribute__((always_inline))
FP_DOUBLEPREC g_vec_by_matrix(FP_DOUBLEPREC a, FP_DOUBLEPREC b, FP_DOUBLEPREC c)
{
    FP_DOUBLEPREC out;
    asm volatile (
        "addi t0, %1, 0\n\t"                
        ".insn r 0x6b, 0x05, 0x1, %0, t0, %2, %3\n\t"
        : "=&r"(out)  : "r"(a), "r"(b), "r"(c) : "t0", "cc"                       
    );
    return out;
}

#define K 5
#define N_K 10

int main() {

    FP_DOUBLEPREC e[K] = { 432, 355, 432, 355, 432};

    FP_DOUBLEPREC V_tr[K][N_K] = {
        { /* i = 0 (e=432) */
        54, 61, 333, 454, 489, 465, 347, 278, 465,
        176
        },
        { /* i = 1 (e=355) */
        295, 391, 248, 205, 107, 284, 266, 467, 350,
        218
        },
        { /* i = 0 (e=432) */
        54, 61, 333, 454, 489, 465, 347, 278, 465,
        176
        },
        { /* i = 1 (e=355) */
        295, 391, 248, 205, 107, 284, 266, 467, 350,
        218
        },
        { /* i = 0 (e=432) */
        54, 61, 333, 454, 489, 465, 347, 278, 465,
        176
        }
    };

    FP_DOUBLEPREC res[K][N_K] = {
        { /* inputs for i=0 */
        275, 95, 499, 172, 493, 255, 500, 110, 508,
        347
        },
        { /* inputs for i=1 */
        189, 488, 308, 335, 506, 80, 249, 82, 333,
        29
        },
        { /* inputs for i=0 */
        275, 95, 499, 172, 493, 255, 500, 110, 508,
        347
        },
        { /* inputs for i=1 */
        189, 488, 308, 335, 506, 80, 249, 82, 333,
        29
        },
        { /* inputs for i=0 */
        275, 95, 499, 172, 493, 255, 500, 110, 508,
        347
        }
    };

    FP_DOUBLEPREC golden_res[K][N_K] = {
        { /* RED results for i=0 */
        189, 488, 308, 335, 506, 80, 249, 82, 333,
        29
        },
        { /* RED results for i=1 */
        60, 336, 291, 323, 316, 118, 5, 442, 387,
        51
        },
                { /* RED results for i=0 */
        189, 488, 308, 335, 506, 80, 249, 82, 333,
        29
        },
        { /* RED results for i=1 */
        60, 336, 291, 323, 316, 118, 5, 442, 387,
        51
        },
        { /* RED results for i=0 */
        189, 488, 308, 335, 506, 80, 249, 82, 333,
        29
        }
    };

    FP_DOUBLEPREC computed_res[K][N_K];


    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            computed_res[i][j] = g_vec_by_matrix(e[i], V_tr[i][j], res[i][j]);
        }
    }
    vcd_disable();

    return 0;

}
