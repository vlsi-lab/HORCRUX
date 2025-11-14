#include <stdint.h>
#include "params.h"
#include "reduce.h"

#include "../include/dilithium5_instructions.h"

/*************************************************
* Name:        montgomery_reduce
*
* Description: For finite field element a with -2^{31}Q <= a <= Q*2^31,
*              compute r \equiv a*2^{-32} (mod Q) such that -Q < r < Q.
*
* Arguments:   - int64_t: finite field element a
*
* Returns r.
**************************************************/
#if ENABLE_DILITHIUM_MONTG
  int32_t montgomery_reduce(int64_t a) {

    uint32_t temp2;
    int32_t result;

    temp2 = a >> 32;
    asm volatile (
        "mv a3, %[rs1]\n\t"
        "mv a2, %[rs2]\n\t"
        ".insn r 0x3b, 0x1, 0x6, %[rd], a3, a2\n\t"
        : [rd] "=r" (result)
        : [rs1] "r" ((int64_t) a), [rs2] "r" (temp2)
        : "a2", "a3", "cc" );

    return result;
  }

#else
  int32_t montgomery_reduce(int64_t a) {

      uint32_t lo = (uint32_t)a;
      uint32_t t  = (lo << 25)
                  + (lo << 24)
                  + (lo << 23)
                  + (lo << 13)
                  +  lo;            // all adds/wrap mod 2^32

      // 2) m = t * Q  via shifts: Q = (1<<23) - (1<<13) + 1
      int64_t m = ((int64_t)t << 23)
                - ((int64_t)t << 13)
                +  (int64_t)t;

      int32_t result = (int32_t)((a - m) >> 32);

      return result;
  }
#endif

/*************************************************
* Name:        reduce32
*
* Description: For finite field element a with a <= 2^{31} - 2^{22} - 1,
*              compute r \equiv a (mod Q) such that -6283008 <= r <= 6283008.
*
* Arguments:   - int32_t: finite field element a
*
* Returns r.
**************************************************/
#if ENABLE_DILITHIUM_REDUCE32
  int32_t reduce32(int32_t a) {
    int32_t z;
    asm volatile (
        "mv a3, %[rs1]\n\t"
        ".insn r 0x3b, 0x01, 0x11, %[rd], a3, x0\n\t"
        : [rd] "=r" (z)
        : [rs1] "r" (a)
        : "a4", "a5", "a3", "cc" );
  
    return z;
  }
#else
  int32_t reduce32(int32_t a) {
      int32_t z;
      //printf("a=0x%04X (%d)\n", a,a);
      int32_t hi =  a >> 23;
      int32_t lo = (a >> 22) & 1;
      int32_t t  = hi + lo;
      //printf("t=0x%04X (%d)\n", t,t);

      // 2) u = t * Q    with Q = 8 380 417 = 2^23 - 2^13 + 1
      int32_t u  = (t << 23)
                - (t << 13)
                +  t;
      //printf("u=0x%04X (%d)\n", u,u);
      z = a - u;
      //printf("m=0x%04X (%d)\n", z,z);


      /* 4. z in [0, Q), reduce once more */
      if (z >= Q) {
          z -= Q;
      } 
      //printf("z=0x%04X (%d)\n", z,z);


      return z;
  }
#endif

/*************************************************
* Name:        caddq
*
* Description: Add Q if input coefficient is negative.
*
* Arguments:   - int32_t: finite field element a
*
* Returns r.
**************************************************/

int32_t caddq(int32_t a) {
  a += (a >> 31) & Q;
  return a;
}

/*************************************************
* Name:        freeze
*
* Description: For finite field element a, compute standard
*              representative r = a mod^+ Q.
*
* Arguments:   - int32_t: finite field element a
*
* Returns r.
**************************************************/
int32_t freeze(int32_t a) {
  a = reduce32(a);
  a = caddq(a);
  return a;
}
