#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "vcd_util.h"


#include "core_v_mini_mcu.h"
#include "csr.h"


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



#define N_K 50

int main() {

    FP_ELEM u_prime[N_K] = {
        446,  55, 428, 270,  91, 409, 196, 198, 474,  20,
        318, 479,  14, 428, 321, 495, 300,  57,  60, 467,
        293, 431, 454, 215,  80, 380, 334, 302, 189, 305,
        32, 246,  90,  64, 221,  29, 220, 494, 164, 386,
        471,  58, 417, 341,  94, 235, 428, 484,  85,  50
    };

    FP_ELEM e[N_K] = {
        84, 116, 119,  38,  68,  60, 101,  48,  79,  18,
        117,  29,  81,  96,  23, 118,  83,  61,  36,  65,
        46,  80, 105,  83,  44, 117, 116,  90,  56,  76,
        26,  55,  17,  10,   6,  95,  73,  40,  76,  53,
        75,  23, 122,  51,  45,  72,  79,   2,   2,  95
    };

    FP_ELEM chall_1 = 199;

    FP_ELEM golden_res[N_K] = {
        122, 463,  40, 315, 336, 338, 312, 492, 465,  74,
        229, 423, 255, 205, 229,  89, 407, 448,  90, 456,
        489, 287, 206, 322,  41, 291, 233, 481, 214, 241,
        369, 343, 157, 414, 320, 365, 387, 307, 100, 281,
        467, 475, 267, 271, 488, 150, 419,  19, 129, 386
    };


    FP_ELEM computed_res[N_K];

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_K; i++){  
        computed_res[i] = FPRED_DOUBLE( (FP_DOUBLEPREC) u_prime[i] +
                            (FP_DOUBLEPREC) RESTR_TO_VAL(e[i]) * (FP_DOUBLEPREC) chall_1) ;
    }
    vcd_disable();


    return 0;

}
