#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FP_ELEM uint8_t
#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))
#define FP_DOUBLE_ZERO_NORM(x) (((x) + (((x) + 1) >> 7)) & 0x7F)

#define RESTR_TO_VAL(x) ( (FP_ELEM) (RESTR_G_TABLE >> (8*(uint64_t)(x))) )
#define RESTR_G_TABLE ((uint64_t) (0x0140201008040201))
#define RESTR_G_GEN 2


#define N_K 50

int main() {

    FP_ELEM u_prime[51] = {
        4, 93, 42, 47, 86, 99, 26, 121, 85, 72,
        65, 97, 88, 6, 26, 119, 39, 28, 12, 25,
        64, 122, 13, 29, 76, 126, 9, 85, 11, 44,
        87, 108, 94, 27, 103, 77, 118, 43, 7, 6,
        96, 7, 55, 54, 55, 31, 90, 63, 10, 27,
        22
    };

    FP_ELEM e[51] = {
        2, 3, 6, 5, 5, 0, 1, 2, 1, 4,
        1, 4, 5, 1, 1, 3, 4, 2, 4, 1,
        3, 5, 5, 5, 4, 2, 4, 6, 1, 3,
        5, 5, 0, 2, 6, 4, 2, 0, 4, 0,
        4, 5, 5, 6, 1, 0, 6, 3, 4, 4,
        5
    };

    FP_ELEM chall_1[51] = {
        88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
        88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
        88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
        88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
        88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
        88
    };

    FP_ELEM golden_res[51] = {
        102, 35, 86, 69, 108, 60, 75, 92, 7, 83,
        114, 108, 110, 55, 75, 61, 50, 126, 23, 74,
        6, 17, 35, 51, 87, 97, 20, 2, 60, 113,
        109, 3, 55, 125, 20, 88, 89, 4, 18, 94,
        107, 29, 77, 98, 104, 119, 7, 5, 21, 38,
        44
    };


    FP_ELEM computed_res[N_K];

    // Perform multiply-add-reduce and store in computed_res
    for (int i = 0; i < N_K; i++){  
        computed_res[i] = FPRED_DOUBLE( (FP_DOUBLEPREC) u_prime[i] +
                               (FP_DOUBLEPREC) RESTR_TO_VAL(e[i]) * (FP_DOUBLEPREC) chall_1[i]) ;
    }

    // Compare computed_res with golden_res
    printf("\n==== COMPARISON ====\n");
    bool success = true;
    for (int i = 0; i < N_K; i++) {
        if (computed_res[i]!= golden_res[i]) {
            printf("❌ Mismatch at [%d]: got %u, expected %u\n",
                    i, computed_res[i], golden_res[i]);
            success = false;
        } else {
            printf("✅ Match at [%d]: %u\n", i, computed_res[i]);
        }
    }

    if (success) {
        printf("\nAll tests passed ✅\n");
        return 0;
    } else {
        printf("\nSome tests failed ❌\n");
        return 1;
    }
}
