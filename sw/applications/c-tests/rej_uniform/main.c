#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef PERF_CNT_CYCLES
  #include "core_v_mini_mcu.h"
  #include "csr.h"
#endif

/* ------------------------------------------------------------
 * Opcode set: each opcode implies a fixed (b, k) profile.
 * ------------------------------------------------------------ */
typedef enum {
  REJ_KYBER_12x2 = 0,     // b=12, k=2   -> lanes at [11:0], [23:12]
  REJ_DIL_23x1   = 1,     // b=23, k=1   -> lane  at [22:0]
  REJ_16x2       = 2,     // example extra profile: b=16, k=2
  REJ_8x4        = 3,     // example extra profile: b=8,  k=4
  REJ_DEFAULT    = 7      // safe default (same as KYBER_12x2)
} rej_insn_t;

typedef struct { uint8_t b, k; } rej_profile_t;

static inline rej_profile_t rej_get_profile(rej_insn_t op)
{
    switch (op) {
      case REJ_KYBER_12x2: return (rej_profile_t){12, 2};
      case REJ_DIL_23x1:   return (rej_profile_t){23, 1};
      case REJ_16x2:       return (rej_profile_t){16, 2};
      case REJ_8x4:        return (rej_profile_t){ 8, 4};
      default:             return (rej_profile_t){12, 2}; // safe default
    }
}

/* ------------------------------------------------------------
 * Software model (single 32-bit step, fixed-slice).
 * ------------------------------------------------------------ */
static inline uint32_t rej_fixed_step(uint32_t rs1, uint32_t m, uint32_t j, rej_insn_t op)
{
    rej_profile_t p = rej_get_profile(op);
    uint32_t b = p.b;
    uint32_t k = p.k;

    if (b == 0) b = 1;
    if (b > 32) b = 32;

    /* cap k so we never read beyond rs1[31:0] */
    if (b >= 32)      k = 1;
    else if (b >= 16) k = (k > 2) ? 2 : k;
    else if (b >= 11) k = (k > 2) ? 2 : k;
    else if (b >= 8)  k = (k > 4) ? 4 : k;

    const uint32_t mask_b = (b == 32) ? 0xFFFFFFFFu : ((1u << b) - 1u);
    const uint32_t m_eff  = m & mask_b;

    uint32_t rd = 0;
    const uint32_t base = j * b;

    for (uint32_t t = 0; t < k; ++t) {
        uint32_t sh = base + t * b;
        if (sh + b > 32) continue;                // would overrun current rs1
        uint32_t cand = (rs1 >> sh) & mask_b;
        uint32_t acc  = (cand < m_eff);           // accept?
        if (acc) {
            /* careful with shift width; use 64-bit temp to avoid UB at b=32 */
            uint64_t lane = ((uint64_t)cand) << (t * b);
            rd |= (uint32_t)lane;
        }
    }
    return rd;
}

/* --- no-inline wrapper to measure only the function body --- */
__attribute__((noinline))
static uint32_t rej_fixed_step_perf(uint32_t rs1, uint32_t m, uint32_t j, rej_insn_t op)
{
    /* prevent tail-call or LTO weirdness */
    asm volatile("" ::: "memory");
    uint32_t r = rej_fixed_step(rs1, m, j, op);
    asm volatile("" ::: "memory");
    return r;
}

/* =========================
 * Tiny tests (like your style)
 * ========================= */

