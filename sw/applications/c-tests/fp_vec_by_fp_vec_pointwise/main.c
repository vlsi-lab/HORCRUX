#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FP_ELEM uint8_t
#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))
#define FP_DOUBLE_ZERO_NORM(x) (((x) + (((x) + 1) >> 7)) & 0x7F)

#define N_K 50

int main() {

    const FP_ELEM in1[N_K] = {
    64, 64, 64,  8,  8,  1, 32,  2,  4,  1,
    4, 32, 32,  2, 32,  1,  1,  4, 32, 32,
    1, 32,  2,  4, 64, 64, 16,  1, 64,  8,
    4,  8,  2, 16, 32,  4, 32, 16, 32, 64,
    32,  1, 32,  4,  4, 16,  8, 16, 16,  2
    };

    const FP_ELEM in2[N_K] = {
    4, 93, 42, 47, 86, 99, 26,121, 85, 72,
    65, 97, 88,  6, 26,119, 39, 28, 12, 25,
    64,122, 13, 29, 76,126,  9, 85, 11, 44,
    87,108, 94, 27,103, 77,118, 43,  7,  6,
    96,  7, 55, 54, 55, 31, 90, 63, 10, 27
    };

    const FP_ELEM golden_res[N_K] = {
    2,110, 21,122, 53, 99, 70,115, 86, 72,
    6, 56, 22, 12, 70,119, 39,112,  3, 38,
    64, 94, 26,116, 38, 63, 17, 85, 69, 98,
    94,102, 61, 51,121, 54, 93, 53, 97,  3,
    24,  7,109, 89, 93,115, 85,119, 33, 54
    };

    FP_ELEM computed_res[N_K];

    // Perform multiply-add-reduce and store in computed_res
    for (int i = 0; i < N_K; i++){  
        computed_res[i] = FPRED_DOUBLE( (FP_DOUBLEPREC) in1[i] * (FP_DOUBLEPREC) in2[i] );
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
            //printf("✅ Match at [%d]: %u\n", i, computed_res[i]);
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
