#include <stdint.h>
#include "params.h"
#include "ntt.h"
#include "reduce.h"

#include "../include/mlkem768_instructions.h"

/* Code to generate zetas and zetas_inv used in the number-theoretic transform:

#define KYBER_ROOT_OF_UNITY 17

static const uint8_t tree[128] = {
  0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120,
  4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124,
  2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122,
  6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126,
  1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121,
  5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125,
  3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123,
  7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127
};

void init_ntt() {
  unsigned int i;
  int16_t tmp[128];

  tmp[0] = MONT;
  for(i=1;i<128;i++)
    tmp[i] = fqmul(tmp[i-1],MONT*KYBER_ROOT_OF_UNITY % KYBER_Q);

  for(i=0;i<128;i++) {
    zetas[i] = tmp[tree[i]];
    if(zetas[i] > KYBER_Q/2)
      zetas[i] -= KYBER_Q;
    if(zetas[i] < -KYBER_Q/2)
      zetas[i] += KYBER_Q;
  }
}
*/

const int16_t zetas[128] = {
  -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
   -171,   622,  1577,   182,   962, -1202, -1474,  1468,
    573, -1325,   264,   383,  -829,  1458, -1602,  -130,
   -681,  1017,   732,   608, -1542,   411,  -205, -1571,
   1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
    516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
   -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
   -398,   961, -1508,  -725,   448, -1065,   677, -1275,
  -1103,   430,   555,   843, -1251,   871,  1550,   105,
    422,   587,   177,  -235,  -291,  -460,  1574,  1653,
   -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
  -1590,   644,  -872,   349,   418,   329,  -156,   -75,
    817,  1097,   603,   610,  1322, -1285, -1465,   384,
  -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
  -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
   -108,  -308,   996,   991,   958, -1460,  1522,  1628
};

/*************************************************
* Name:        fqmul
*
* Description: Multiplication followed by Montgomery reduction
*
* Arguments:   - int16_t a: first factor
*              - int16_t b: second factor
*
* Returns 16-bit integer congruent to a*b*R^{-1} mod q
**************************************************/
static int16_t fqmul(int16_t a, int16_t b) {
  int16_t res;

  #if ENABLE_KYBER_MONTG
      int32_t prod;
      prod = (int32_t)a * b;
      asm volatile (
          "mv a3, %[rs1]\n\t"
          ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
          : [rd] "=r" (res)
          : [rs1] "r" (prod)
          : "a3", "cc" );
  #else
      res = montgomery_reduce((int32_t)a * b);
  #endif

  return res;
}

/*************************************************
* Name:        ntt
*
* Description: Inplace number-theoretic transform (NTT) in Rq.
*              input is in standard order, output is in bitreversed order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
#if ENABLE_KYBER_NTT

    /* Custom/hardware-accelerated NTT */
    void ntt(int16_t r[256]) {
        unsigned int len, start, j, k;
        int16_t t, zeta;
        int32_t product;

        k = 1;
        for (len = 128; len >= 2; len >>= 1) {
            for (start = 0; start < 256; start = j + len) {
                zeta = zetas[k++];
                for (j = start; j < start + len; j++) {
                  int32_t rj   = r[j];
                  int32_t rlen = r[j + len];
                  int32_t t;

                  asm volatile(
                      "mul   %[t],   %[z],   %[rlen]\n\t"
                      ".insn r 0x3b, 0x01, 0x5, %[t], %[t], x0\n\t"
                      "sub   %[rlen], %[rj],  %[t]\n\t"
                      "add   %[rj],   %[rj],  %[t]\n\t"
                      : [rj] "+r"(rj), [rlen] "+r"(rlen), [t] "=&r"(t)
                      : [z] "r"((int32_t)zeta)
                      : "cc"
                  );
                  r[j]       = (int16_t)rj;
                  r[j + len] = (int16_t)rlen;
                }
            }
        }
    }

#else

/* Reference NTT */
void ntt(int16_t r[256]) {
    unsigned int len, start, j, k;
    int16_t t, zeta;

    k = 1;
    for (len = 128; len >= 2; len >>= 1) {
        for (start = 0; start < 256; start = j + len) {
            zeta = zetas[k++];
            for (j = start; j < start + len; j++) {
                t = fqmul(zeta, r[j + len]);
                r[j + len] = r[j] - t;
                r[j]       = r[j] + t;
            }
        }
    }
}

#endif  /* ENABLE_KYBER_NTT */

/*************************************************
* Name:        invntt_tomont
*
* Description: Inplace inverse number-theoretic transform in Rq and
*              multiplication by Montgomery factor 2^16.
*              Input is in bitreversed order, output is in standard order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
#if ENABLE_KYBER_INTT
/* Custom/hardware-accelerated INTT */

void invntt(int16_t r[256]) {
    unsigned int start, len, j, k;
    int16_t t, zeta;
    const int16_t f = 1441; // mont^2/128
    int32_t t32;

    k = 127;
    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < 256; start = j + len) {
            zeta = zetas[k--];
            for (j = start; j < start + len; j++) {

              int32_t rj   = r[j];
              int32_t rlen = r[j + len];
              int32_t sum, diff, prod;

              asm volatile(
                  "add   %[sum],  %[rj],   %[rlen]\n\t"
                  ".insn r 0x3b, 0x01, 0x10, %[sum], %[sum], x0\n\t"
                  "sub   %[diff], %[rlen], %[rj]\n\t"
                  "mul   %[prod], %[z],    %[diff]\n\t"
                  ".insn r 0x3b, 0x01, 0x5, %[prod], %[prod], x0\n\t"
                  : [sum]  "=&r"(sum),
                  [diff] "=&r"(diff),
                  [prod] "=&r"(prod)
                  : [rj]   "r"(rj),
                  [rlen] "r"(rlen),
                  [z]    "r"((int32_t)zeta)
                  : "cc"
              );

              r[j]       = (int16_t)sum;   
              r[j + len] = (int16_t)prod;     
            }
        }
      }

    for (j = 0; j < 256; j++) {
      int32_t out, prod_lo;
      asm volatile(
          "mul    %[prod], %[x], %[f]\n\t"
          ".insn r 0x3b, 0x1, 0x5, %[out], %[prod], x0\n\t"
          : [out] "=&r"(out), [prod] "=&r"(prod_lo)
          : [x] "r"((int32_t)r[j]), [f] "r"((int32_t)f)
          : "cc", "a3"
      );
      r[j] = out;
    }
}

