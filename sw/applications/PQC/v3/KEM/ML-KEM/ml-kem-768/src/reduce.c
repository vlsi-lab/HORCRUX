#include <stdint.h>
#include "params.h"
#include "reduce.h"

#include "../include/mlkem768_instructions.h"

/*************************************************
* Name:        montgomery_reduce
*
* Description: Montgomery reduction; given a 32-bit integer a, computes
*              16-bit integer congruent to a * R^-1 mod q, where R=2^16
*
* Arguments:   - int32_t a: input integer to be reduced;
*                           has to be in {-q2^15,...,q2^15-1}
*
* Returns:     integer in {-q+1,...,q-1} congruent to a * R^-1 modulo q.
**************************************************/
int16_t montgomery_reduce(int32_t a)
{
  //int16_t t;
  //t = (int16_t)a*QINV;
  //t = (a - (int32_t)t*KYBER_Q) >> 16;
  //return t;

    uint32_t lo = (uint32_t)a & 0xFFFF;
    // QINV = 0xF301 = (1<<15)+(1<<14)+(1<<13)+(1<<12)+(1<<9)+(1<<8)+(1<<0)
    uint32_t t32 = (lo << 15)
                 + (lo << 14)
                 + (lo << 13)
                 + (lo << 12)
                 + (lo <<  9)
                 + (lo <<  8)
                 +  lo;
    // Mask back down to 16 bits
    uint16_t t = (uint16_t)t32;
    // Step 2: m = t * q using only shifts and adds
    //    t*q = t*(2^11 + 2^10 + 2^8 + 1)
    int32_t m  = ( (int32_t)t << 11 )
               + ( (int32_t)t << 10 )
               + ( (int32_t)t <<  8 )
               +  (int32_t)t;
    // Step 3: (a + m) >> 16
    int16_t result = (int16_t)((a - m) >> 16);

    return result;
}

/*************************************************
* Name:        barrett_reduce
*
* Description: Barrett reduction; given a 16-bit integer a, computes
*              centered representative congruent to a mod q in {-(q-1)/2,...,(q-1)/2}
*
* Arguments:   - int16_t a: input integer to be reduced
*
* Returns:     integer in {-(q-1)/2,...,(q-1)/2} congruent to a modulo q.
**************************************************/
int16_t barrett_reduce(int16_t a) {
  //int16_t t;
  //const int16_t v = ((1<<26) + KYBER_Q/2)/KYBER_Q;
  //t  = ((int32_t)v*a + (1<<25)) >> 26;
  //t *= KYBER_Q;
  //return a - t;

  int32_t t1 = a;        //   1·x
  int32_t t2 = a << 1;   //   2·x
  int32_t t3 = a << 2;   //   4·x
  int32_t t4 = a << 3;   //   8·x
  int32_t t5 = a << 5;   //  32·x
  int32_t t6 = a << 7;   // 128·x
  int32_t t7 = a << 8;   // 256·x
  int32_t t8 = a << 9;   // 512·x
  int32_t t9 = a << 12;  // 4096·x

  int32_t t = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9;

  // 2. First reduction: >> 24
  t >>= 24;

  // 3. Reconstruction: t*(2^11 + 2^10 + 2^8 + 1)
  t = (t << 11) + (t << 10) + (t << 8) + t;

  // 4. Compute z before conditional subtract
  int16_t z = a - (int16_t)t;

  // 5. Conditional subtract of KYBER_Q
  if (z >= KYBER_Q) {
      z -= KYBER_Q;
  } 
  return z;
}

