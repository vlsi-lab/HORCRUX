#include <stdint.h>
#include "params.h"
#include "sign.h"
#include "packing.h"
#include "polyvec.h"
#include "poly.h"
#include "randombytes.h"
#include "symmetric.h"
#include "fips202.h"
#include "dilithium2_instructions.h"
#include "core_v_mini_mcu.h"
#include "csr.h"

#include <stdio.h>
#include <string.h>

/*************************************************
* Name:        crypto_sign_keypair
*
* Description: Generates public and private key.
*
* Arguments:   - uint8_t *pk: pointer to output public key (allocated
*                             array of CRYPTO_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (allocated
*                             array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_sign_keypair(uint8_t *pk, uint8_t *sk, uint8_t *seed) {
  
  uint8_t seedbuf[2*SEEDBYTES + CRHBYTES];
  uint8_t tr[TRBYTES];
  const uint8_t *rho, *rhoprime, *key;
  polyvecl mat[K];
  polyvecl s1, s1hat;
  polyveck s2, t1, t0;

  unsigned shake_cycles, polyvec_matrix_expand_cycles, polyvecl_uniform_eta_cycles, polyveck_uniform_eta_cycles;
  unsigned polyvecl_ntt_cycles, polyvec_matrix_pointwise_montgomery_cycles;
  unsigned polyveck_reduce_cycles, polyveck_invntt_tomont_cycles, polyveck_caddq_cycles, polyveck_power2round_cycles;
  unsigned pack_pk_cycles, shake256_cycles, pack_sk_cycles, polyveck_add_cycles;
  unsigned polyveck_ntt_cycles;
  unsigned crh_cycles;

  /* Get randomness for rho, rhoprime and key */
  //randombytes(seedbuf, SEEDBYTES);
  memset(seedbuf, 0, sizeof(seedbuf));
  memcpy(seedbuf, seed, SEEDBYTES);

  seedbuf[SEEDBYTES+0] = K;
  seedbuf[SEEDBYTES+1] = L;
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  shake256(seedbuf, 2*SEEDBYTES + CRHBYTES, seedbuf, SEEDBYTES+2);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &shake_cycles);
      printf("shake_cycles cycles: %d\n", shake_cycles);
  #endif
  rho = seedbuf;
  rhoprime = rho + SEEDBYTES;
  key = rhoprime + CRHBYTES;

  /* Expand matrix */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvec_matrix_expand(mat, rho);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyvec_matrix_expand_cycles);
      printf("polyvec_matrix_expand_cycles cycles: %d\n", polyvec_matrix_expand_cycles);
  #endif  

  /* Sample short vectors s1 and s2 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_uniform_eta(&s1, rhoprime, 0);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyvecl_uniform_eta_cycles);
      printf("polyvecl_uniform_eta_cycles cycles: %d\n", polyvecl_uniform_eta_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_uniform_eta(&s2, rhoprime, L);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyveck_uniform_eta_cycles);
      printf("polyveck_uniform_eta_cycles cycles: %d\n", polyveck_uniform_eta_cycles);
  #endif

  /* Matrix-vector multiplication */
  s1hat = s1;
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_ntt(&s1hat);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyvecl_ntt_cycles);
      printf("polyvecl_ntt_cycles cycles: %d\n", polyvecl_ntt_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvec_matrix_pointwise_montgomery(&t1, mat, &s1hat);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
      CSR_READ(CSR_REG_MCYCLE, &polyvec_matrix_pointwise_montgomery_cycles);
      printf("polyvec_matrix_pointwise_montgomery_cycles cycles: %d\n", polyvec_matrix_pointwise_montgomery_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_reduce(&t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyveck_reduce_cycles);
      printf("polyveck_reduce_cycles cycles: %d\n", polyveck_reduce_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_invntt_tomont(&t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyveck_invntt_tomont_cycles);
      printf("polyveck_invntt_tomont_cycles cycles: %d\n", polyveck_invntt_tomont_cycles);
  #endif

  /* Add error vector s2 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_add(&t1, &t1, &s2);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyveck_add_cycles);
      printf("polyveck_add_cycles cycles: %d\n", polyveck_add_cycles);
  #endif

  /* Extract t1 and write public key */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_caddq(&t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1     
      CSR_READ(CSR_REG_MCYCLE, &polyveck_caddq_cycles); 
      printf("polyveck_caddq_cycles cycles: %d\n", polyveck_caddq_cycles);
  #endif

  /* Pack t1 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_power2round(&t1, &t0, &t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &polyveck_power2round_cycles);
      printf("polyveck_power2round_cycles cycles: %d\n", polyveck_power2round_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  pack_pk(pk, rho, &t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
      CSR_READ(CSR_REG_MCYCLE, &pack_pk_cycles);
      printf("pack_pk_cycles cycles: %d\n", pack_pk_cycles);  
  #endif

  /* Compute H(rho, t1) and write secret key */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  shake256(tr, TRBYTES, pk, CRYPTO_PUBLICKEYBYTES);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &crh_cycles);
      printf("shake256_cycles cycles: %d\n", crh_cycles);  
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  pack_sk(sk, rho, tr, key, &t0, &s1, &s2);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &pack_sk_cycles);
      printf("pack_sk_cycles cycles: %d\n", pack_sk_cycles);
  #endif
  return 0;
}

/*************************************************
* Name:        crypto_sign_signature_internal
*
* Description: Computes signature. Internal API.
*
* Arguments:   - uint8_t *sig:   pointer to output signature (of length CRYPTO_BYTES)
*              - size_t *siglen: pointer to output length of signature
*              - uint8_t *m:     pointer to message to be signed
*              - size_t mlen:    length of message
*              - uint8_t *pre:   pointer to prefix string
*              - size_t prelen:  length of prefix string
*              - uint8_t *rnd:   pointer to random seed
*              - uint8_t *sk:    pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign_signature_internal(uint8_t *sig,
                                   size_t *siglen,
                                   uint8_t *m,
                                   size_t mlen,
                                   const uint8_t *pre,
                                   size_t prelen,
                                   const uint8_t rnd[RNDBYTES],
                                   const uint8_t *sk)
{
  unsigned int n;
  uint8_t seedbuf[2*SEEDBYTES + TRBYTES + 2*CRHBYTES];
  uint8_t *rho, *tr, *key, *mu, *rhoprime;
  uint16_t nonce = 0;
  polyvecl mat[K], s1, y, z;
  polyveck t0, s2, w1, w0, h;
  poly cp;
  keccak_state state;

  unsigned unpack_sk_cycles, shake256_cycles, polyvec_matrix_expand_cycles;
  unsigned polyveckl_ntt_cycles;
  unsigned polyvecl_ntt_cycles;
  unsigned polyveck_ntt_cycles;
  unsigned polyvecl_uniform_gamma1_cycles;
  unsigned polyvec_matrix_pointwise_montgomery_cycles;
  unsigned polyveck_reduce_cycles, polyveck_invntt_tomont_cycles, polyvecl_invntt_tomont_cycles;
  unsigned polyveck_caddq_cycles, polyveck_decompose_cycles;
  unsigned polyveck_pack_w1_cycles, poly_challenge_cycles;
  unsigned poly_ntt_cycles;
  unsigned polyveck_sub_cycles;
  unsigned polyveck_chknorm_cycles;
  unsigned make_hint_cycles;
  unsigned pack_sig_cycles;
  unsigned polyvecl_reduce_cycles;

  rho = seedbuf;
  tr = rho + SEEDBYTES;
  key = tr + TRBYTES;
  mu = key + SEEDBYTES;
  rhoprime = mu + CRHBYTES;
  /* Unpack secret key */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &unpack_sk_cycles);
    printf("unpack_sk_cycles cycles: %d\n", unpack_sk_cycles);  
  #endif
  /* Compute mu = CRH(tr, pre, msg) */
  /* Debug print */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif  
  shake256_init(&state);
  shake256_absorb(&state, tr, TRBYTES);
  shake256_absorb(&state, pre, prelen);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &shake256_cycles);
    printf("shake256_cycles cycles: %d\n", shake256_cycles);  
  #endif

  /* Compute rhoprime = CRH(key, rnd, mu) */
  shake256_init(&state);
  shake256_absorb(&state, key, SEEDBYTES);
  shake256_absorb(&state, rnd, RNDBYTES);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_finalize(&state);
  shake256_squeeze(rhoprime, CRHBYTES, &state);

  /* Expand matrix and transform vectors */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvec_matrix_expand(mat, rho);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvec_matrix_expand_cycles);
    printf("polyvec_matrix_expand_cycles cycles: %d\n", polyvec_matrix_expand_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_ntt(&s1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvecl_ntt_cycles);
    printf("polyvecl_ntt_cycles cycles: %d\n", polyvecl_ntt_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_ntt(&s2);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_ntt_cycles);
    printf("polyveck_ntt_cycles cycles: %d\n", polyveck_ntt_cycles);
  #endif

  polyveck_ntt(&t0);

rej:
  /* Sample intermediate vector y */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_uniform_gamma1(&y, rhoprime, nonce++);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvecl_uniform_gamma1_cycles);
    printf("polyvecl_uniform_gamma1_cycles cycles: %d\n", polyvecl_uniform_gamma1_cycles);
  #endif

  /* Matrix-vector multiplication */
  z = y;
  polyvecl_ntt(&z);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvec_matrix_pointwise_montgomery(&w1, mat, &z);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvec_matrix_pointwise_montgomery_cycles);
    printf("polyvec_matrix_pointwise_montgomery_cycles cycles: %d\n", polyvec_matrix_pointwise_montgomery_cycles);
  #endif

  /* Transform w1 back, reduce coefficients of w1 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif  
  polyveck_reduce(&w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_reduce_cycles);
    printf("polyveck_reduce_cycles cycles: %d\n", polyveck_reduce_cycles);    
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_invntt_tomont(&w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_invntt_tomont_cycles);
    printf("polyveck_invntt_tomont_cycles cycles: %d\n", polyveck_invntt_tomont_cycles);
  #endif

  /* Decompose w and call the random oracle */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_caddq(&w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_caddq_cycles);
    printf("polyveck_caddq_cycles cycles: %d\n", polyveck_caddq_cycles);
  #endif
  
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_decompose(&w1, &w0, &w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_decompose_cycles);
    printf("polyveck_decompose_cycles cycles: %d\n", polyveck_decompose_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_pack_w1(sig, &w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_pack_w1_cycles);
    printf("polyveck_pack_w1_cycles cycles: %d\n", polyveck_pack_w1_cycles);
  #endif


  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, sig, K*POLYW1_PACKEDBYTES);
  shake256_finalize(&state);
  shake256_squeeze(sig, CTILDEBYTES, &state);

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif  
  poly_challenge(&cp, sig);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &poly_challenge_cycles);
    printf("poly_challenge_cycles cycles: %d\n", poly_challenge_cycles);
  #endif

  /* Compute z = y + c*s1 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  poly_ntt(&cp);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &poly_ntt_cycles);
    printf("poly_ntt_cycles cycles: %d\n", poly_ntt_cycles);
  #endif

  /* Compute z, reject if it reveals secret */

  polyvecl_pointwise_poly_montgomery(&z, &cp, &s1);

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_invntt_tomont(&z);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvecl_invntt_tomont_cycles);
    printf("polyvecl_invntt_tomont_cycles cycles: %d\n", polyvecl_invntt_tomont_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_add(&z, &z, &y);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_sub_cycles);
    printf("polyveck_sub_cycles cycles: %d\n", polyveck_sub_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif

  polyvecl_reduce(&z);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_chknorm_cycles);
    printf("polyvecl_reduce_cycles cycles: %d\n", polyvecl_reduce_cycles);
  #endif

  if(polyvecl_chknorm(&z, GAMMA1 - BETA))
    goto rej;

  /* Check that subtracting cs2 does not change high bits of w and low bits
   * do not reveal secret information */
  polyveck_pointwise_poly_montgomery(&h, &cp, &s2);
  polyveck_invntt_tomont(&h);
  polyveck_sub(&w0, &w0, &h);
  polyveck_reduce(&w0);
  if(polyveck_chknorm(&w0, GAMMA2 - BETA))
    goto rej;

  /* Compute hints for w1 */
  polyveck_pointwise_poly_montgomery(&h, &cp, &t0);
  polyveck_invntt_tomont(&h);
  polyveck_reduce(&h);
  if(polyveck_chknorm(&h, GAMMA2))
    goto rej;

  polyveck_add(&w0, &w0, &h);
  n = polyveck_make_hint(&h, &w0, &w1);
  if(n > OMEGA)
    goto rej;

  /* Write signature */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  pack_sig(sig, sig, &z, &h);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &pack_sig_cycles);
    printf("pack_sig_cycles cycles: %d\n", pack_sig_cycles);
  #endif
  *siglen = CRYPTO_BYTES;
  return 0;
}