#else
  void invntt(int16_t r[256]) {
    unsigned int start, len, j, k;
    int16_t t, zeta;
    const int16_t f = 1441; // mont^2/128

    k = 127;
    for(len = 2; len <= 128; len <<= 1) {
      for(start = 0; start < 256; start = j + len) {
        zeta = zetas[k--];
        for(j = start; j < start + len; j++) {
          t = r[j];
          r[j] = barrett_reduce(t + r[j + len]);
          r[j + len] = r[j + len] - t;
          r[j + len] = fqmul(zeta, r[j + len]);
        }
      }
    }

    for(j = 0; j < 256; j++)
      r[j] = fqmul(r[j], f);
  }
#endif

/*************************************************
* Name:        basemul
*
* Description: Multiplication of polynomials in Zq[X]/(X^2-zeta)
*              used for multiplication of elements in Rq in NTT domain
*
* Arguments:   - int16_t r[2]: pointer to the output polynomial
*              - const int16_t a[2]: pointer to the first factor
*              - const int16_t b[2]: pointer to the second factor
*              - int16_t zeta: integer defining the reduction polynomial
**************************************************/
void basemul(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta)
{

  #if ENABLE_KYBER_MONTG

    int32_t r0i, r1i;

    asm volatile(
      // a6 = a[1]*b[1]; REDUCE(t1) in place
      "mul    a3, %[a1], %[b1]\n\t"
      ".insn  r 0x3b, 0x01, 0x5, a6, a3, x0\n\t"
      // a4 = t1*zeta; REDUCE(t2)
      "mul    a5, a6, %[z]\n\t"
      ".insn  r 0x3b, 0x01, 0x5, a4, a5, x0\n\t"
      // a3 = a[0]*b[0]; REDUCE(t3)
      "mul    a3 , %[a0], %[b0]\n\t"
      ".insn  r 0x3b, 0x01, 0x5, a7, a3, x0\n\t"
      // r0 = t2 + t3
      "add    %[r0], a7, a4\n\t"
      // t4 = a[0]*b[1]; REDUCE(t4)
      "mul    a5, %[a0], %[b1]\n\t"
      ".insn  r 0x3b, 0x01, 0x5, a6, a5, x0\n\t"
      // t5 = a[1]*b[0]; REDUCE(t5)
      "mul    a4, %[a1], %[b0]\n\t"
      ".insn  r 0x3b, 0x01, 0x5, a3, a4, x0\n\t"
      // r1 = t4 + t5
      "add    %[r1], a6, a3\n\t"
      : [r0] "=&r"(r0i), [r1] "=&r"(r1i)
      : [a0] "r"(a[0]), [a1] "r"(a[1]),
        [b0] "r"(b[0]), [b1] "r"(b[1]),
        [z]  "r"(zeta)
      : "a3", "a4",  "a5",  "a6",  "a7","cc"
    );

    r[0] = (int16_t)r0i;
    r[1] = (int16_t)r1i;


  #else
    //res = montgomery_reduce((int32_t)a * b);
    r[0]  = fqmul(a[1], b[1]);
    r[0]  = fqmul(r[0], zeta);
    r[0] += fqmul(a[0], b[0]);
    r[1]  = fqmul(a[0], b[1]);
    r[1] += fqmul(a[1], b[0]);
  #endif

}


//      int32_t prod1, prod2, prod3, prod4, prod5;
//      int16_t res, res2, res3, res4, res5;
//
//      prod1 = (int32_t)a[1] * b[1];
//      asm volatile (
//          "mv a3, %[rs1]\n\t"
//          ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
//          : [rd] "=r" (res)
//          : [rs1] "r" (prod1)
//          : "a3", "cc" );
//      prod2 = (int32_t)res * zeta;
//      asm volatile (
//          "mv a3, %[rs1]\n\t"
//          ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
//          : [rd] "=r" (res2)
//          : [rs1] "r" (prod2)
//          : "a3", "cc" );
//      prod3 = (int32_t)a[0] * b[0];
//      asm volatile (
//          "mv a3, %[rs1]\n\t"
//          ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
//          : [rd] "=r" (res3)
//          : [rs1] "r" (prod3)
//          : "a3", "cc" );
//      r[0] = res2 + res3;
//
//      
//      prod4 = (int32_t)a[0] * b[1];
//      asm volatile (
//          "mv a3, %[rs1]\n\t"
//          ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
//          : [rd] "=r" (res4)
//          : [rs1] "r" (prod4)
//          : "a3", "cc" );
//      prod5 = (int32_t)a[1] * b[0];
//      asm volatile (
//          "mv a3, %[rs1]\n\t"
//          ".insn r 0x3b, 0x01, 0x5, %[rd], a3, x0\n\t"
//          : [rd] "=r" (res5)
//          : [rs1] "r" (prod5)
//          : "a3", "cc" );
//      r[1] = res4 + res5;