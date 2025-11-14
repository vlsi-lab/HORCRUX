#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "api.h"
#include "params.h"
#include "wots.h"
#include "fors.h"
#include "hash.h"
#include "thash.h"
#include "address.h"
#include "randombytes.h"
#include "utils.h"
#include "merkle.h"
#include "sphincs_instructions.h"
#include "core_v_mini_mcu.h"
#include "csr.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>   // for memset/explicit_bzero if available

#ifndef explicit_bzero
#define explicit_bzero(b,len) memset((b), 0, (len))
#endif



/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void)
{
    return CRYPTO_SECRETKEYBYTES;
}

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void)
{
    return CRYPTO_PUBLICKEYBYTES;
}

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void)
{
    return CRYPTO_BYTES;
}

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void)
{
    return CRYPTO_SEEDBYTES;
}

/*
 * Generates an SPX key pair given a seed of length
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed)
{
    spx_ctx ctx;
    unsigned init_hash_cycles, merkle_gen_root_cycles;

    /* Initialize SK_SEED, SK_PRF and PUB_SEED from seed. */
    memcpy(sk, seed, CRYPTO_SEEDBYTES);

    memcpy(pk, sk + 2*SPX_N, SPX_N);

    memcpy(ctx.pub_seed, pk, SPX_N);
    memcpy(ctx.sk_seed, sk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    initialize_hash_function(&ctx);  
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &init_hash_cycles);
      printf("inith_hash cycles: %u\n", init_hash_cycles);
    #endif
    /* Compute root node of the top-most subtree. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    merkle_gen_root(sk + 3*SPX_N, &ctx);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
      CSR_READ(CSR_REG_MCYCLE, &merkle_gen_root_cycles);
      printf("merkle_gen_root cycles: %u\n", merkle_gen_root_cycles);
    #endif
    memcpy(pk + SPX_N, sk + 3*SPX_N, SPX_N);

    return 0;
}

/*
 * Generates an SPX key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk, uint8_t *keypair_rnd)
{
  //unsigned char seed[CRYPTO_SEEDBYTES];
  //randombytes(seed, CRYPTO_SEEDBYTES);
  crypto_sign_seed_keypair(pk, sk, keypair_rnd);

  return 0;
}

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen, const uint8_t *sk, uint8_t* signature_rnd)
{
    spx_ctx ctx;
    const unsigned char *sk_prf = sk + SPX_N;
    const unsigned char *pk = sk + 2*SPX_N;

    unsigned char optrand[SPX_N];
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char root[SPX_N];
    uint32_t i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};


    unsigned init_hash_cycles, set_type_cycles, gen_message_random_cycles, hash_message_cycles;
    unsigned fors_sign_cycles, set_layer_addr_cycles, final_assign_cycles;
    unsigned set_tree_addr_cycles, copy_subtree_addr_cycles, set_keypair_addr_cycles;
    unsigned merkle_sign_cycles;

    memcpy(ctx.sk_seed, sk, SPX_N);
    memcpy(ctx.pub_seed, pk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    initialize_hash_function(&ctx);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &init_hash_cycles);
        printf("init_hash cycles: %u\n", init_hash_cycles); 
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &set_type_cycles);
        printf("set_type cycles: %u\n", set_type_cycles); 
    #endif

    /* Optionally, signing can be made non-deterministic using optrand.
       This can help counter side-channel attacks that would benefit from
       getting a large number of traces when the signer uses the same nodes. */
    //randombytes(optrand, SPX_N);


    /* Compute the digest randomization value. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    gen_message_random(sig, sk_prf, signature_rnd, m, mlen, &ctx);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &gen_message_random_cycles);
        printf("gen_message_random cycles: %u\n", gen_message_random_cycles); 
    #endif

    /* Derive the message digest and leaf index from R, PK and M. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_message_cycles);
        printf("hash_message cycles: %u\n", hash_message_cycles); 
    #endif

    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    /* Sign the message hash using FORS. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
             CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    fors_sign(sig, root, mhash, &ctx, wots_addr);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &fors_sign_cycles);
        printf("fors_sign cycles: %u\n", fors_sign_cycles); 
    #endif

    sig += SPX_FORS_BYTES;


    for (i = 0; i < SPX_D; i++) {
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
             CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        set_layer_addr(tree_addr, i);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &set_layer_addr_cycles);
            printf("set_layer_addr cycles: %u\n", set_layer_addr_cycles); 
        #endif
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        set_tree_addr(tree_addr, tree);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &set_tree_addr_cycles);
            printf("set_tree_addr cycles: %u\n", set_tree_addr_cycles); 
        #endif
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        copy_subtree_addr(wots_addr, tree_addr);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &copy_subtree_addr_cycles);
            printf("copy_subtree_addr cycles: %u\n", copy_subtree_addr_cycles); 
        #endif
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        set_keypair_addr(wots_addr, idx_leaf);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &set_keypair_addr_cycles);
            printf("set_keypair_addr cycles: %u\n", set_keypair_addr_cycles); 
        #endif

        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        merkle_sign(sig, root, &ctx, wots_addr, tree_addr, idx_leaf);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &merkle_sign_cycles);
            printf("merkle_sign cycles: %u\n", merkle_sign_cycles); 
        #endif

        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        /* Update the indices for the next layer. */
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    *siglen = SPX_BYTES;

    return 0;
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk)
{
    spx_ctx ctx;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];
    unsigned char root[SPX_N];
    unsigned char leaf[SPX_N];
    unsigned int i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t wots_pk_addr[8] = {0};

    unsigned init_hash_cycles, set_type_cycles;
    unsigned hash_message_cycles, set_tree_addr_cycles, set_keypair_addr_cycles;
    unsigned fors_pk_from_sig_cycles;
    unsigned set_layer_addr_cycles;
    unsigned merkle_pk_cycles;
    unsigned copy_subtree_addr_cycles;
    unsigned thash_cycles;
    unsigned compute_root_cycles;
    unsigned wots_pk_from_sig_cycles, copy_keypair_addr_cycles;

    if (siglen != SPX_BYTES) {
        return -1;
    }

    memcpy(ctx.pub_seed, pk, SPX_N);

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    initialize_hash_function(&ctx);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &init_hash_cycles);
        printf("init_hash cycles: %u\n", init_hash_cycles); 
    #endif

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

    /* Derive the message digest and leaf index from R || PK || M. */
    /* The additional SPX_N is a result of the hash domain separator. */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen, &ctx);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &hash_message_cycles);
        printf("hash_message cycles: %u\n", hash_message_cycles); 
    #endif
    sig += SPX_N;

    /* Layer correctly defaults to 0, so no need to set_layer_addr */
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    set_tree_addr(wots_addr, tree);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &set_tree_addr_cycles);
        printf("set_tree_addr cycles: %u\n", set_tree_addr_cycles); 
    #endif
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    set_keypair_addr(wots_addr, idx_leaf);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &set_keypair_addr_cycles);
        printf("set_keypair_addr cycles: %u\n", set_keypair_addr_cycles); 
    #endif

    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    fors_pk_from_sig(root, sig, mhash, &ctx, wots_addr);
    #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
        CSR_READ(CSR_REG_MCYCLE, &fors_pk_from_sig_cycles);
        printf("fors_pk_from_sig cycles: %u\n", fors_pk_from_sig_cycles); 
    #endif
    sig += SPX_FORS_BYTES;

    /* For each subtree.. */
    for (i = 0; i < SPX_D; i++) {
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        set_layer_addr(tree_addr, i);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &set_layer_addr_cycles);
            printf("set_layer_addr cycles: %u\n", set_layer_addr_cycles); 
        #endif
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        set_tree_addr(tree_addr, tree);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &set_tree_addr_cycles);
            printf("set_tree_addr cycles: %u\n", set_tree_addr_cycles); 
        #endif

        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        copy_subtree_addr(wots_addr, tree_addr);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &copy_subtree_addr_cycles);
            printf("copy_subtree_addr cycles: %u\n", copy_subtree_addr_cycles); 
        #endif
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        set_keypair_addr(wots_addr, idx_leaf);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &set_keypair_addr_cycles);
            printf("set_keypair_addr cycles: %u\n", set_keypair_addr_cycles); 
        #endif
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        copy_keypair_addr(wots_pk_addr, wots_addr);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &copy_keypair_addr_cycles);
            printf("copy_keypair_addr cycles: %u\n", copy_keypair_addr_cycles); 
        #endif

        /* The WOTS public key is only correct if the signature was correct. */
        /* Initially, root is the FORS pk, but on subsequent iterations it is
           the root of the subtree below the currently processed subtree. */
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif   
        wots_pk_from_sig(wots_pk, sig, root, &ctx, wots_addr);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &wots_pk_from_sig_cycles);
            printf("wots_pk_from_sig cycles: %u\n", wots_pk_from_sig_cycles); 
        #endif
        sig += SPX_WOTS_BYTES;

        /* Compute the leaf node using the WOTS public key. */
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        thash(leaf, wots_pk, SPX_WOTS_LEN, &ctx, wots_pk_addr);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &thash_cycles);
            printf("thash cycles: %u\n", thash_cycles); 
        #endif

        /* Compute the root node of this subtree. */
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        compute_root(root, leaf, idx_leaf, 0, sig, SPX_TREE_HEIGHT,
                     &ctx, tree_addr);
        #if PERF_CNT_CYCLES == 1 && PROFILING_SIGN == 1
            CSR_READ(CSR_REG_MCYCLE, &compute_root_cycles);
            printf("compute_root_cycles cycles: %u\n", compute_root_cycles); 
        #endif
        sig += SPX_TREE_HEIGHT * SPX_N;

        /* Update the indices for the next layer. */
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    /* Check if the root node equals the root node in the public key. */
    if (memcmp(root, pub_root, SPX_N)) {
        return -1;
    }

    return 0;
}


/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk, uint8_t *signature_rnd)
{
    size_t siglen;

    crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk, signature_rnd);
    memmove(sm + SPX_BYTES, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    /* The API caller does not necessarily know what size a signature should be
       but SPHINCS+ signatures are always exactly SPX_BYTES. */
    if (smlen < SPX_BYTES) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    *mlen = smlen - SPX_BYTES;

    if (crypto_sign_verify(sm, SPX_BYTES, sm + SPX_BYTES, (size_t)*mlen, pk)) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    /* If verification was successful, move the message to the right place. */
    memmove(m, sm + SPX_BYTES, *mlen);

    return 0;
}