/*************************************************
* Name:        crypto_sign_signature
*
* Description: Computes signature.
*
* Arguments:   - uint8_t *sig:   pointer to output signature (of length CRYPTO_BYTES)
*              - size_t *siglen: pointer to output length of signature
*              - uint8_t *m:     pointer to message to be signed
*              - size_t mlen:    length of message
*              - uint8_t *ctx:   pointer to contex string
*              - size_t ctxlen:  length of contex string
*              - uint8_t *sk:    pointer to bit-packed secret key
*
* Returns 0 (success) or -1 (context string too long)
**************************************************/
int crypto_sign_signature(uint8_t *sig,
                          size_t *siglen,
                          uint8_t *m,
                          size_t mlen,
                          const uint8_t *ctx,
                          size_t ctxlen,
                          const uint8_t *sk,
                          const uint8_t rnd[RNDBYTES])
{
  size_t i;
  uint8_t pre[257];

  if(ctxlen > 255)
    return -1;

  /* Prepare pre = (0, ctxlen, ctx) */
  pre[0] = 0;
  pre[1] = ctxlen;
  for(i = 0; i < ctxlen; i++)
    pre[2 + i] = ctx[i];

  crypto_sign_signature_internal(sig,siglen,m,mlen,pre,2+ctxlen,rnd,sk);

  return 0;
}

