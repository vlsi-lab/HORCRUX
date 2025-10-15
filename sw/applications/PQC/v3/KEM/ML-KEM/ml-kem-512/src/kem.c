#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "params.h"
#include "kem.h"
#include "indcpa.h"
#include "verify.h"
#include "symmetric.h"
#include "randombytes.h"

#include "core_v_mini_mcu.h"
#include "csr.h"

#include "../include/mlkem512_instructions.h"

#include <stdio.h>
/*************************************************
* Name:        crypto_kem_keypair_derand
*
* Description: Generates public and private key
*              for CCA-secure Kyber key encapsulation mechanism
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
*                (an already allocated array of KYBER_SECRETKEYBYTES bytes)
*              - uint8_t *coins: pointer to input randomness
*                (an already allocated array filled with 2*KYBER_SYMBYTES random bytes)
**
* Returns 0 (success)
**************************************************/
int crypto_kem_keypair_derand(uint8_t *pk,
                              uint8_t *sk,
                              const uint8_t *coins)
{

  unsigned keypair_cycles, hash_h_cycles;        

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  indcpa_keypair_derand(pk, sk, coins);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &keypair_cycles);
      printf("keypair_cycles cycles: %d\n", keypair_cycles);
  #endif

  memcpy(sk+KYBER_INDCPA_SECRETKEYBYTES, pk, KYBER_PUBLICKEYBYTES);

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  hash_h(sk+KYBER_SECRETKEYBYTES-2*KYBER_SYMBYTES, pk, KYBER_PUBLICKEYBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &hash_h_cycles);
      printf("hash_h_cycles cycles: %d\n", hash_h_cycles);
  #endif
  /* Value z for pseudo-random output on reject */
  memcpy(sk+KYBER_SECRETKEYBYTES-KYBER_SYMBYTES, coins+KYBER_SYMBYTES, KYBER_SYMBYTES);
  

  return 0;
}

/*************************************************
* Name:        crypto_kem_keypair
*
* Description: Generates public and private key
*              for CCA-secure Kyber key encapsulation mechanism
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
*                (an already allocated array of KYBER_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
//int crypto_kem_keypair(uint8_t *pk,
//                       uint8_t *sk)
//{
//  uint8_t coins[2*KYBER_SYMBYTES];
//  randombytes(coins, 2*KYBER_SYMBYTES);
//  crypto_kem_keypair_derand(pk, sk, coins);
//  return 0;
//}

/*************************************************
* Name:        crypto_kem_enc_derand
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - uint8_t *ct: pointer to output cipher text
*                (an already allocated array of KYBER_CIPHERTEXTBYTES bytes)
*              - uint8_t *ss: pointer to output shared secret
*                (an already allocated array of KYBER_SSBYTES bytes)
*              - const uint8_t *pk: pointer to input public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*              - const uint8_t *coins: pointer to input randomness
*                (an already allocated array filled with KYBER_SYMBYTES random bytes)
**
* Returns 0 (success)
**************************************************/
int crypto_kem_enc_derand(uint8_t *ct,
                          uint8_t *ss,
                          const uint8_t *pk,
                          const uint8_t *coins)
{
  uint8_t buf[2*KYBER_SYMBYTES];
  /* Will contain key, coins */
  uint8_t kr[2*KYBER_SYMBYTES];

  memcpy(buf, coins, KYBER_SYMBYTES);

  unsigned hash_h_cycles, hash_g_cycles, enc_cycles;   

  /* Multitarget countermeasure for coins + contributory KEM */
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  hash_h(buf+KYBER_SYMBYTES, pk, KYBER_PUBLICKEYBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &hash_h_cycles);
      printf("hash_h_cycles cycles: %d\n", hash_h_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif 
  hash_g(kr, buf, 2*KYBER_SYMBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &hash_g_cycles);
      printf("hash_g_cycles cycles: %d\n", hash_g_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif 
  /* coins are in kr+KYBER_SYMBYTES */
  indcpa_enc(ct, buf, pk, kr+KYBER_SYMBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &enc_cycles);
      printf("kem-enc_cycles cycles: %d\n", enc_cycles);
  #endif

  memcpy(ss,kr,KYBER_SYMBYTES);
  return 0;
}

/*************************************************
* Name:        crypto_kem_enc
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - uint8_t *ct: pointer to output cipher text
*                (an already allocated array of KYBER_CIPHERTEXTBYTES bytes)
*              - uint8_t *ss: pointer to output shared secret
*                (an already allocated array of KYBER_SSBYTES bytes)
*              - const uint8_t *pk: pointer to input public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
//int crypto_kem_enc(uint8_t *ct,
//                   uint8_t *ss,
//                   const uint8_t *pk)
//{
//  uint8_t coins[KYBER_SYMBYTES];
//  randombytes(coins, KYBER_SYMBYTES);
//  crypto_kem_enc_derand(ct, ss, pk, coins);
//  return 0;
//}

/*************************************************
* Name:        crypto_kem_dec
*
* Description: Generates shared secret for given
*              cipher text and private key
*
* Arguments:   - uint8_t *ss: pointer to output shared secret
*                (an already allocated array of KYBER_SSBYTES bytes)
*              - const uint8_t *ct: pointer to input cipher text
*                (an already allocated array of KYBER_CIPHERTEXTBYTES bytes)
*              - const uint8_t *sk: pointer to input private key
*                (an already allocated array of KYBER_SECRETKEYBYTES bytes)
*
* Returns 0.
*
* On failure, ss will contain a pseudo-random value.
**************************************************/
int crypto_kem_dec(uint8_t *ss,
                   const uint8_t *ct,
                   const uint8_t *sk)
{
  int fail;
  uint8_t buf[2*KYBER_SYMBYTES];
  /* Will contain key, coins */
  uint8_t kr[2*KYBER_SYMBYTES];
//  uint8_t cmp[KYBER_CIPHERTEXTBYTES+KYBER_SYMBYTES];
  uint8_t cmp[KYBER_CIPHERTEXTBYTES];
  const uint8_t *pk = sk+KYBER_INDCPA_SECRETKEYBYTES;

  unsigned dec_cycles, hash_g_cycles, verify_cycles, rkprf_cycles;
  unsigned enc_cycles;

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  indcpa_dec(buf, ct, sk);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &dec_cycles);
      printf("dec_cycles cycles: %d\n", dec_cycles);
  #endif

  /* Multitarget countermeasure for coins + contributory KEM */
  memcpy(buf+KYBER_SYMBYTES, sk+KYBER_SECRETKEYBYTES-2*KYBER_SYMBYTES, KYBER_SYMBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  hash_g(kr, buf, 2*KYBER_SYMBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &hash_g_cycles);
      printf("hash_g_cycles cycles: %d\n", hash_g_cycles);
  #endif
  /* coins are in kr+KYBER_SYMBYTES */
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  indcpa_enc(cmp, buf, pk, kr+KYBER_SYMBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &enc_cycles);
      printf("enc cycles: %d\n", enc_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  fail = verify(ct, cmp, KYBER_CIPHERTEXTBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &verify_cycles);
      printf("verify_cycles cycles: %d\n", verify_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  /* Compute rejection key */
  rkprf(ss,sk+KYBER_SECRETKEYBYTES-KYBER_SYMBYTES,ct);
  #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
      CSR_READ(CSR_REG_MCYCLE, &rkprf_cycles);
      printf("rkprf_cycles cycles: %d\n", rkprf_cycles);
  #endif

  /* Copy true key to return buffer if fail is false */
  cmov(ss,kr,KYBER_SYMBYTES,!fail);

  return 0;
}
