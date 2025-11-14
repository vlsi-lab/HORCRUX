#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "vcd_util.h"


#include "core_v_mini_mcu.h"
#include "csr.h"


#define ENABLE_RESTR_TO_VAL_G 1

#define   P (509)
#define   Z (127)
#define RESTR_G_GEN 16
#define FZ_ELEM uint8_t
#define FZ_DOUBLEPREC uint16_t
#define FP_ELEM uint16_t
#define FP_DOUBLEPREC uint32_t
#define FP_TRIPLEPREC uint32_t

#define FPRED_SINGLE(x) (((x) - (((uint64_t)(x) * 2160140723) >> 40) * P))
#define FPRED_DOUBLE(x) (FPRED_SINGLE(x))
#define FPRED_OPPOSITE(x) (FPRED_SINGLE(P - (x)))
/* no redundant zero notation in F_509 */
#define FP_DOUBLE_ZERO_NORM(x) (x)

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

    // These correspond to v_in[36..54]
    FZ_ELEM in[N_K] = {
        95,  95,  126, 30,  82,  60,  51,  44,  98, 
        112, 99,  115, 113, 79,  55,  80,  44, 125, 101,
        7, 123, 14, 0, 127, 38, 66, 1, 92,
        24, 58, 71, 35, 110, 3, 49, 77, 12,
        64, 83, 26, 105, 41, 59, 2, 121, 18,
        96, 68, 43, 87
        
    };

    // These correspond to v_out[36..54]
    FP_ELEM golden_res[N_K] = {
        104, 104, 350, 62, 224, 281, 148, 97, 460, 145, 234, 426,
        284, 179, 333, 319, 97, 340, 351, 54, 57, 371, 1, 1, 123,
        503, 16, 174, 320, 357, 293, 196, 436, 24, 438, 289, 417,
        505, 21, 480, 408, 407, 113, 256, 38, 453, 137, 500, 356, 429
    };
    FP_ELEM computed_res[N_K];

    if (vcd_init() != 0)
    return 1;

    vcd_enable();
    for (int i = 0; i < N_K; i++){  
        asm volatile (
            ".insn r 0x6b, 0x05, 0x0, %[rd], %[in1], x0, x0\n\t"
            : [rd] "=r" (computed_res[i])
            : [in1] "r" (in[i])
            : "cc");
    }
    vcd_disable();
  
    return 0;

}
