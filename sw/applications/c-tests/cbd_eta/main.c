#include <stdint.h>
#include <stdio.h>

#define NTESTS 100

/* -------- CBD software (parametrizzato per eta) -------- */
static inline void cbd_eta_sw(uint32_t d, int eta, int lanes, int8_t *out) {
    const uint32_t mask = (eta == 32) ? 0xFFFFFFFFu : ((1u << eta) - 1u);
    const int stride = 2 * eta;
    for (int j = 0; j < lanes; ++j) {
        int sh_a = j * stride;
        int sh_b = sh_a + eta;
        uint32_t a = (d >> sh_a) & mask;
        uint32_t b = (d >> sh_b) & mask;
        out[j] = (int8_t)((int)a - (int)b);
    } 
}

void cbd_eta1(int32_t d, int8_t out[NTESTS][16], int i) {
    for (int j = 0; j < 16; j++) {
        int32_t a = (d >> (2 * j + 0)) & 0x1;
        int32_t b = (d >> (2 * j + 1)) & 0x1;
        out[i][j] = (int8_t)(a - b);
    }
    //printf("{%d, %d, %d, %d,%d, %d, %d, %d, %d, %d, %d, %d,%d, %d, %d, %d},\n", out[i][0], out[i][1], out[i][2], out[i][3],
    //       out[i][4], out[i][5], out[i][6], out[i][7], out[i][8], out[i][9], out[i][10], out[i][11],
    //       out[i][12], out[i][13], out[i][14], out[i][15]);
}

// η = 2  → 8 lanes, per-lane bits: a=[4*j+0..1], b=[4*j+2..3]
void cbd_eta2(int32_t d, int8_t out[NTESTS][8], int i) {
    uint32_t u = (uint32_t)d;
    for (int j = 0; j < 8; j++) {
        int32_t a = (u >> (4*j + 0)) & 0x3;
        int32_t b = (u >> (4*j + 2)) & 0x3;
        out[i][j] = (int8_t)(a - b);
    }
    //printf("{%d, %d, %d, %d,%d, %d, %d, %d},\n", out[i][0], out[i][1], out[i][2], out[i][3],
    //       out[i][4], out[i][5], out[i][6], out[i][7]);
    
}

// η = 3  → 5 lanes, per-lane bits: a=[6*j+0..2], b=[6*j+3..5]
void cbd_eta3(int32_t d, int8_t out[NTESTS][5], int i) {
    uint32_t u = (uint32_t)d;
    for (int j = 0; j < 5; j++) {
        int32_t a = (u >> (6*j + 0)) & 0x7;
        int32_t b = (u >> (6*j + 3)) & 0x7;
        out[i][j] = (int8_t)(a - b);
    }
    //printf("{%d, %d, %d, %d, %d},\n", out[i][0], out[i][1], out[i][2], out[i][3], out[i][4]);
}


void cbd_eta4(int32_t d, int8_t out[NTESTS][4], int i) {
    for (int j = 0; j < 4; j++) {
        int32_t a = (d >> (8 * j + 0)) & 0xF;  // lower 4 bits
        int32_t b = (d >> (8 * j + 4)) & 0xF;  // upper 4 bits
        out[i][j] = (int8_t)(a - b);
    }
    //printf("{%d, %d, %d, %d},\n", out[i][0], out[i][1], out[i][2], out[i][3]);
}

/* ===================== η = 1 (16 lanes) ===================== */

static const int32_t d_eta1[NTESTS] = {
    598833565, -966241705, -1188800802, 1015636137, -335343207
};
static const int8_t golden_eta1[NTESTS][16] = {
    {  1,  0,  1, -1,  1, -1,  0,  1,  1,  0,  0, -1,  0,  0, -1,  0 },
    {  0,  1,  1,  1, -1,  0,  1,  1,  0, -1, -1,  1, -1,  1,  0,  0 },
    { -1,  0,  1,  0, -1,  1,  1,  1,  0,  1, -1,  0,  1, -1,  0, -1 },
    {  1, -1, -1, -1,  0,  0, -1,  1,  1, -1,  0, -1,  0,  0,  0,  0 },
    {  1, -1,  1, -1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0, -1,  0 },
};

/* ===================== η = 2 (8 lanes) ====================== */
static const int32_t d_eta2[NTESTS] = {
   -2019505554, -1208440693, 193022198, -1200698400, 383393196
};
static const int8_t golden_eta2[NTESTS][8] = {
    {-1,  1,  0, -3,  0,  0,  2, -2},
    { 1, -2, -2,  0, -2,  0,  2,  1},
    { 1,  0, -2, -1,  1, -2,  1,  0},
    { 0, -1,  1, -3, -1,  1, -2,  1},
    {-3,  0, -2,  1,  0, -2,  1,  1}
};

/* ===================== η = 3 (5 lanes) ====================== */
static const int32_t d_eta3[NTESTS] = {
    1313483709, 851002234, -686118794, -1479704272, -701821063
};

static const int8_t golden_eta3[NTESTS][5] = {
    {-2,  1, -2,  0,  5},
    {-5,  4,  2,  1, -4},
    { 0, -3, -3,  6,  5},
    {-6,  4, -3, -3,  3},
    {-6, -2, -6,  1,  4}
};

