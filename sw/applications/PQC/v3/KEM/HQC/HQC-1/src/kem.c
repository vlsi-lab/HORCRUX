/**
 * @file kem.c
 * @brief Implementation of api.h
 */

#include <stdint.h>
#include <string.h>
#include "api.h"
#include "crypto_memset.h"
#include "hqc.h"
#include "parameters.h"
#include "parsing.h"
#include "symmetric.h"
#include "vector.h"

#include "hqc1_instructions.h"
#include "core_v_mini_mcu.h"
#include "csr.h"
#include <stdio.h>


static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
        if ((i + 1) % 16 == 0) printf("\n    "); // wrap every 16 bytes
    }
    printf("\n");
}

/**
 * @brief Generates a keypair for the KEM (Key Encapsulation Mechanism) scheme.
 *
 * This function generates a public/private keypair used for key encapsulation and decapsulation.
 * The encapsulation key (`ek`) is used to encapsulate a shared secret, while the decapsulation key (`dk`)
 * is used to recover it.
 *
 * @param[out] ek_kem Pointer to the output buffer where the encapsulation key will be stored.
 * @param[out] dk_kem Pointer to the output buffer where the decapsulation key will be stored.
 *
 * @return 0 on success.
 *
 * @pre The PRNG **must be seeded** with ::prng_init() before calling this function.
 * @warning This function calls ::prng_get_bytes() to sample `seed_kem`. If the PRNG has not been
 *          properly seeded beforehand, the generated keys will be insecure/predictable.
 * @note An example of correct seeding is provided in `main_hqc.c` (see `init_randomness()`), which
 *       seeds the PRNG using `syscall(SYS_getrandom, ...)` (32 bytes) by default..
 * @see prng_init, prng_get_bytes, main_hqc.c
 */
int crypto_kem_keypair(uint8_t *ek_kem, uint8_t *dk_kem) {

    uint8_t seed_kem[SEED_BYTES] = {0};
    uint8_t sigma[PARAM_SECURITY_BYTES] = {0};
    uint8_t seed_pke[SEED_BYTES] = {0};
    shake256_xof_ctx ctx_kem;

    uint8_t ek_pke[PUBLIC_KEY_BYTES] = {0};
    uint8_t dk_pke[SEED_BYTES] = {0};

    unsigned prng_get_bytes_cycles, xof_init_cycles, xof_get_bytes_cycles, hqc_pke_keygen_cycles;

    // Sample seed_kem
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif   
    prng_get_bytes(seed_kem, SEED_BYTES);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &prng_get_bytes_cycles);
        printf("prng_get_bytes_cycles cycles: %d\n", prng_get_bytes_cycles);        
    #endif

    //print_hex("seed_kem", seed_kem, SEED_BYTES);

    // Compute seed_pke and randomness sigma
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    xof_init(&ctx_kem, seed_kem, SEED_BYTES);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &xof_init_cycles);
        printf("xof_init_cycles cycles: %d\n", xof_init_cycles);    
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    xof_get_bytes(&ctx_kem, seed_pke, SEED_BYTES);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &xof_get_bytes_cycles);
        printf("xof_get_bytes_cycles cycles: %d\n", xof_get_bytes_cycles);  
    #endif
    xof_get_bytes(&ctx_kem, sigma, PARAM_SECURITY_BYTES);
    //print_hex("seed_pke", seed_pke, SEED_BYTES);
    //print_hex("sigma", sigma, PARAM_SECURITY_BYTES);

    // Compute HQC-PKE keypair
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hqc_pke_keygen(ek_pke, dk_pke, seed_pke);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_pke_keygen_cycles);
        printf("hqc_pke_keygen_cycles cycles: %d\n", hqc_pke_keygen_cycles);        
    #endif
    //print_hex("ek_pke", ek_pke, PUBLIC_KEY_BYTES);
    //print_hex("dk_pke", dk_pke, SEED_BYTES);

    // Compute HQC-KEM keypair
    memcpy(ek_kem, ek_pke, PUBLIC_KEY_BYTES);
    memcpy(dk_kem, ek_kem, PUBLIC_KEY_BYTES);
    memcpy(dk_kem + PUBLIC_KEY_BYTES, dk_pke, SEED_BYTES);
    memcpy(dk_kem + PUBLIC_KEY_BYTES + SEED_BYTES, sigma, PARAM_SECURITY_BYTES);
    memcpy(dk_kem + PUBLIC_KEY_BYTES + SEED_BYTES + PARAM_SECURITY_BYTES, seed_kem, SEED_BYTES);


    // Zeroize sensitive data
    memset_zero(seed_kem, sizeof seed_kem);
    memset_zero(sigma, sizeof sigma);
    memset_zero(seed_pke, sizeof seed_pke);
    memset_zero(dk_pke, sizeof dk_pke);

    return 0;
}