/* --- REJ12x2 (Kyber, m=3329) --- */
#define NTESTS_R12 100
static int test_rej12x2(void)
{
    const uint32_t m = 3329u; // 0x0D01
    const rej_insn_t op = REJ_KYBER_12x2;

    static const uint32_t rs1_in[NTESTS_R12] = {
        0xA3B1799Du,
        0x1C80317Fu,
        0x06671AD1u,
        0xBDD640FBu,
        0x46685257u,
        0x3EB13B90u,
        0x392456DEu,
        0x23B8C1E9u,
        0xBC8960A9u,
        0x1A3D1FA7u,
        0xA36F2D1Eu,
        0x69C71C9Eu,
        0x3DAF7CCDu,
        0xE1F58E65u,
        0x7D6F0B29u,
        0x404A1E70u,
        0x55B2CCE6u,
        0x85A7F8C7u,
        0xE8A1F4B2u,
        0x1B8A64B2u,
        0xC9EAC2F9u,
        0x5E70B2B8u,
        0x7A1F9B1Du,
        0x7E9C640Bu,
        0x1FAD12F7u,
        0x5B8F0BBFu,
        0xB5D5B88Au,
        0xE9B3C9F8u,
        0x7FBAF03Du,
        0xA2E9B06Bu,
        0x7E9BD33Cu,
        0xC7CC8F93u,
        0x4F8F7D0Du,
        0x84E0C1E5u,
        0x6A7ED836u,
        0x92A6D6A1u,
        0x7A3E3D8Fu,
        0xB8B0B19Au,
        0x12F335C4u,
        0xC4B8C9E8u,
        0xCF8D4D6Bu,
        0xF7B7B8F8u,
        0xEC517B8Bu,
        0x486E88F1u,
        0xEE4D0D1Cu,
        0x8D4E779Fu,
        0x079A1A6Fu,
        0x3B3D850Fu,
        0x4B49A1D1u,
        0x8D0E3D3Fu,
        0x5C93CB59u,
        0xD9C33B9Cu,
        0x5AFA1E19u,
        0x1E8E019Cu,
        0x3B3C71B1u,
        0x2C2D63E7u,
        0x8A937C8Fu,
        0xF3AA6F54u,
        0x0B407953u,
        0xF0C8C5F1u,
        0x98688B0Cu,
        0x9E8B3A10u,
        0xB94F1B87u,
        0x92C6BC5Bu,
        0xB7B4E38Fu,
        0xCB9B08EBu,
        0x3F95F0C7u,
        0xE5B5A87Bu,
        0x37ABBBADu,
        0xA8CDE581u,
        0xD13F98F8u,
        0x2E779197u,
        0xF648E3EDu,
        0x9ABF3AF7u,
        0xDB631F63u,
        0x6E80E0B0u,
        0xE2C7533Fu,
        0x94E7A3F3u,
        0xC3D551D3u,
        0x3BD89C84u,
        0x8ED8A3C2u,
        0x2032C71Du,
        0x1C03A25Eu,
        0x8A0A9B1Cu,
        0x5E31E0D2u,
        0x6970C1C3u,
        0x58E9E484u,
        0xB5D93D2Bu,
        0x7F1A4A55u,
        0x4E4A7C64u,
        0x26C5C1D0u,
        0xA1BFAA36u,
        0xB6A1F06Bu,
        0xE63E7B4Du,
        0xCACE98C6u,
        0x160C4968u,
        0xDFBD1F3Eu,
        0x0B75D8D2u,
        0x9C53CEC3u,
        0xD58842DEu,
        0x5D65A441u,
        0x29A3B2E9u,
        0x1F3E4C5Du,
        0xFDDB21E9u,
        0xCB6A3C2Fu,
        0xDEDBB7C6u,
        0x0B0C0D0Eu,
        0x1F2F3F4Fu,
        0xABCDEF01u
    };
        
    static const uint32_t golden[NTESTS_R12] = {
        0x00B1799Du,
        0x0080317Fu,
        0x00671AD1u,
        0x000000FBu,
        0x00685257u,
        0x00B13B90u,
        0x002456DEu,
        0x00B8C1E9u,
        0x008960A9u,
        0x003D1000u,
        0x006F2000u,
        0x00C71C9Eu,
        0x00AF7CCDu,
        0x00000000u,
        0x006F0B29u,
        0x004A1000u,
        0x00B2CCE6u,
        0x00A7F8C7u,
        0x00A1F4B2u,
        0x008A64B2u,
        0x000002F9u,
        0x0070B2B8u,
        0x001F9B1Du,
        0x009C640Bu,
        0x00AD12F7u,
        0x008F0BBFu,
        0x0000088Au,
        0x00B3C9F8u,
        0x00BAF03Du,
        0x0000006Bu,
        0x009BD33Cu,
        0x00CC8000u,
        0x008F7000u,
        0x000001E5u,
        0x007ED836u,
        0x00A6D6A1u,
        0x003E3000u,
        0x00B0B19Au,
        0x000005C4u,
        0x00B8C9E8u,
        0x008D4000u,
        0x00B7B8F8u,
        0x00517B8Bu,
        0x006E88F1u,
        0x004D0000u,
        0x004E779Fu,
        0x009A1A6Fu,
        0x003D850Fu,
        0x0049A1D1u,
        0x000E3000u,
        0x0093CB59u,
        0x00C33B9Cu,
        0x00000000u,
        0x008E019Cu,
        0x003C71B1u,
        0x002D63E7u,
        0x00937C8Fu,
        0x00AA6000u,
        0x00407953u,
        0x00C8C5F1u,
        0x00688B0Cu,
        0x008B3A10u,
        0x004F1B87u,
        0x00C6BC5Bu,
        0x00B4E38Fu,
        0x009B08EBu,
        0x0095F0C7u,
        0x00B5A87Bu,
        0x00ABBBADu,
        0x00CDE581u,
        0x003F98F8u,
        0x00779197u,
        0x0048E3EDu,
        0x00BF3AF7u,
        0x00631000u,
        0x0080E0B0u,
        0x00C7533Fu,
        0x000003F3u,
        0x000001D3u,
        0x00000C84u,
        0x000003C2u,
        0x0032C71Du,
        0x0003A25Eu,
        0x000A9B1Cu,
        0x0031E0D2u,
        0x0070C1C3u,
        0x00000484u,
        0x00000000u,
        0x001A4A55u,
        0x004A7C64u,
        0x00C5C1D0u,
        0x00BFAA36u,
        0x00A1F06Bu,
        0x003E7B4Du,
        0x00CE98C6u,
        0x000C4968u,
        0x00BD1000u,
        0x0075D8D2u,
        0x0053C000u,
        0x008842DEu
    };

    uint64_t cycles_sum = 0;
    int pass = 1;
    for (int i = 0; i < NTESTS_R12; ++i) {
        unsigned cycles;

        uint32_t got = rej_fixed_step_perf(rs1_in[i], m, /*j=*/0, op);

        if (got != golden[i]) {
            printf("REJ12x2 FAIL [%d]: rs1=0x%08X exp=0x%08X got=0x%08X\n",
                   i, rs1_in[i], golden[i], got);
            pass = 0;
        }
        //printf("0x%08Xu,\n", got);
    }

    if (pass) printf("REJ12x2: All %d tests passed!\n", NTESTS_R12);
    return pass;
}