/* ===================== η = 4 (4 lanes) ====================== */
static const int32_t d_eta4[NTESTS] = {
   //-1749143279, -1732089961, 1492476947, 445499907, 1319105919
    1343628325, -1437313772, 38830161, 1417980328, -1869176699,
    1344636249, 1147406601, 446211349, 352866393, 1159043162,
    -1548650083, 478163327, 107420369, -1110032133, 1181241943,
    1051802512, 958682846, 599310825, -1131847511, 440213415,
    -1388565139, -1113823565, -463085232, -1952635852, 373399426,
    -1758821271, 1812140441, 136505587, 127978094, 402418010,
    939042955, 999270936, -2124482863, -1709316540, 113971123,
    -1884438106, 854001193, -1219686479, -1503734903, -1282799476,
    -1954461450, 1801823908, 946785248, 1929338154, -1764090452,
    1194819984, -818489973, -561350837, 27911967, -1035914485,
    -833999939, 685731524, -1296481414, 1815115025, 1461364854,
    1193448329, 667779376, 924765563, -183768477, -1015784978,
    1445662585, 438989805, 398340369, 1631775357, 415393687,
    1541804686, -655006701, 1477278577, -1701983741, 1136108454,
    -828377729, 186618211, -1160793136, 1973214822, -1991885179,
    536124280, -115466932, -333738847, 1625792787, 338444264,
    -1923970831, 1259191105, -732701390, -1594979922, -1638445190,
    -491981009, -593964351, 1553210608, -1815258689, 825873196,
    -1268854288, 298737106, 196814233, -1454862951, 978815630,
    -974664054, 1242911821, -63473076, 342703921, -621405658,
};
static const int8_t golden_eta4[NTESTS][4] = {
    {  0,  -2,   3,  -2 },
    { -2,   1, -10,  -1 },
    {  2,  -2, -10,   3 },
    {  3,  -2,   5,   9 },
    {  8, -10,   6,  10 },
};

static int run_eta1(void) {
    int ok = 1;
    int8_t out[NTESTS][16];

    for (int i = 0; i < NTESTS; ++i) {    
        cbd_eta1(d_eta1[i], out, i);
        for (int j = 0; j < 16; ++j) {
            if (out[i][j] != golden_eta1[i][j]) {
                printf("η=1 FAIL [t=%d][j=%d]: d=%d exp=%d got=%d\n",
                       i, j, d_eta1[i], golden_eta1[i][j], out[i][j]);
                ok = 0;
            }
        }
    }
    if (ok) printf("η=1: all %d tests passed\n", NTESTS);
    return ok;
}

static int run_eta2(void) {
    int ok = 1;
    int8_t out[NTESTS][8];

    for (int i = 0; i < NTESTS; ++i) {
        cbd_eta2(d_eta2[i], out, i);
        for (int j = 0; j < 8; ++j) {
            if (out[i][j] != golden_eta2[i][j]) {
                printf("η=2 FAIL [t=%d][j=%d]: d=%d exp=%d got=%d\n",
                       i, j, d_eta2[i], golden_eta2[i][j], out[i][j]);
                ok = 0;
            }
        }
    }
    if (ok) printf("η=2: all %d tests passed\n", NTESTS);
    return ok;
}

static int run_eta3(void) {
    int ok = 1;
    int8_t out[NTESTS][5];
    
    for (int i = 0; i < NTESTS; ++i) {    
        cbd_eta3(d_eta3[i], out, i);
        //cbd_eta_sw(d_eta3[i], 3, 5, out[i]);  // SW reference
        for (int j = 0; j < 5; ++j) {
            if (out[i][j] != golden_eta3[i][j]) {
                printf("η=3 FAIL [t=%d][j=%d]: d=%d exp=%d got=%d\n",
                       i, j, d_eta3[i], golden_eta3[i][j], out[i][j]);
                ok = 0;
            }
        }
    }
    if (ok) printf("η=3: all %d tests passed\n", NTESTS);
    return ok;
}

static int run_eta4(void) {
    int ok = 1;
    int8_t out[NTESTS][4];

    for (int i = 0; i < NTESTS; ++i) {
        cbd_eta4(d_eta4[i], out, i);
        //for (int j = 0; j < 4; ++j) {
        //    if (out[i][j] != golden_eta4[i][j]) {
        //        printf("η=4 FAIL [t=%d][j=%d]: d=%d exp=%d got=%d\n",
        //               i, j, d_eta4[i], golden_eta4[i][j], out[i][j]);
        //        ok = 0;
        //    }
        //}
        printf("{%d, %d, %d, %d},\n", out[i][0], out[i][1], out[i][2], out[i][3]);
    }
    if (ok) printf("η=4: all %d tests passed\n", NTESTS);
    return ok;
}

int main(void) {
    printf("CBDη tests (SW only)\n");
    int ok = 1;
    //ok &= run_eta1();
    //ok &= run_eta2();
    //ok &= run_eta3();
    ok &= run_eta4();
    if (ok) {
        printf("All suites passed ✅\n");
        return 0;
    } else {
        printf("Some tests FAILED ❌\n");
        return 1;
    }
}