/**
 * @brief Performs key encapsulation using the KEM scheme.
 *
 * This function uses the encapsulation key (`ek`) to generate a ciphertext (`c_kem`) and a shared secret (`K`)..
 *
 * @param[out] c_kem   Pointer to the output buffer where the KEM ciphertext will be stored.
 * @param[out] K       Pointer to the output buffer where the shared secret will be stored.
 * @param[in]  ek_kem      Pointer to the encapsulation key.
 *
 * @return Returns 0 on success.
 *
 * @pre The PRNG **must be seeded** with ::prng_init() before calling this function.
 * @warning This function calls ::prng_get_bytes() to sample `seed_kem`. If the PRNG has not been
 *          properly seeded beforehand, the generated keys will be insecure/predictable.
 * @note An example of correct seeding is provided in `main_hqc.c` (see `init_randomness()`), which
 *       seeds the PRNG using `syscall(SYS_getrandom, ...)` (32 bytes) by default..
 * @see prng_init, prng_get_bytes, main_hqc.c
 */
int crypto_kem_enc(uint8_t *c_kem, uint8_t *K, const uint8_t *ek_kem) {
#ifdef VERBOSE
    printf("\n\n\n\n### ENCAPS ###");
#endif

    uint8_t m[PARAM_SECURITY_BYTES] = {0};
    uint8_t K_theta[SHARED_SECRET_BYTES + SEED_BYTES] = {0};
    uint8_t theta[SEED_BYTES] = {0};
    uint8_t hash_ek_kem[SEED_BYTES] = {0};
    ciphertext_kem_t c_kem_t = {0};

    unsigned prng_get_bytes_cycles, hash_h_cycles, hash_g_cycles, hqc_pke_encrypt_cycles;
    unsigned hqc_c_kem_to_string_cycles;

    // Sample message m and salt
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    prng_get_bytes(m, PARAM_SECURITY_BYTES);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &prng_get_bytes_cycles);
        printf("prng_get_bytes_cycles cycles: %d\n", prng_get_bytes_cycles);
    #endif
    prng_get_bytes(c_kem_t.salt, SALT_BYTES);

    // Compute shared key K and ciphertext c_kem
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);   
    #endif
    hash_h(hash_ek_kem, ek_kem);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_h_cycles);
        printf("hash_h_cycles cycles: %d\n", hash_h_cycles);        
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);   
    #endif
    hash_g(K_theta, hash_ek_kem, m, c_kem_t.salt);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_g_cycles);
        printf("hash_g_cycles cycles: %d\n", hash_g_cycles);    
    #endif
    memcpy(theta, K_theta + SEED_BYTES, SEED_BYTES);

    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);   
    #endif
    hqc_pke_encrypt(&c_kem_t.c_pke, ek_kem, (uint64_t *)m, theta);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_pke_encrypt_cycles);
        printf("hqc_pke_encrypt_cycles cycles: %d\n", hqc_pke_encrypt_cycles);  
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hqc_c_kem_to_string(c_kem, &c_kem_t);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_c_kem_to_string_cycles);
        printf("hqc_c_kem_to_string_cycles cycles: %d\n", hqc_c_kem_to_string_cycles);
    #endif
    memcpy(K, K_theta, SHARED_SECRET_BYTES);

    // Zeroize sensitive data
    memset_zero(m, sizeof m);
    memset_zero(K_theta, sizeof K_theta);
    memset_zero(theta, sizeof theta);

    return 0;
}

/**
 * @brief Performs key decapsulation using the KEM scheme.
 *
 * This function uses the decapsulation key (`dk`) to recover the shared secret (`K_prime`)
 * from the given KEM ciphertext (`c_kem`), which was generated during encapsulation.
 *
 * @param[out] K_prime   Pointer to the output buffer where the recovered shared secret will be stored.
 * @param[in]  c_kem     Pointer to the input KEM ciphertext.
 * @param[in]  dk_kem    Pointer to the decapsulation key.
 *
 * @return Returns 0 on success.
 */
