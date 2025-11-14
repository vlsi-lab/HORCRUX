#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FP_DOUBLEPREC uint16_t
#define FPRED_SINGLE(x) (((x) & 0x7F) + ((x) >> 7))
#define FPRED_DOUBLE(x) FPRED_SINGLE(FPRED_SINGLE(x))

#define K 2
#define N_K 51

int main() {
    FP_DOUBLEPREC e[K] = {2, 110};

    FP_DOUBLEPREC V_tr[K][N_K] = {
        {
            110, 57, 104, 41, 28, 71, 59, 119, 52, 31,
            33, 40, 122, 23, 50, 7, 26, 88, 109, 20,
            35, 97, 39, 3, 2, 33, 41, 54, 123, 104,
            97, 47, 37, 99, 19, 125, 111, 65, 56, 4,
            121, 69, 45, 21, 30, 43, 112, 34, 88, 1,
            13
        },
        {
            14, 33, 84, 38, 37, 119, 101, 101, 55, 19,
            82, 72, 46, 108, 84, 16, 38, 22, 53, 66,
            38, 26, 78, 37, 125, 43, 77, 80, 81, 10,
            17, 37, 104, 85, 111, 108, 98, 79, 73, 87,
            34, 4, 59, 15, 1, 67, 85, 89, 84, 105,
            111
        }
    };

    FP_DOUBLEPREC res[K][N_K] = {
        {
            108, 47, 5, 6, 77, 63, 95, 19, 41, 11,
            43, 8, 115, 41, 42, 124, 37, 4, 105, 64,
            18, 38, 91, 36, 98, 91, 28, 95, 5, 26,
            64, 8, 107, 82, 40, 83, 34, 112, 122, 2,
            77, 53, 13, 7, 119, 27, 80, 91, 25, 116,
            13
        },
        {
            74, 34, 86, 88, 6, 78, 86, 3, 18, 73,
            109, 88, 105, 87, 15, 11, 89, 53, 69, 104,
            88, 105, 42, 42, 102, 30, 110, 76, 124, 107,
            4, 102, 54, 26, 78, 79, 2, 115, 107, 10,
            65, 64, 103, 49, 52, 113, 50, 32, 74, 118,
            39
        }
    };

    FP_DOUBLEPREC golden_res[K][N_K] = {
        {
            74, 34, 86, 88, 6, 78, 86, 3, 18, 73,
            109, 88, 105, 87, 15, 11, 89, 53, 69, 104,
            88, 105, 42, 42, 102, 30, 110, 76, 124, 107,
            4, 102, 54, 26, 78, 79, 2, 115, 107, 10,
            65, 64, 103, 49, 52, 113, 50, 32, 74, 118,
            39
        },
        {
            90, 108, 55, 77, 12, 87, 20, 64, 99, 4,
            112, 7, 85, 29, 111, 120, 78, 60, 57, 125,
            77, 44, 113, 48, 9, 61, 71, 113, 17, 64,
            96, 108, 64, 105, 96, 21, 114, 42, 9, 55,
            122, 123, 116, 48, 35, 117, 2, 43, 43, 111,
            57
        }
    };

    FP_DOUBLEPREC computed_res[K][N_K];

    // Perform multiply-add-reduce and store in computed_res
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N_K; j++) {
            FP_DOUBLEPREC mul = (FP_DOUBLEPREC)e[i] * (FP_DOUBLEPREC)V_tr[i][j];
            FP_DOUBLEPREC sum = (FP_DOUBLEPREC)res[i][j] + mul;
            FP_DOUBLEPREC reduced = FPRED_DOUBLE(sum);
            computed_res[i][j] = reduced;
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
                //printf("✅ Match at [%d][%d]: %u\n", i, j, computed_res[i][j]);
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
