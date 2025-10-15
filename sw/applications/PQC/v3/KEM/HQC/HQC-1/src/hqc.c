/**
 * @file hqc.c
 * @brief High-level HQC-PKE API implementation
 */

#include "hqc.h"
#include <stdint.h>
#include <string.h>
#include "code.h"
#include "crypto_memset.h"
#include "gf2x.h"
#include "parameters.h"
#include "symmetric.h"
#include "vector.h"
#include <stdio.h>

#include "hqc1_instructions.h"
#include "core_v_mini_mcu.h"
#include "csr.h"
/**
 * @brief Generates a key pair for the HQC public-key encryption (PKE) scheme.
 *
 * This function creates a public encryption key (`ek_pke`) and a private decryption key (`dk_pke`)
 * for use in the HQC PKE scheme. The key generation process is seeded with the given `seed` input.
 *
 * @param[out] ek_pke  Pointer to the buffer that will receive the encryption key.
 * @param[out] dk_pke  Pointer to the buffer that will receive the decryption key.
 * @param[in]  seed    Pointer to the seed used to deterministically generate the key pair.
 *
 */
void hqc_pke_keygen(uint8_t *ek_pke, uint8_t *dk_pke, uint8_t *seed) {
    uint8_t keypair_seed[2 * SEED_BYTES] = {0};
    uint8_t *seed_dk = keypair_seed;
    uint8_t *seed_ek = keypair_seed + SEED_BYTES;
    shake256_xof_ctx dk_xof_ctx = {0};
    shake256_xof_ctx ek_xof_ctx = {0};

    uint64_t x[VEC_N_SIZE_64] = {0};
    uint64_t y[VEC_N_SIZE_64] = {0};
    uint64_t h[VEC_N_SIZE_64] = {0};
    uint64_t s[VEC_N_SIZE_64] = {0};

    unsigned hash_i_cycles, xof_init_cycles, vect_sample_fixed_weight1_cycles;
    unsigned vect_set_random_cycles, vect_mul_cycles, vect_add_cycles;


    // Derive keypair seeds
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hash_i(keypair_seed, seed);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_i_cycles);
        printf("hash_i_cycles cycles: %d\n", hash_i_cycles);
    #endif

    // Compute decryption key
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    xof_init(&dk_xof_ctx, seed_dk, SEED_BYTES);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &xof_init_cycles);
        printf("xof_init_cycles cycles: %d\n", xof_init_cycles);
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    vect_sample_fixed_weight1(&dk_xof_ctx, y, PARAM_OMEGA);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_sample_fixed_weight1_cycles);
        printf("vect_sample_fixed_weight1_cycles cycles: %d\n", vect_sample_fixed_weight1_cycles);
    #endif

    vect_sample_fixed_weight1(&dk_xof_ctx, x, PARAM_OMEGA);

    // Compute encryption key
    xof_init(&ek_xof_ctx, seed_ek, SEED_BYTES);
    
    // Generate h
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif    
    vect_set_random(&ek_xof_ctx, h);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_set_random_cycles);
        printf("vect_set_random_cycles cycles: %d\n", vect_set_random_cycles);
    #endif

    // Compute s = x + h.y
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1  
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    vect_mul(s, y, h);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_mul_cycles);
        printf("vect_mul_cycles cycles: %d\n", vect_mul_cycles);        
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif  
    vect_add(s, x, s, VEC_N_SIZE_64);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_add_cycles);
        printf("vect_add_cycles cycles: %d\n", vect_add_cycles);    
    #endif

    // Parse encryption key to string
    memcpy(ek_pke, seed_ek, SEED_BYTES);
    memcpy(ek_pke + SEED_BYTES, s, VEC_N_SIZE_BYTES);

    // Parse decryption key to string
    memcpy(dk_pke, seed_dk, SEED_BYTES);


    // Zeroize sensitive data
    memset_zero(keypair_seed, sizeof keypair_seed);
    memset_zero(x, sizeof x);
    memset_zero(y, sizeof y);
    memset_zero(&dk_xof_ctx, sizeof dk_xof_ctx);
}

/**
 * @brief Encrypts a message using the HQC public-key encryption (PKE) scheme.
 *
 * This function performs encryption in the HQC PKE scheme. It uses the given encryption key (`ek_pke`)
 * and encryption randomness (`theta`) to encrypt the message `m`, producing a ciphertext `c_pke`.
 *
 * @param[out] c_pke     Pointer to the output ciphertext structure (PKE ciphertext).
 * @param[in]  ek_pke    Pointer to the encryption key.
 * @param[in]  m         Pointer to the message to be encrypted.
 * @param[in]  theta     Pointer to the encryption randomness used during encryption.
 *
 */