int crypto_kem_dec(uint8_t *K_prime, const uint8_t *c_kem, const uint8_t *dk_kem) {

    uint8_t ek_pke[PUBLIC_KEY_BYTES] = {0};
    uint8_t dk_pke[SEED_BYTES] = {0};
    uint8_t sigma[PARAM_SECURITY_BYTES] = {0};
    uint8_t m_prime[PARAM_SECURITY_BYTES] = {0};
    uint8_t hash_ek_kem[SEED_BYTES] = {0};
    uint8_t K_theta_prime[SHARED_SECRET_BYTES + SEED_BYTES] = {0};
    uint8_t K_bar[SHARED_SECRET_BYTES] = {0};
    uint8_t theta_prime[SEED_BYTES] = {0};
    ciphertext_kem_t c_kem_t = {0};
    ciphertext_kem_t c_kem_prime_t = {0};
    uint8_t result;

    unsigned hqc_c_kem_from_string_cycles, hqc_pke_decrypt_cycles, hash_h_cycles, hash_g_cycles, hqc_pke_encrypt_cycles;
    unsigned vect_compare_cycles, hash_j_cycles;

    // Parse decapsulation key dk_kem
    memcpy(ek_pke, dk_kem, PUBLIC_KEY_BYTES);
    memcpy(dk_pke, dk_kem + PUBLIC_KEY_BYTES, SEED_BYTES);
    memcpy(sigma, dk_kem + PUBLIC_KEY_BYTES + SEED_BYTES, PARAM_SECURITY_BYTES);

    // Parse ciphertext c_kem
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hqc_c_kem_from_string(&c_kem_t.c_pke, c_kem_t.salt, c_kem);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_c_kem_from_string_cycles);
        printf("hqc_c_kem_from_string_cycles cycles: %d\n", hqc_c_kem_from_string_cycles);
    #endif

    // Compute message m_prime
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    result = hqc_pke_decrypt((uint64_t *)m_prime, dk_pke, &c_kem_t.c_pke);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_pke_decrypt_cycles);
        printf("hqc_pke_decrypt_cycles cycles: %d\n", hqc_pke_decrypt_cycles);
    #endif

    // Compute shared key K_prime and ciphertext c_kem_prime
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hash_h(hash_ek_kem, ek_pke);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_h_cycles);
        printf("hash_h_cycles cycles: %d\n", hash_h_cycles);
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);   
    #endif
    hash_g(K_theta_prime, hash_ek_kem, m_prime, c_kem_t.salt);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_g_cycles);
        printf("hash_g_cycles cycles: %d\n", hash_g_cycles);
    #endif

    memcpy(K_prime, K_theta_prime, SHARED_SECRET_BYTES);
    memcpy(theta_prime, K_theta_prime + SHARED_SECRET_BYTES, SEED_BYTES);

    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hqc_pke_encrypt(&c_kem_prime_t.c_pke, ek_pke, (uint64_t *)m_prime, theta_prime);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hqc_pke_encrypt_cycles);
        printf("hqc_pke_encrypt_cycles cycles: %d\n", hqc_pke_encrypt_cycles);
    #endif

    memcpy(c_kem_prime_t.salt, c_kem_t.salt, SALT_BYTES);

    // Compute rejection key K_bar
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hash_j(K_bar, hash_ek_kem, sigma, &c_kem_t);
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_j_cycles);
        printf("hash_j_cycles cycles: %d\n", hash_j_cycles);
    #endif

    // If c_kem != c_kem_prime or if decryption failed (result != 0), set K_prime = K_bar
    // Note: result = 0x00..00 if decryption is successful,
    //       result = 0xFF..FF if decryption fails
    #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif    
    result |= vect_compare((uint8_t *)c_kem_t.c_pke.u, (uint8_t *)c_kem_prime_t.c_pke.u, VEC_N_SIZE_BYTES);
     #if PERF_CNT_CYCLES == 1 && PROFILING_KEM == 1
        CSR_READ(CSR_REG_MCYCLE, &vect_compare_cycles);
        printf("vect_compare_cycles cycles: %d\n", vect_compare_cycles);
    #endif
   
    result |= vect_compare((uint8_t *)c_kem_t.c_pke.v, (uint8_t *)c_kem_prime_t.c_pke.v, VEC_N1N2_SIZE_BYTES);
    result |= vect_compare(c_kem_t.salt, c_kem_prime_t.salt, SALT_BYTES);
    result -= 1;
    for (size_t i = 0; i < SHARED_SECRET_BYTES; ++i) {
        K_prime[i] = (K_prime[i] & result) ^ (K_bar[i] & ~result);
    }

    // Zeroize sensitive data
    memset_zero(dk_pke, sizeof dk_pke);
    memset_zero(sigma, sizeof sigma);
    memset_zero(m_prime, sizeof m_prime);
    memset_zero(K_theta_prime, sizeof K_theta_prime);
    memset_zero(K_bar, sizeof K_bar);
    memset_zero(theta_prime, sizeof theta_prime);

    return 0;
}
