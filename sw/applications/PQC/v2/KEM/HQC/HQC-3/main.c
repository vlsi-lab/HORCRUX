///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "symmetric.h"
#include "test_vectors_3.h"

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif

#define TEST_KEY        1
#define TEST_ENC        1
#define TEST_DEC        1

void printVect(char* name, uint8_t* buf, size_t size) {
    printf("%s = ", name);
    for (int i=0; i<size; i++){
        printf("%02X", buf[i]);
    }
    printf("\n");
}


int main(void)
{
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t ct[CRYPTO_CIPHERTEXTBYTES] = {0};
    uint8_t ss[CRYPTO_BYTES] = {0};
    uint8_t ss1[CRYPTO_BYTES] = {0};    

    unsigned cycles_keygen, cycles_encaps, cycles_decaps;

    #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif


    printf("Started test.\n");
    memset(pk, 0, CRYPTO_PUBLICKEYBYTES);
    memset(sk, 0, CRYPTO_SECRETKEYBYTES);

    char test_str[50] = "Testing";
    #if TEST_KEY == 1
        strcat(test_str," keypair");
    #endif
    #if TEST_ENC == 1
        strcat(test_str," encaps.");
    #endif
    #if TEST_DEC == 1
        strcat(test_str," decaps.");
    #endif
    printf("%s\n", test_str);


    //prng_get_bytes(TVEC_SEED, 48);
    prng_init(TVEC_SEED, NULL, 48, 0);

    //************************************************* 
    // KEY
    //*************************************************
    #ifdef TEST_KEY

        #if PERF_CNT_CYCLES == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        crypto_kem_keypair(pk, sk);
        #if PERF_CNT_CYCLES == 1
            CSR_READ(CSR_REG_MCYCLE, &cycles_keygen);
            printf("Keygen cycles: %d\n", cycles_keygen);
        #endif

        if(memcmp(pk, TVEC_OUT_PK, CRYPTO_PUBLICKEYBYTES)) { printf("\nERROR: PK mismatch\n");}
        if(memcmp(sk, TVEC_OUT_SK, CRYPTO_SECRETKEYBYTES)) { printf("\nERROR: SK mismatch\n");}
        //printVect("pk", pk, CRYPTO_PUBLICKEYBYTES);
        //printVect("sk", sk, CRYPTO_SECRETKEYBYTES);
        //printf("\n");
    
    #endif /* TEST_KEY */   

    //************************************************* 
    // ENCAPSULATION
    //*************************************************     
    #ifdef TEST_ENC
         
        #ifndef TEST_KEY
            memcpy(pk, TVEC_OUT_PK, CRYPTO_PUBLICKEYBYTES);
        #endif  

        #if PERF_CNT_CYCLES == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        crypto_kem_enc(ct, ss, pk);
        #if PERF_CNT_CYCLES == 1
            CSR_READ(CSR_REG_MCYCLE, &cycles_encaps);
            printf("Encaps cycles: %d\n", cycles_encaps);
        #endif

        if(memcmp(ct, TVEC_OUT_CT, CRYPTO_CIPHERTEXTBYTES)) { printf("ERROR: CT mismatch\n");}
        if(memcmp(ss, TVEC_OUT_SS, CRYPTO_BYTES)) { printf("ERROR: SS mismatch\n");}
        //printVect("ct", ct, CRYPTO_CIPHERTEXTBYTES);
        //printVect("key_a", ss, CRYPTO_BYTES);   

    #endif /* TEST_ENC */

    //************************************************* 
    // DECAPSULATION
    //*************************************************

    #ifdef TEST_DEC
        #ifndef TEST_KEY
            memcpy(sk, TVEC_OUT_SK[0], CRYPTO_SECRETKEYBYTES);
        #endif 
        #ifndef TEST_ENC
            memcpy(ct, TVEC_OUT_CT[0], CRYPTO_CIPHERTEXTBYTES);
        #endif 

        #if PERF_CNT_CYCLES == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        crypto_kem_dec(ss1, ct, sk);
        #if PERF_CNT_CYCLES == 1
            CSR_READ(CSR_REG_MCYCLE, &cycles_decaps);
            printf("Decaps cycles: %d\n", cycles_decaps);
        #endif
        
        if(memcmp(ss1, TVEC_OUT_SS, CRYPTO_BYTES)) { printf("ERROR: SS mismatch\n");}
        //printVect("key_b", ss1, CRYPTO_BYTES);
    #endif /* TEST_DEC */

    #ifdef PRINT_VECT
            printVect("pk", pk, CRYPTO_PUBLICKEYBYTES);
            printVect("sk", sk, CRYPTO_SECRETKEYBYTES);
            printVect("ct", ct, CRYPTO_CIPHERTEXTBYTES);
            printVect("key_a", key_a, CRYPTO_BYTES);
            printVect("key_b", key_b, CRYPTO_BYTES);
            printf("\n");
    #endif 


    printf("Test Successful\n");


    return 0;
}