/* --- REJ23x1 (Dilithium, m=8380417) --- */
#define NTESTS_R23 100
static int test_rej23x1(void)
{
    const uint32_t m = 8380417u; // 0x007FE001
    const rej_insn_t op = REJ_DIL_23x1;

    static const uint32_t rs1_in[NTESTS_R23] = {
        0xA3B1799Du,
        0x1C80317Fu,
        0x06671AD1u,
        0xBDD640FBu,
        0x46685257u,
        0x3EB13B90u,
        0x392456DEu,
        0x23B8C1E9u,
        0xBC8960A9u,
        0x1A3D1FA7u,
        0xA36F2D1Eu,
        0x69C71C9Eu,
        0x3DAF7CCDu,
        0xE1F58E65u,
        0x7D6F0B29u,
        0x404A1E70u,
        0x55B2CCE6u,
        0x85A7F8C7u,
        0xE8A1F4B2u,
        0x1B8A64B2u,
        0xC9EAC2F9u,
        0x5E70B2B8u,
        0x7A1F9B1Du,
        0x7E9C640Bu,
        0x1FAD12F7u,
        0x5B8F0BBFu,
        0xB5D5B88Au,
        0xE9B3C9F8u,
        0x7FBAF03Du,
        0xA2E9B06Bu,
        0x7E9BD33Cu,
        0xC7CC8F93u,
        0x4F8F7D0Du,
        0x84E0C1E5u,
        0x6A7ED836u,
        0x92A6D6A1u,
        0x7A3E3D8Fu,
        0xB8B0B19Au,
        0x12F335C4u,
        0xC4B8C9E8u,
        0xCF8D4D6Bu,
        0xF7B7B8F8u,
        0xEC517B8Bu,
        0x486E88F1u,
        0xEE4D0D1Cu,
        0x8D4E779Fu,
        0x079A1A6Fu,
        0x3B3D850Fu,
        0x4B49A1D1u,
        0x8D0E3D3Fu,
        0x5C93CB59u,
        0xD9C33B9Cu,
        0x5AFA1E19u,
        0x1E8E019Cu,
        0x3B3C71B1u,
        0x2C2D63E7u,
        0x8A937C8Fu,
        0xF3AA6F54u,
        0x0B407953u,
        0xF0C8C5F1u,
        0x98688B0Cu,
        0x9E8B3A10u,
        0xB94F1B87u,
        0x92C6BC5Bu,
        0xB7B4E38Fu,
        0xCB9B08EBu,
        0x3F95F0C7u,
        0xE5B5A87Bu,
        0x37ABBBADu,
        0xA8CDE581u,
        0xD13F98F8u,
        0x2E779197u,
        0xF648E3EDu,
        0x9ABF3AF7u,
        0xDB631F63u,
        0x6E80E0B0u,
        0xE2C7533Fu,
        0x94E7A3F3u,
        0xC3D551D3u,
        0x3BD89C84u,
        0x8ED8A3C2u,
        0x2032C71Du,
        0x1C03A25Eu,
        0x8A0A9B1Cu,
        0x5E31E0D2u,
        0x6970C1C3u,
        0x58E9E484u,
        0xB5D93D2Bu,
        0x7F1A4A55u,
        0x4E4A7C64u,
        0x26C5C1D0u,
        0xA1BFAA36u,
        0xB6A1F06Bu,
        0xE63E7B4Du,
        0xCACE98C6u,
        0x160C4968u,
        0xDFBD1F3Eu,
        0x0B75D8D2u,
        0x9C53CEC3u,
        0xD58842DEu,
        0x5D65A441u,
        0x29A3B2E9u,
        0x1F3E4C5Du,
        0xFDDB21E9u,
        0xCB6A3C2Fu,
        0xDEDBB7C6u,
        0x0B0C0D0Eu,
        0x1F2F3F4Fu,
        0xABCDEF01u
        };
        
    static const uint32_t golden[NTESTS_R23] = {
        0x0031799Du,
        0x0000317Fu,
        0x00671AD1u,
        0x005640FBu,
        0x00685257u,
        0x00313B90u,
        0x002456DEu,
        0x0038C1E9u,
        0x000960A9u,
        0x003D1FA7u,
        0x006F2D1Eu,
        0x00471C9Eu,
        0x002F7CCDu,
        0x00758E65u,
        0x006F0B29u,
        0x004A1E70u,
        0x0032CCE6u,
        0x0027F8C7u,
        0x0021F4B2u,
        0x000A64B2u,
        0x006AC2F9u,
        0x0070B2B8u,
        0x001F9B1Du,
        0x001C640Bu,
        0x002D12F7u,
        0x000F0BBFu,
        0x0055B88Au,
        0x0033C9F8u,
        0x003AF03Du,
        0x0069B06Bu,
        0x001BD33Cu,
        0x004C8F93u,
        0x000F7D0Du,
        0x0060C1E5u,
        0x007ED836u,
        0x0026D6A1u,
        0x003E3D8Fu,
        0x0030B19Au,
        0x007335C4u,
        0x0038C9E8u,
        0x000D4D6Bu,
        0x0037B8F8u,
        0x00517B8Bu,
        0x006E88F1u,
        0x004D0D1Cu,
        0x004E779Fu,
        0x001A1A6Fu,
        0x003D850Fu,
        0x0049A1D1u,
        0x000E3D3Fu,
        0x0013CB59u,
        0x00433B9Cu,
        0x007A1E19u,
        0x000E019Cu,
        0x003C71B1u,
        0x002D63E7u,
        0x00137C8Fu,
        0x002A6F54u,
        0x00407953u,
        0x0048C5F1u,
        0x00688B0Cu,
        0x000B3A10u,
        0x004F1B87u,
        0x0046BC5Bu,
        0x0034E38Fu,
        0x001B08EBu,
        0x0015F0C7u,
        0x0035A87Bu,
        0x002BBBADu,
        0x004DE581u,
        0x003F98F8u,
        0x00779197u,
        0x0048E3EDu,
        0x003F3AF7u,
        0x00631F63u,
        0x0000E0B0u,
        0x0047533Fu,
        0x0067A3F3u,
        0x005551D3u,
        0x00589C84u,
        0x0058A3C2u,
        0x0032C71Du,
        0x0003A25Eu,
        0x000A9B1Cu,
        0x0031E0D2u,
        0x0070C1C3u,
        0x0069E484u,
        0x00593D2Bu,
        0x001A4A55u,
        0x004A7C64u,
        0x0045C1D0u,
        0x003FAA36u,
        0x0021F06Bu,
        0x003E7B4Du,
        0x004E98C6u,
        0x000C4968u,
        0x003D1F3Eu,
        0x0075D8D2u,
        0x0053CEC3u,
        0x000842DEu
    };

    uint64_t cycles_sum = 0;
    int pass = 1;
    for (int i = 0; i < NTESTS_R23; ++i) {
        unsigned cycles;

        uint32_t got = rej_fixed_step_perf(rs1_in[i], m, /*j=*/0, op);

        if (got != golden[i]) {
            printf("REJ23x1 FAIL [%d]: rs1=0x%08X exp=0x%08X got=0x%08X\n",
                   i, rs1_in[i], golden[i], got);
            pass = 0;
        }
        //printf("0x%08Xu,\n", got);
    }

    if (pass) printf("REJ23x1: All %d tests passed!\n", NTESTS_R23);
    return pass;
}