/*************************************************
* Name:        crypto_sign
*
* Description: Compute signed message.
*
* Arguments:   - uint8_t *sm: pointer to output signed message (allocated
*                             array with CRYPTO_BYTES + mlen bytes),
*                             can be equal to m
*              - size_t *smlen: pointer to output length of signed
*                               message
*              - const uint8_t *m: pointer to message to be signed
*              - size_t mlen: length of message
*              - const uint8_t *ctx: pointer to context string
*              - size_t ctxlen: length of context string
*              - const uint8_t *sk: pointer to bit-packed secret key
*
* Returns 0 (success) or -1 (context string too long)
**************************************************/
int crypto_sign(uint8_t *sm,
                size_t *smlen,
                uint8_t *m,
                size_t mlen,
                const uint8_t *ctx,
                size_t ctxlen,
                const uint8_t *sk,
                const uint8_t rnd[RNDBYTES])
{
  int ret;
  size_t i;

  for(i = 0; i < mlen; ++i)
    sm[CRYPTO_BYTES + mlen - 1 - i] = m[mlen - 1 - i];
  ret = crypto_sign_signature(sm, smlen, sm + CRYPTO_BYTES, mlen, ctx, ctxlen, sk, rnd);
  *smlen += mlen;
  return ret;
}

/*************************************************
* Name:        crypto_sign_verify_internal
*
* Description: Verifies signature. Internal API.
*
* Arguments:   - uint8_t *m: pointer to input signature
*              - size_t siglen: length of signature
*              - const uint8_t *m: pointer to message
*              - size_t mlen: length of message
*              - const uint8_t *pre: pointer to prefix string
*              - size_t prelen: length of prefix string
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signature could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_verify_internal(const uint8_t *sig,
                                size_t siglen,
                                const uint8_t *m,
                                size_t mlen,
                                const uint8_t *pre,
                                size_t prelen,
                                const uint8_t *pk)
{
  unsigned int i;
  uint8_t buf[K*POLYW1_PACKEDBYTES];
  uint8_t rho[SEEDBYTES];
  uint8_t mu[CRHBYTES];
  uint8_t c[CTILDEBYTES];
  uint8_t c2[CTILDEBYTES];
  poly cp;
  polyvecl mat[K], z;
  polyveck t1, w1, h;
  keccak_state state;

  unsigned unpack_pk_cycles, shake256_cycles;
  unsigned unpack_sig_cycles, polyvecl_chknorm_cycles;
  unsigned poly_challenge_cycles, polyvec_matrix_expand_cycles;
  unsigned polyvecl_ntt_cycles, polyveck_ntt_cycles;
  unsigned polyveck_shiftl_cycles, polyveck_pointwise_poly_montgomery_cycles;
  unsigned polyveck_reduce_cycles, polyveck_invntt_tomont_cycles;
  unsigned polyveck_caddq_cycles;
  unsigned polyvec_matrix_pointwise_montgomery_cycles;
  unsigned poly_ntt_cycles;
  unsigned polyveck_sub_cycles;
  unsigned make_hint_cycles;
  unsigned polyveck_pack_w1_cycles;

  if(siglen != CRYPTO_BYTES)
    return -1;

  /* Unpack public key */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  unpack_pk(rho, &t1, pk);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &unpack_pk_cycles);
    printf("unpack_pk_cycles cycles: %d\n", unpack_pk_cycles);
  #endif

  if(unpack_sig(c, &z, &h, sig))
    return -1;
  if(polyvecl_chknorm(&z, GAMMA1 - BETA))
    return -1;

  /* Compute CRH(H(rho, t1), pre, msg) */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  shake256(mu, TRBYTES, pk, CRYPTO_PUBLICKEYBYTES);
  shake256_init(&state);
  shake256_absorb(&state, mu, TRBYTES);
  shake256_absorb(&state, pre, prelen);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &shake256_cycles);
    printf("shake256_cycles cycles: %d\n", shake256_cycles);  
  #endif

  /* Matrix-vector multiplication; compute Az - c2^dt1 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  poly_challenge(&cp, c);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &poly_challenge_cycles);
    printf("poly_challenge_cycles cycles: %d\n", poly_challenge_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvec_matrix_expand(mat, rho);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvec_matrix_expand_cycles);
    printf("polyvec_matrix_expand_cycles cycles: %d\n", polyvec_matrix_expand_cycles);  
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvecl_ntt(&z);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvecl_ntt_cycles);
    printf("polyvecl_ntt_cycles cycles: %d\n", polyvecl_ntt_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyvec_matrix_pointwise_montgomery(&w1, mat, &z);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyvec_matrix_pointwise_montgomery_cycles);
    printf("polyvec_matrix_pointwise_montgomery_cycles cycles: %d\n", polyvec_matrix_pointwise_montgomery_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  poly_ntt(&cp);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &poly_ntt_cycles);
    printf("poly_ntt_cycles cycles: %d\n", poly_ntt_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_shiftl(&t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_shiftl_cycles);
    printf("polyveck_shiftl_cycles cycles: %d\n", polyveck_shiftl_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif  
  polyveck_ntt(&t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_ntt_cycles);
    printf("polyveck_ntt_cycles cycles: %d\n", polyveck_ntt_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_pointwise_poly_montgomery(&t1, &cp, &t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_pointwise_poly_montgomery_cycles);
    printf("polyveck_pointwise_poly_montgomery_cycles cycles: %d\n", polyveck_pointwise_poly_montgomery_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_sub(&w1, &w1, &t1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_sub_cycles);
    printf("polyveck_sub_cycles cycles: %d\n", polyveck_sub_cycles);
  #endif


  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_reduce(&w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_reduce_cycles);
    printf("polyveck_reduce_cycles cycles: %d\n", polyveck_reduce_cycles);
  #endif


  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_invntt_tomont(&w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_invntt_tomont_cycles);
    printf("polyveck_invntt_tomont_cycles cycles: %d\n", polyveck_invntt_tomont_cycles);
  #endif

  /* Reconstruct w1 */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_caddq(&w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &polyveck_caddq_cycles);
    printf("polyveck_caddq_cycles cycles: %d\n", polyveck_caddq_cycles);
  #endif

  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_use_hint(&w1, &w1, &h);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
    CSR_READ(CSR_REG_MCYCLE, &make_hint_cycles);
    printf("make_hint_cycles cycles: %d\n", make_hint_cycles);
  #endif

  /* Compute challenge hash */
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
    CSR_WRITE(CSR_REG_MCYCLE, 0);
  #endif
  polyveck_pack_w1(buf, &w1);
  #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1 
    CSR_READ(CSR_REG_MCYCLE, &polyveck_pack_w1_cycles);
    printf("polyveck_pack_w1_cycles cycles: %d\n", polyveck_pack_w1_cycles);
  #endif

  
  /* Call random oracle and verify challenge */
  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, buf, K*POLYW1_PACKEDBYTES);
  shake256_finalize(&state);
  shake256_squeeze(c2, CTILDEBYTES, &state);
  for(i = 0; i < CTILDEBYTES; ++i)
    if(c[i] != c2[i])
      return -1;

  return 0;
}

