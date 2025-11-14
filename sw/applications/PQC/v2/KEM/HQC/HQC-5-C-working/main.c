///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// Auth: Alessandra Dolmeta, Valeria Piscopo - Politecnico di Torino                 //
// Date: September 2025                                                              //
// Desc: Entry point for testing the HQC implementation using input and output       //
//       test vector coming from NIST                                                //
//                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "symmetric.h"
#include "test_vectors_5.h"


#define TEST_KEY        1
#define TEST_ENC        0
#define TEST_DEC        0



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

        //Alice generates a public key
        crypto_kem_keypair(pk, sk);

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

        crypto_kem_enc(ct, ss, pk);

        if(memcmp(ct, TVEC_OUT_CT, CRYPTO_CIPHERTEXTBYTES)) { printf("ERROR: CT mismatch\n");}
        if(memcmp(ss, TVEC_OUT_SS, CRYPTO_BYTES)) { printf("ERROR: SS mismatch\n");}
        printVect("ct", ct, CRYPTO_CIPHERTEXTBYTES);
        printVect("key_a", ss, CRYPTO_BYTES);   

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

        crypto_kem_dec(ss1, ct, sk);

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