/* --- REJ8x4 (b=8, k=4), LSB-first packing --- */
#define NTESTS_R8 100
static int test_rej8x4(void) {
  const uint32_t m  = 200u;           // 0xC8
  const rej_insn_t op = REJ_8x4;

    static const uint32_t in[NTESTS_R8] = {
    0xA3B1799Du,
    0x1C80317Fu,
    0x06671AD1u,
    0xBDD640FBu,
    0x46685257u,
    0x3EB13B90u,
    0x392456DEu,
    0x23B8C1E9u,
    0xBC8960A9u,
    0x1A3D1FA7u,
    0xA36F2D1Eu,
    0x69C71C9Eu,
    0x3DAF7CCDu,
    0xE1F58E65u,
    0x7D6F0B29u,
    0x404A1E70u,
    0x55B2CCE6u,
    0x85A7F8C7u,
    0xE8A1F4B2u,
    0x1B8A64B2u,
    0xC9EAC2F9u,
    0x5E70B2B8u,
    0x7A1F9B1Du,
    0x7E9C640Bu,
    0x1FAD12F7u,
    0x5B8F0BBFu,
    0xB5D5B88Au,
    0xE9B3C9F8u,
    0x7FBAF03Du,
    0xA2E9B06Bu,
    0x7E9BD33Cu,
    0xC7CC8F93u,
    0x4F8F7D0Du,
    0x84E0C1E5u,
    0x6A7ED836u,
    0x92A6D6A1u,
    0x7A3E3D8Fu,
    0xB8B0B19Au,
    0x12F335C4u,
    0xC4B8C9E8u,
    0xCF8D4D6Bu,
    0xF7B7B8F8u,
    0xEC517B8Bu,
    0x486E88F1u,
    0xEE4D0D1Cu,
    0x8D4E779Fu,
    0x079A1A6Fu,
    0x3B3D850Fu,
    0x4B49A1D1u,
    0x8D0E3D3Fu,
    0x5C93CB59u,
    0xD9C33B9Cu,
    0x5AFA1E19u,
    0x1E8E019Cu,
    0x3B3C71B1u,
    0x2C2D63E7u,
    0x8A937C8Fu,
    0xF3AA6F54u,
    0x0B407953u,
    0xF0C8C5F1u,
    0x98688B0Cu,
    0x9E8B3A10u,
    0xB94F1B87u,
    0x92C6BC5Bu,
    0xB7B4E38Fu,
    0xCB9B08EBu,
    0x3F95F0C7u,
    0xE5B5A87Bu,
    0x37ABBBADu,
    0xA8CDE581u,
    0xD13F98F8u,
    0x2E779197u,
    0xF648E3EDu,
    0x9ABF3AF7u,
    0xDB631F63u,
    0x6E80E0B0u,
    0xE2C7533Fu,
    0x94E7A3F3u,
    0xC3D551D3u,
    0x3BD89C84u,
    0x8ED8A3C2u,
    0x2032C71Du,
    0x1C03A25Eu,
    0x8A0A9B1Cu,
    0x5E31E0D2u,
    0x6970C1C3u,
    0x58E9E484u,
    0xB5D93D2Bu,
    0x7F1A4A55u,
    0x4E4A7C64u,
    0x26C5C1D0u,
    0xA1BFAA36u,
    0xB6A1F06Bu,
    0xE63E7B4Du,
    0xCACE98C6u,
    0x160C4968u,
    0xDFBD1F3Eu,
    0x0B75D8D2u,
    0x9C53CEC3u,
    0xD58842DEu,
    0x5D65A441u,
    0x29A3B2E9u,
    0x1F3E4C5Du,
    0xFDDB21E9u,
    0xCB6A3C2Fu,
    0xDEDBB7C6u,
    0x0B0C0D0Eu,
    0x1F2F3F4Fu,
    0xABCDEF01u
    };

  static const uint32_t gold[NTESTS_R8] = {
    0xA3B1799Du,
    0x1C80317Fu,
    0x06671A00u,
    0xBD004000u,
    0x46685257u,
    0x3EB13B90u,
    0x39245600u,
    0x23B8C100u,
    0xBC8960A9u,
    0x1A3D1FA7u,
    0xA36F2D1Eu,
    0x69C71C9Eu,
    0x3DAF7C00u,
    0x00008E65u,
    0x7D6F0B29u,
    0x404A1E70u,
    0x55B20000u,
    0x85A700C7u,
    0x00A100B2u,
    0x1B8A64B2u,
    0x0000C200u,
    0x5E70B2B8u,
    0x7A1F9B1Du,
    0x7E9C640Bu,
    0x1FAD1200u,
    0x5B8F0BBFu,
    0xB500B88Au,
    0x00B30000u,
    0x7FBA003Du,
    0xA200B06Bu,
    0x7E9B003Cu,
    0xC7008F93u,
    0x4F8F7D0Du,
    0x8400C100u,
    0x6A7E0036u,
    0x92A600A1u,
    0x7A3E3D8Fu,
    0xB8B0B19Au,
    0x120035C4u,
    0xC4B80000u,
    0x008D4D6Bu,
    0x00B7B800u,
    0x00517B8Bu,
    0x486E8800u,
    0x004D0D1Cu,
    0x8D4E779Fu,
    0x079A1A6Fu,
    0x3B3D850Fu,
    0x4B49A100u,
    0x8D0E3D3Fu,
    0x5C930059u,
    0x00C33B9Cu,
    0x5A001E19u,
    0x1E8E019Cu,
    0x3B3C71B1u,
    0x2C2D6300u,
    0x8A937C8Fu,
    0x00AA6F54u,
    0x0B407953u,
    0x0000C500u,
    0x98688B0Cu,
    0x9E8B3A10u,
    0xB94F1B87u,
    0x92C6BC5Bu,
    0xB7B4008Fu,
    0x009B0800u,
    0x3F9500C7u,
    0x00B5A87Bu,
    0x37ABBBADu,
    0xA8000081u,
    0x003F9800u,
    0x2E779197u,
    0x00480000u,
    0x9ABF3A00u,
    0x00631F63u,
    0x6E8000B0u,
    0x00C7533Fu,
    0x9400A300u,
    0xC3005100u,
    0x3B009C84u,
    0x8E00A3C2u,
    0x2032C71Du,
    0x1C03A25Eu,
    0x8A0A9B1Cu,
    0x5E310000u,
    0x6970C1C3u,
    0x58000084u,
    0xB5003D2Bu,
    0x7F1A4A55u,
    0x4E4A7C64u,
    0x26C5C100u,
    0xA1BFAA36u,
    0xB6A1006Bu,
    0x003E7B4Du,
    0x000098C6u,
    0x160C4968u,
    0x00BD1F3Eu,
    0x0B750000u,
    0x9C5300C3u,
    0x00884200u
    };

  uint64_t cycles_sum = 0;
  int ok = 1;
  for (int i = 0; i < NTESTS_R8; ++i) {
        unsigned cycles;

        #if PERF_CNT_CYCLES == 1
            CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
        uint32_t got = rej_fixed_step_perf(in[i], m, /*j=*/0, op);
        #if PERF_CNT_CYCLES == 1
            CSR_READ(CSR_REG_MCYCLE, &cycles);
            printf("test_rej8x4: %d\n", cycles);
        #endif
    

    if (got != gold[i]) {
    printf("REJ8x4 FAIL [%d]: rs1=0x%08X exp=0x%08X got=0x%08X\n",  i, in[i], gold[i], got);
    ok = 0;
    }
    //printf("0x%08Xu,\n", got);
    
  }

  if (ok) printf("REJ8x4: All %d tests passed!\n", NTESTS_R8);
  return ok;
}

int main(void)
{
    #if PERF_CNT_CYCLES == 1
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif

    int ok = 1;
    ok &= test_rej12x2();
    ok &= test_rej23x1();
    ok &= test_rej8x4();

    printf("\nSummary: %s\n", ok ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
    return ok ? 0 : 1;
}