/*************************************************
* Name:        crypto_sign_verify
*
* Description: Verifies signature.
*
* Arguments:   - uint8_t *m: pointer to input signature
*              - size_t siglen: length of signature
*              - const uint8_t *m: pointer to message
*              - size_t mlen: length of message
*              - const uint8_t *ctx: pointer to context string
*              - size_t ctxlen: length of context string
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signature could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_verify(const uint8_t *sig,
                       size_t siglen,
                       const uint8_t *m,
                       size_t mlen,
                       const uint8_t *ctx,
                       size_t ctxlen,
                       const uint8_t *pk)
{
  size_t i;
  uint8_t pre[257];

  if(ctxlen > 255)
    return -1;

  pre[0] = 0;
  pre[1] = ctxlen;
  for(i = 0; i < ctxlen; i++)
    pre[2 + i] = ctx[i];

  return crypto_sign_verify_internal(sig,siglen,m,mlen,pre,2+ctxlen,pk);
}

/*************************************************
* Name:        crypto_sign_open
*
* Description: Verify signed message.
*
* Arguments:   - uint8_t *m: pointer to output message (allocated
*                            array with smlen bytes), can be equal to sm
*              - size_t *mlen: pointer to output length of message
*              - const uint8_t *sm: pointer to signed message
*              - size_t smlen: length of signed message
*              - const uint8_t *ctx: pointer to context tring
*              - size_t ctxlen: length of context string
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_open(uint8_t *m,
                     size_t *mlen,
                     const uint8_t *sm,
                     size_t smlen,
                     const uint8_t *ctx,
                     size_t ctxlen,
                     const uint8_t *pk)
{
  size_t i;

  if(smlen < CRYPTO_BYTES)
    goto badsig;

  *mlen = smlen - CRYPTO_BYTES;
  if(crypto_sign_verify(sm, CRYPTO_BYTES, sm + CRYPTO_BYTES, *mlen, ctx, ctxlen, pk))
    goto badsig;
  else {
    /* All good, copy msg, return 0 */
    for(i = 0; i < *mlen; ++i)
      m[i] = sm[CRYPTO_BYTES + i];
    return 0;
  }

badsig:
  /* Signature verification failed */
  *mlen = 0;
  for(i = 0; i < smlen; ++i)
    m[i] = 0;

  return -1;
}
