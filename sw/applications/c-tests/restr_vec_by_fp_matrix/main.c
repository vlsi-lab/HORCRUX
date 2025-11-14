#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))

#define RESTR_TO_VAL(x) ( (FP_ELEM) (RESTR_G_TABLE >> (8*(uint64_t)(x))) )
#define RESTR_G_TABLE ((uint64_t) (0x0140201008040201))
#define RESTR_G_GEN 2
#define FP_ELEM uint8_t

#define K 2
#define N_K 25

int main() {

    /* New test vectors from logs */
    FP_DOUBLEPREC e[K] = { 1, 2 };

    FP_DOUBLEPREC V_tr[K][N_K] = {
        { /* i = 0 */
        110, 57, 104, 41, 28, 71, 59, 119, 52, 31,
        33, 40, 122, 23, 50, 7, 26, 88, 109, 20,
        35, 97, 39, 3, 2
        },
        { /* i = 1 */
        14, 33, 84, 38, 37, 119, 101, 101, 55, 19,
        82, 72, 46, 108, 84, 16, 38, 22, 53, 66,
        38, 26, 78, 37, 125
        }
    };

    FP_DOUBLEPREC res[K][N_K] = {
        { /* i = 0, res[j] before update */
        2, 8, 4, 32, 8, 32, 64, 64, 8, 2,
        32, 2, 32, 16, 32, 2, 1, 64, 1, 32,
        64, 32, 2, 2, 4
        },
        { /* i = 1, res[j] before update (i.e., RED after i=0) */
        95, 122, 85, 114, 64, 47, 55, 48, 112, 64,
        98, 82, 22, 62, 5, 16, 53, 113, 92, 72,
        7, 99, 80, 8, 8
        }
    };

    FP_DOUBLEPREC golden_res[K][N_K] = {
        { /* i = 0, RED results */
        95, 122, 85, 114, 64, 47, 55, 48, 112, 64,
        98, 82, 22, 62, 5, 16, 53, 113, 92, 72,
        7, 99, 80, 8, 8
        },
        { /* i = 1, RED results */
        24, 127, 40, 12, 85, 15, 78, 71, 78, 13,
        45, 116, 79, 113, 87, 80, 78, 74, 50, 82,
        32, 76, 11, 29, 127
        }
    };


    FP_DOUBLEPREC computed_res[K][N_K];

    for (int i = 0; i < K; i++) {
        FP_ELEM restr_i = (FP_ELEM)RESTR_TO_VAL(e[i]);

        for (int j = 0; j < N_K; j++) {
            FP_DOUBLEPREC mul     = (FP_DOUBLEPREC)restr_i * (FP_DOUBLEPREC)V_tr[i][j];
            FP_DOUBLEPREC sum     = (FP_DOUBLEPREC)res[i][j] + mul;
            FP_DOUBLEPREC reduced = FPRED_DOUBLE(sum);
            computed_res[i][j] = (FP_ELEM)reduced;
        }
    }

    // Compare computed_res with golden_res
    printf("\n==== COMPARISON ====\n");
    bool success = true;
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            if (computed_res[i][j] != golden_res[i][j]) {
                printf("❌ Mismatch at [%d][%d]: got %u, expected %u\n",
                       i, j, computed_res[i][j], golden_res[i][j]);
                success = false;
            } else {
                printf("✅ Match at [%d][%d]: %u\n", i, j, computed_res[i][j]);
            }
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