void hqc_pke_encrypt(ciphertext_pke_t *c_pke, const uint8_t *ek_pke, const uint64_t *m, const uint8_t *theta) {
    shake256_xof_ctx theta_xof_ctx = {0};
    uint64_t h[VEC_N_SIZE_64] = {0};
    uint64_t s[VEC_N_SIZE_64] = {0};
    uint64_t r1[VEC_N_SIZE_64] = {0};
    uint64_t r2[VEC_N_SIZE_64] = {0};
    uint64_t e[VEC_N_SIZE_64] = {0};
    uint64_t tmp[VEC_N_SIZE_64] = {0};

    unsigned xof_init_cycles, hqc_ek_pke_from_string_cycles, vect_sample_fixed_weight2_cycles, vect_mul_cycles, vect_add_cycles;
    unsigned code_encode_cycles, vect_resize_cycles, vect_truncate_cycles;

    // Initialize Xof using theta
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif    
    xof_init(&theta_xof_ctx, theta, SEED_BYTES);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &xof_init_cycles);
        printf("xof_init_cycles cycles: %d\n", xof_init_cycles);
    #endif

    // Retrieve h and s from public key
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hqc_ek_pke_from_string(h, s, ek_pke);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_ek_pke_from_string_cycles);
        printf("hqc_ek_pke_from_string_cycles cycles: %d\n", hqc_ek_pke_from_string_cycles);
    #endif

    // Generate re, e and r1
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    vect_sample_fixed_weight2(&theta_xof_ctx, r2, PARAM_OMEGA_R);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_sample_fixed_weight2_cycles);
        printf("vect_sample_fixed_weight2_cycles cycles: %d\n", vect_sample_fixed_weight2_cycles);
    #endif

    vect_sample_fixed_weight2(&theta_xof_ctx, e, PARAM_OMEGA_E);
    vect_sample_fixed_weight2(&theta_xof_ctx, r1, PARAM_OMEGA_R);

    // Compute u = r1 + r2.h
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif    
    vect_mul(c_pke->u, r2, h);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_mul_cycles);
        printf("vect_mul_cycles cycles: %d\n", vect_mul_cycles);        
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    vect_add(c_pke->u, r1, c_pke->u, VEC_N_SIZE_64);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_add_cycles);
        printf("vect_add_cycles cycles: %d\n", vect_add_cycles);    
    #endif

    // Compute v = C.encode(m)
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    code_encode(c_pke->v, m);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &code_encode_cycles);
        printf("code_encode_cycles cycles: %d\n", code_encode_cycles);    
    #endif

    // Compute v = C.encode(m) + Truncate(s.r2 + e)
    vect_mul(tmp, r2, s);
    vect_add(tmp, e, tmp, VEC_N_SIZE_64);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif    
    vect_truncate(tmp);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_truncate_cycles);
        printf("vect_truncate_cycles cycles: %d\n", vect_truncate_cycles);    
    #endif
    vect_add(c_pke->v, c_pke->v, tmp, VEC_N1N2_SIZE_64);


    // Zeroize sensitive data
    memset_zero(r1, sizeof r1);
    memset_zero(r2, sizeof r2);
    memset_zero(e, sizeof e);
    memset_zero(tmp, sizeof tmp);
    memset_zero(&theta_xof_ctx, sizeof theta_xof_ctx);
}

/**
 * @brief Decrypts a ciphertext using the HQC public-key encryption (PKE) scheme.
 *
 * This function performs decryption in the HQC PKE scheme. It uses the given decryption key (`dk_pke`)
 * to decrypt the ciphertext `c_pke`, recovering the original message `m`.
 *
 * @param[out] m         Pointer to the output buffer where the decrypted message will be stored.
 * @param[in]  dk_pke    Pointer to the decryption key.
 * @param[in]  c_pke     Pointer to the input ciphertext structure (PKE ciphertext).
 *
 * @return Returns 0 on success.
 *
 */
uint8_t hqc_pke_decrypt(uint64_t *m, const uint8_t *dk_pke, const ciphertext_pke_t *c_pke) {
    uint64_t y[VEC_N_SIZE_64] = {0};
    uint64_t tmp1[VEC_N_SIZE_64] = {0};
    uint64_t tmp2[VEC_N_SIZE_64] = {0};

    unsigned hqc_dk_pke_from_string_cycles, vect_truncate_cycles, vect_mul_cycles, vect_add_cycles, code_decode_cycles;

    // Parse decryption key dk_pke
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif  
    hqc_dk_pke_from_string(y, dk_pke);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_dk_pke_from_string_cycles);
        printf("hqc_dk_pke_from_string_cycles cycles: %d\n", hqc_dk_pke_from_string_cycles);        
    #endif

    // Compute u.y
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    vect_mul(tmp1, y, c_pke->u);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_mul_cycles);
        printf("vect_mul_cycles cycles: %d\n", vect_mul_cycles);        
    #endif
    // Truncate(u.y)
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    vect_truncate(tmp1);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_truncate_cycles);
        printf("vect_truncate_cycles cycles: %d\n", vect_truncate_cycles);        
    #endif
    // Compute v - Truncate(u.y)
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);   
    #endif
    vect_add(tmp2, c_pke->v, tmp1, VEC_N1N2_SIZE_64);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_add_cycles);
        printf("vect_add_cycles cycles: %d\n", vect_add_cycles);        
    #endif


    // Compute plaintext m
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    code_decode(m, tmp2);
    #if PERF_CNT_CYCLES == 1 && PROFILING_HQC == 1
        CSR_READ(CSR_REG_MCYCLE, &code_decode_cycles);
        printf("code_decode_cycles cycles: %d\n", code_decode_cycles);        
    #endif

    // Zeroize sensitive data
    memset_zero(y, sizeof y);
    memset_zero(tmp1, sizeof tmp1);
    memset_zero(tmp2, sizeof tmp2);

    return 0;
}
