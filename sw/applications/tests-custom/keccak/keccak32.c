#include <stdint.h>
#include <string.h>
#include "keccak32.h"

#define NROUNDS 24

static const uint32_t KeccakF1600RoundConstants32[2 * 24] = {
    0x00000000, 0x00000001,
    0x00000000, 0x00008082,
    0x80000000, 0x0000808a,
    0x80000000, 0x80008000,
    0x00000000, 0x0000808b,
    0x00000000, 0x80000001,
    0x80000000, 0x80008081,
    0x80000000, 0x00008009,
    0x00000000, 0x0000008a,
    0x00000000, 0x00000088,
    0x00000000, 0x80008009,
    0x00000000, 0x8000000a,
    0x00000000, 0x8000808b,
    0x80000000, 0x0000008b,
    0x80000000, 0x00008089,
    0x80000000, 0x00008003,
    0x80000000, 0x00008002,
    0x80000000, 0x00000080,
    0x00000000, 0x0000800a,
    0x80000000, 0x8000000a,
    0x80000000, 0x80008081,
    0x80000000, 0x00008080,
    0x00000000, 0x80000001,
    0x80000000, 0x80008008
};

void Keccak32_bcop(uint32_t *state){

    uint32_t Aba0, Abe0, Abi0, Abo0, Abu0;
    uint32_t Aba1, Abe1, Abi1, Abo1, Abu1;
    uint32_t Aga0, Age0, Agi0, Ago0, Agu0;
    uint32_t Aga1, Age1, Agi1, Ago1, Agu1;
    uint32_t Aka0, Ake0, Aki0, Ako0, Aku0;
    uint32_t Aka1, Ake1, Aki1, Ako1, Aku1;
    uint32_t Ama0, Ame0, Ami0, Amo0, Amu0;
    uint32_t Ama1, Ame1, Ami1, Amo1, Amu1;
    uint32_t Asa0, Ase0, Asi0, Aso0, Asu0;
    uint32_t Asa1, Ase1, Asi1, Aso1, Asu1;
    uint32_t BCa0, BCe0, BCi0, BCo0, BCu0;
    uint32_t BCa1, BCe1, BCi1, BCo1, BCu1;
    uint32_t Da0, De0, Di0, Do0, Du0;
    uint32_t Da1, De1, Di1, Do1, Du1;
    uint32_t Eba0, Ebe0, Ebi0, Ebo0, Ebu0;
    uint32_t Eba1, Ebe1, Ebi1, Ebo1, Ebu1;
    uint32_t Ega0, Ege0, Egi0, Ego0, Egu0;
    uint32_t Ega1, Ege1, Egi1, Ego1, Egu1;
    uint32_t Eka0, Eke0, Eki0, Eko0, Eku0;
    uint32_t Eka1, Eke1, Eki1, Eko1, Eku1;
    uint32_t Ema0, Eme0, Emi0, Emo0, Emu0;
    uint32_t Ema1, Eme1, Emi1, Emo1, Emu1;
    uint32_t Esa0, Ese0, Esi0, Eso0, Esu0;
    uint32_t Esa1, Ese1, Esi1, Eso1, Esu1;

    //copyFromState(A, state)
        Aba0 = state[ 0];
        Aba1 = state[ 1];
        Abe0 = state[ 2];
        Abe1 = state[ 3];
        Abi0 = state[ 4];
        Abi1 = state[ 5];
        Abo0 = state[ 6];
        Abo1 = state[ 7];
        Abu0 = state[ 8];
        Abu1 = state[ 9];
        Aga0 = state[10];
        Aga1 = state[11];
        Age0 = state[12];
        Age1 = state[13];
        Agi0 = state[14];
        Agi1 = state[15];
        Ago0 = state[16];
        Ago1 = state[17];
        Agu0 = state[18];
        Agu1 = state[19];
        Aka0 = state[20];
        Aka1 = state[21];
        Ake0 = state[22];
        Ake1 = state[23];
        Aki0 = state[24];
        Aki1 = state[25];
        Ako0 = state[26];
        Ako1 = state[27];
        Aku0 = state[28];
        Aku1 = state[29];
        Ama0 = state[30];
        Ama1 = state[31];
        Ame0 = state[32];
        Ame1 = state[33];
        Ami0 = state[34];
        Ami1 = state[35];
        Amo0 = state[36];
        Amo1 = state[37];
        Amu0 = state[38];
        Amu1 = state[39];
        Asa0 = state[40];
        Asa1 = state[41];
        Ase0 = state[42];
        Ase1 = state[43];
        Asi0 = state[44];
        Asi1 = state[45];
        Aso0 = state[46];
        Aso1 = state[47];
        Asu0 = state[48];
        Asu1 = state[49];

        uint32_t Da0a, Da0b;
        uint32_t De0a, De0b;
        uint32_t Di0a, Di0b;
        uint32_t Do0a, Do0b;
        uint32_t Du0a, Du0b;
        uint32_t BCa0a, BCa0b;
        uint32_t BCe0a, BCe0b;
        uint32_t BCi0a, BCi0b;
        uint32_t BCo0a, BCo0b;
        uint32_t BCu0a, BCu0b;


        for(int round = 0; round < 24; round += 2 )
        {
            //    prepareTheta
            BCa0 = Aba0^Aga0^Aka0^Ama0^Asa0;
            BCa1 = Aba1^Aga1^Aka1^Ama1^Asa1;
            
            BCe0 = Abe0^Age0^Ake0^Ame0^Ase0;
            BCe1 = Abe1^Age1^Ake1^Ame1^Ase1;

            BCi0 = Abi0^Agi0^Aki0^Ami0^Asi0;
            BCi1 = Abi1^Agi1^Aki1^Ami1^Asi1;

            BCo0 = Abo0^Ago0^Ako0^Amo0^Aso0;
            BCo1 = Abo1^Ago1^Ako1^Amo1^Aso1;

            BCu0 = Abu0^Agu0^Aku0^Amu0^Asu0;
            BCu1 = Abu1^Agu1^Aku1^Amu1^Asu1;

            //printf("BCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n\n", BCu1, BCu0);

            //thetaRhoPiChiIota(round  , A, E)
            rol32_asm(BCe1, BCe0, 1, &Da1, &Da0);
            Da0 ^= BCu0;
            Da1 ^= BCu1;

            rol32_asm(BCi1, BCi0, 1, &De1, &De0);
            De0 ^= BCa0;
            De1 ^= BCa1;

            rol32_asm(BCo1, BCo0, 1, &Di1, &Di0);
            Di0 ^= BCe0;
            Di1 ^= BCe1;

            rol32_asm(BCu1, BCu0, 1, &Do1, &Do0);
            Do0 ^= BCi0;
            Do1 ^= BCi1;

            rol32_asm(BCa1, BCa0, 1, &Du1, &Du0);
            Du0 ^= BCo0;
            Du1 ^= BCo1;

            //printf("Da: %04X-%04X\n", Da1, Da0);
            //printf("De: %04X-%04X\n", De1, De0);
            //printf("Di: %04X-%04X\n", Di1, Di0);
            //printf("Do: %04X-%04X\n", Do1, Do0);
            //printf("Du: %04X-%04X\n", Du1, Du0);

            BCa0 = Aba0 ^ Da0;
            BCa1 = Aba1 ^ Da1;

            Age0 ^= De0;
            Age1 ^= De1;
            rol32_asm(Age1, Age0, 44, &BCe1, &BCe0);

            Aki1 ^= Di1;
            Aki0 ^= Di0;
            rol32_asm(Aki1, Aki0, 43, &BCi1, &BCi0);

            Amo1 ^= Do1;
            Amo0 ^= Do0;
            rol32_asm(Amo1, Amo0, 21, &BCo1, &BCo0);

            Asu0 ^= Du0;
            Asu1 ^= Du1;
            rol32_asm(Asu1, Asu0, 14, &BCu1, &BCu0);

            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);


            //Eba0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Eba1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Eba0);
            bcop32_asm(BCa1, BCe1, BCi1, &Eba1);

            Eba0 ^= KeccakF1600RoundConstants32[round*2+1];
            Eba1 ^= KeccakF1600RoundConstants32[round*2+0];

            //Ebe0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ebe1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Ebe0);
            bcop32_asm(BCe1, BCi1, BCo1, &Ebe1);

            //Ebi0 =   BCi0 ^((~BCo0)&  BCu0 );
            //Ebi1 =   BCi1 ^((~BCo1)&  BCu1 );
            bcop32_asm(BCi0, BCo0, BCu0, &Ebi0);
            bcop32_asm(BCi1, BCo1, BCu1, &Ebi1);

            //Ebo0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Ebo1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Ebo0);
            asm volatile (
                "mv a0, %[rs1]\n\t"
                "mv a1, %[rs2]\n\t"
                "mv a2, %[rs3]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rd1], a0, a1, a2\n\t"
                : [rd1] "=r" (Ebo1)
                : [rs1] "r" (BCo1), [rs2] "r" (BCu1), [rs3] "r" (BCa1)
                : "a0","a1","a2","cc");
            //bcop32_asm(BCo1, BCu1, BCa1, &Ebo1);

            //Ebu0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Ebu1 =   BCu1 ^((~BCa1)&  BCe1 );
            bcop32_asm(BCu0, BCa0, BCe0, &Ebu0);
            bcop32_asm(BCu1, BCa1, BCe1, &Ebu1);
            //bcop32_2_asm(BCu0, BCa0, BCe0, &Ebu0, BCu1, BCa1, BCe1, &Ebu1);

            //printf("Ebe0: %04X-%04X\n", Ebe1, Ebe0);
            //printf("Ebi0: %04X-%04X\n", Ebi1, Ebi0);
            //printf("Ebo0: %04X-%04X\n", Ebo1, Ebo0);
            //printf("Ebu0: %04X-%04X\n", Ebu1, Ebu0);
       
            Abo0 ^= Do0;
            Abo1 ^= Do1;
            rol32_asm(Abo1, Abo0, 28, &BCa1, &BCa0);

            Agu0 ^= Du0;
            Agu1 ^= Du1;
            rol32_asm(Agu1, Agu0, 20, &BCe1, &BCe0);

            Aka1 ^= Da1;
            Aka0 ^= Da0;
            rol32_asm(Aka1, Aka0, 3, &BCi1, &BCi0);

            Ame1 ^= De1;
            Ame0 ^= De0;
            rol32_asm(Ame1, Ame0, 45, &BCo1, &BCo0);

            Asi1 ^= Di1;
            Asi0 ^= Di0;
            rol32_asm(Asi1, Asi0, 61, &BCu1, &BCu0);

            //printf("\n\nAbo: %04X-%04X\n", Abo1, Abo0);
            //printf("Agu: %04X-%04X\n", Agu1, Agu0);
            //printf("Aka: %04X-%04X\n", Aka1, Aka0);
            //printf("Ame: %04X-%04X\n", Ame1, Ame0);
            //printf("Asi: %04X-%04X\n", Asi1, Asi0);
            //printf("\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            //Ega0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Ega1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Ega0);
            bcop32_asm(BCa1, BCe1, BCi1, &Ega1);

            //Ege0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ege1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Ege0);
            bcop32_asm(BCe1, BCi1, BCo1, &Ege1);           
            
            //Egi0 =   BCi0 ^((~BCo0)&  BCu0 );
            //Egi1 =   BCi1 ^((~BCo1)&  BCu1 );
            bcop32_asm(BCi0, BCo0, BCu0, &Egi0);
            bcop32_asm(BCi1, BCo1, BCu1, &Egi1);            
            
            //Ego0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Ego1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Ego0);
            bcop32_asm(BCo1, BCu1, BCa1, &Ego1);      

            //Egu0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Egu1 =   BCu1 ^((~BCa1)&  BCe1 );
            bcop32_asm(BCu0, BCa0, BCe0, &Egu0);
            bcop32_asm(BCu1, BCa1, BCe1, &Egu1);      

           //printf("\n\nEga: %04X-%04X\n", Ega1, Ega0);
           //printf("Ege: %04X-%04X\n", Ege1, Ege0);
           //printf("Egi: %04X-%04X\n", Egi1, Egi0);
           //printf("Ego: %04X-%04X\n", Ego1, Ego0);
           //printf("Egu: %04X-%04X\n", Egu1, Egu0);

            Abe1 ^= De1;
            Abe0 ^= De0;
            rol32_asm(Abe1, Abe0, 1, &BCa1, &BCa0);

            Agi0 ^= Di0;
            Agi1 ^= Di1;
            rol32_asm(Agi1, Agi0, 6, &BCe1, &BCe0);

            Ako1 ^= Do1;
            Ako0 ^= Do0;
            rol32_asm(Ako1, Ako0, 25, &BCi1, &BCi0);

            Amu0 ^= Du0;
            Amu1 ^= Du1;
            rol32_asm(Amu1, Amu0, 8, &BCo1, &BCo0);

            Asa0 ^= Da0;
            Asa1 ^= Da1;
            rol32_asm(Asa1, Asa0, 18, &BCu1, &BCu0);

           //printf("\n\nBCa: %04X-%04X\n", BCa1, BCa0);
           //printf("BCe: %04X-%04X\n", BCe1, BCe0);
           //printf("BCi: %04X-%04X\n", BCi1, BCi0);
           //printf("BCo: %04X-%04X\n", BCo1, BCo0);
           //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            //Eka0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Eka1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Eka0);
            bcop32_asm(BCa1, BCe1, BCi1, &Eka1);      

            //Eke0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Eke1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Eke0);
            bcop32_asm(BCe1, BCi1, BCo1, &Eke1);      

            Eki0 =   BCi0 ^((~BCo0)&  BCu0 );
            Eki1 =   BCi1 ^((~BCo1)&  BCu1 );
            //bcop32_asm(BCi0, BCo0, BCu0, &Eki0);
            //asm volatile (
            //    "mv a0, %[rs1]\n\t"
            //    "mv a1, %[rs2]\n\t"
            //    "mv a2, %[rs3]\n\t"
            //    ".insn r 0x6b, 0x01, 0x2, %[rd1], a0, a1, a2\n\t"
            //    : [rd1] "=r" (Eki0)
            //    : [rs1] "r" (BCi0), [rs2] "r" (BCo0), [rs3] "r" (BCu0)
            //    : "a0","a1","a2","cc");
            //asm volatile (
            //    "mv a3, %[rs1]\n\t"
            //    "mv a4, %[rs2]\n\t"
            //    "mv a5, %[rs3]\n\t"
            //    ".insn r 0x6b, 0x01, 0x2, %[rd1], a3, a4, a5\n\t"
            //    : [rd1] "=r" (Eki1)
            //    : [rs1] "r" (BCi1), [rs2] "r" (BCo1), [rs3] "r" (BCu1)
            //    : "a3","a4","a5","cc");
            //bcop32_asm(BCi1, BCo1, BCu1, &Eki1);      

            //Eko0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Eko1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Eko0);
            bcop32_asm(BCo1, BCu1, BCa1, &Eko1);      

            //Eku0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Eku1 =   BCu1 ^((~BCa1)&  BCe1 );
            bcop32_asm(BCu0, BCa0, BCe0, &Eku0);
            bcop32_asm(BCu1, BCa1, BCe1, &Eku1);      

            //printf("Eka: %04X-%04X\n", Eka1, Eka0);
            //printf("Eke: %04X-%04X\n", Eke1, Eke0);
            //printf("Eki: %04X-%04X\n", Eki1, Eki0);
            //printf("Eko: %04X-%04X\n", Eko1, Eko0);
            //printf("Eku: %04X-%04X\n", Eku1, Eku0);
       
            Abu1 ^= Du1;
            Abu0 ^= Du0;
            rol32_asm(Abu1, Abu0, 27, &BCa1, &BCa0);


            Aga0 ^= Da0;
            Aga1 ^= Da1;
            rol32_asm(Aga1, Aga0, 36, &BCe1, &BCe0);

            Ake0 ^= De0;
            Ake1 ^= De1;
            rol32_asm(Ake1, Ake0, 10, &BCi1, &BCi0);

            Ami1 ^= Di1;
            Ami0 ^= Di0;
            rol32_asm(Ami1, Ami0, 15, &BCo1, &BCo0);

            Aso0 ^= Do0;
            Aso1 ^= Do1;
            rol32_asm(Aso1, Aso0, 56, &BCu1, &BCu0);

            //printf("\n\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            Ema0 =   BCa0 ^((~BCe0)&  BCi0 );
            Ema1 =   BCa1 ^((~BCe1)&  BCi1 );
            //bcop32_asm(BCa0, BCe0, BCi0, &Ema0);
            //bcop32_asm(BCa1, BCe1, BCi1, &Ema1); 

            Eme0 =   BCe0 ^((~BCi0)&  BCo0 );
            Eme1 =   BCe1 ^((~BCi1)&  BCo1 );
            //bcop32_asm(BCe0, BCi0, BCo0, &Eme0);
            //bcop32_asm(BCe1, BCi1, BCo1, &Eme1); 

            Emi0 =   BCi0 ^((~BCo0)&  BCu0 );
            Emi1 =   BCi1 ^((~BCo1)&  BCu1 );
            //bcop32_asm(BCi0, BCo0, BCu0, &Emi0);
            //bcop32_asm(BCi1, BCo1, BCu1, &Emi1); 

            Emo0 =   BCo0 ^((~BCu0)&  BCa0 );
            Emo1 =   BCo1 ^((~BCu1)&  BCa1 );
            //bcop32_asm(BCo0, BCu0, BCa0, &Em00);
            //bcop32_asm(BCo1, BCu1, BCa1, &Em01); 

            Emu0 =   BCu0 ^((~BCa0)&  BCe0 );       
            Emu1 =   BCu1 ^((~BCa1)&  BCe1 );
            //bcop32_asm(BCu0, BCa0, BCe0, &Emu0);
            //bcop32_asm(BCu1, BCa1, BCe1, &Emu1); 

            //printf("\n\nEma: %04X-%04X\n", Ema1, Ema0);
            //printf("Emi: %04X-%04X\n", Eme1, Eme0);
            //printf("Eme: %04X-%04X\n", Emi1, Emi0);
            //printf("Emo: %04X-%04X\n", Emo1, Emo0);
            //printf("Emu: %04X-%04X\n", Emu1, Emu0);

       
            Abi0 ^= Di0;
            Abi1 ^= Di1;
            rol32_asm(Abi1, Abi0, 62, &BCa1, &BCa0);

            Ago1 ^= Do1;
            Ago0 ^= Do0;
            rol32_asm(Ago1, Ago0, 55, &BCe1, &BCe0);


            Aku1 ^= Du1;
            Aku0 ^= Du0;
            rol32_asm(Aku1, Aku0, 39, &BCi1, &BCi0);

            Ama1 ^= Da1;
            Ama0 ^= Da0;
            rol32_asm(Ama1, Ama0, 41, &BCo1, &BCo0);

            Ase0 ^= De0;
            Ase1 ^= De1;
            rol32_asm(Ase1, Ase0, 2, &BCu1, &BCu0);

            //printf("\n\nAbi: %04X-%04X\n", Abi1, Abi0);
            //printf("Emi: %04X-%04X\n", Eme1, Eme0);
            //printf("Eme: %04X-%04X\n", Emi1, Emi0);
            //printf("Emo: %04X-%04X\n", Emo1, Emo0);
            //printf("Emu: %04X-%04X\n", Emu1, Emu0);

            //printf("Bca: %04X-%04X\n\n", BCa1, BCa0);
            //printf("BCi: %04X-%04X\n", BCe1, BCe0);
            //printf("BCe: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            //Esa0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Esa1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Esa0);
            bcop32_asm(BCa1, BCe1, BCi1, &Esa1); 

            //Ese0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ese1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Ese0);
            bcop32_asm(BCe1, BCi1, BCo1, &Ese1); 

            //Esi0 =   BCi0 ^((~BCo0)&  BCu0 );
            bcop32_asm(BCi0, BCo0, BCu0, &Esi0);
            //bcop32_asm(BCi1, BCo1, BCu1, &Esi1); 
            Esi1 =   BCi1 ^((~BCo1)&  BCu1 );
            
            //Eso0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Eso1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Eso0);
            bcop32_asm(BCo1, BCu1, BCa1, &Eso1); 

            //Esu0 =   BCu0 ^((~BCa0)&  BCe0 );       
            bcop32_asm(BCu0, BCa0, BCe0, &Esu0);
            //bcop32_asm(BCu1, BCa1, BCe1, &Esu1); 
            Esu1 =   BCu1 ^((~BCa1)&  BCe1 );

            //printf("Esa: %04X-%04X\n", Esa1, Esa0);
            //printf("Ese: %04X-%04X\n", Ese1, Ese0);
            //printf("Esi: %04X-%04X\n", Esi1, Esi0);
            //printf("Eso: %04X-%04X\n", Eso1, Eso0);
            //printf("Esu: %04X-%04X\n", Esu1, Esu0);

            //    prepareTheta
            BCa0 = Eba0^Ega0^Eka0^Ema0^Esa0;
            BCa1 = Eba1^Ega1^Eka1^Ema1^Esa1;
            BCe0 = Ebe0^Ege0^Eke0^Eme0^Ese0;
            BCe1 = Ebe1^Ege1^Eke1^Eme1^Ese1;
            BCi0 = Ebi0^Egi0^Eki0^Emi0^Esi0;
            BCi1 = Ebi1^Egi1^Eki1^Emi1^Esi1;
            BCo0 = Ebo0^Ego0^Eko0^Emo0^Eso0;
            BCo1 = Ebo1^Ego1^Eko1^Emo1^Eso1;
            BCu0 = Ebu0^Egu0^Eku0^Emu0^Esu0;
            BCu1 = Ebu1^Egu1^Eku1^Emu1^Esu1;

            //printf("\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            //thetaRhoPiChiIota(round+1, E, A)
            rol32_asm(BCe1, BCe0, 1, &BCe0a, &BCe0b);
            Da0 = BCe0b ^ BCu0;
            Da1 = BCe0a ^ BCu1;

            rol32_asm(BCi1, BCi0, 1, &BCi0a, &BCi0b);
            De0 = BCi0b ^ BCa0;
            De1 = BCi0a ^ BCa1;

            rol32_asm(BCo1, BCo0, 1, &BCo0a, &BCo0b);
            Di0 = BCo0b ^ BCe0;
            Di1 = BCo0a ^ BCe1;

            rol32_asm(BCu1, BCu0, 1, &BCu0a, &BCu0b);
            Do0 = BCu0b ^ BCi0;
            Do1 = BCu0a ^ BCi1;

            rol32_asm(BCa1, BCa0, 1, &BCa0a, &BCa0b);
            Du0 = BCa0b ^ BCo0;
            Du1 = BCa0a ^ BCo1;

            //printf("\n\nDa: %04X-%04X\n", Da1, Da0);
            //printf("De: %04X-%04X\n", De1, De0);
            //printf("Di: %04X-%04X\n", Di1, Di0);
            //printf("Do: %04X-%04X\n", Do1, Do0);
            //printf("Du: %04X-%04X\n", Du1, Du0);

            BCa0 = Eba0 ^ Da0;
            BCa1 = Eba1 ^ Da1;

            //BCa0 = Eba0;
            //BCa1 = Eba1;

            Ege0 ^= De0;
            Ege1 ^= De1;
            rol32_asm(Ege1, Ege0, 44, &BCe1, &BCe0);

            Eki1 ^= Di1;
            Eki0 ^= Di0;
            rol32_asm(Eki1, Eki0, 43, &BCi1, &BCi0);

            Emo1 ^= Do1;
            Emo0 ^= Do0;
            rol32_asm(Emo1, Emo0, 21, &BCo1, &BCo0);

            Esu0 ^= Du0;
            Esu1 ^= Du1;
            rol32_asm(Esu1, Esu0, 14, &BCu1, &BCu0);


            Aba0 =   BCa0 ^((~BCe0)&  BCi0 );
            Aba1 =   BCa1 ^((~BCe1)&  BCi1 );

            Aba0 ^= KeccakF1600RoundConstants32[round*2+3];
            Aba1 ^= KeccakF1600RoundConstants32[round*2+2];
  
            //Abe0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Abe1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Abe0);
            bcop32_asm(BCe1, BCi1, BCo1, &Abe1); 

            //Abi0 =   BCi0 ^((~BCo0)&  BCu0 );
            //Abi1 =   BCi1 ^((~BCo1)&  BCu1 );
            bcop32_asm(BCi0, BCo0, BCu0, &Abi0);
            bcop32_asm(BCi1, BCo1, BCu1, &Abi1);            

            //Abo0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Abo1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Abo0);
            bcop32_asm(BCo1, BCu1, BCa1, &Abo1);

            //Abu0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Abu1 =   BCu1 ^((~BCa1)&  BCe1 );
            bcop32_asm(BCu0, BCa0, BCe0, &Abu0);
            bcop32_asm(BCu1, BCa1, BCe1, &Abu1);
       
            Ebo0 ^= Do0;
            Ebo1 ^= Do1;
            rol32_asm(Ebo1, Ebo0, 28, &BCa1, &BCa0);

            Egu0 ^= Du0;
            Egu1 ^= Du1;
            rol32_asm(Egu1, Egu0, 20, &BCe1, &BCe0);

            Eka1 ^= Da1;
            Eka0 ^= Da0;
            rol32_asm(Eka1, Eka0, 3, &BCi1, &BCi0);

            Eme1 ^= De1;
            Eme0 ^= De0;
            rol32_asm(Eme1, Eme0, 45, &BCo1, &BCo0);

            Esi1 ^= Di1;
            Esi0 ^= Di0;
            rol32_asm(Esi1, Esi0, 61, &BCu1, &BCu0);

            //printf("\n\nBca: %04X-%04X\n", BCa1, BCa0);
            //printf("Bce: %04X-%04X\n", BCe1, BCe0);
            //printf("Bci: %04X-%04X\n", BCi1, BCi0);
            //printf("Bco: %04X-%04X\n", BCo1, BCo0);
            //printf("Bcu: %04X-%04X\n", BCu1, BCu0);


            //Aga0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Aga1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Aga0);
            bcop32_asm(BCa1, BCe1, BCi1, &Aga1);

            //Age0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Age1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Age0);
            bcop32_asm(BCe1, BCi1, BCo1, &Age1);

            Agi0 =   BCi0 ^((~BCo0)&  BCu0 );
            //bcop32_asm(BCi0, BCo0, BCu0, &Agi0);
            bcop32_asm(BCi1, BCo1, BCu1, &Agi1);
            //Agi1 =   BCi1 ^((~BCo1)&  BCu1 );

            Ago0 =   BCo0 ^((~BCu0)&  BCa0 );
            Ago1 =   BCo1 ^((~BCu1)&  BCa1 );
            //bcop32_asm(BCo0, BCu0, BCa0, &Ago0);
            //bcop32_asm(BCo1, BCu1, BCa1, &Ago1);

            Agu0 =   BCu0 ^((~BCa0)&  BCe0 );       
            Agu1 =   BCu1 ^((~BCa1)&  BCe1 );
            //bcop32_asm(BCu0, BCa0, BCe0, &Agu0);
            //bcop32_asm(BCu1, BCa1, BCe1, &Agu1);
       
            //printf("\n\nAga: %04X-%04X\n", Aga1, Aga0);
            //printf("age: %04X-%04X\n", Age1, Age0);
            //printf("agi: %04X-%04X\n", Agi1, Agi0);
            //printf("ago: %04X-%04X\n", Ago1, Ago0);
            //printf("agu: %04X-%04X\n", Agu1, Agu0);

            Ebe1 ^= De1;
            Ebe0 ^= De0;
            rol32_asm(Ebe1, Ebe0, 1, &BCa1, &BCa0);

            Egi0 ^= Di0;
            Egi1 ^= Di1;
            rol32_asm(Egi1, Egi0, 6, &BCe1, &BCe0);
            
            Eko1 ^= Do1;
            Eko0 ^= Do0;
            rol32_asm(Eko1, Eko0, 25, &BCi1, &BCi0);
            
            Emu0 ^= Du0;
            Emu1 ^= Du1;
            rol32_asm(Emu1, Emu0, 8, &BCo1, &BCo0);

            Esa0 ^= Da0;
            Esa1 ^= Da1;
            rol32_asm(Esa1, Esa0, 18, &BCu1, &BCu0);         
            
            //Aka0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Aka1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Aka0);
            bcop32_asm(BCa1, BCe1, BCi1, &Aka1);

            Ake0 =   BCe0 ^((~BCi0)&  BCo0 );
            Ake1 =   BCe1 ^((~BCi1)&  BCo1 );
            //bcop32_asm(BCe0, BCi0, BCo0, &Ake0);
            //bcop32_asm(BCe1, BCi1, BCo1, &Ake1);

            //Aki0 =   BCi0 ^((~BCo0)&  BCu0 );
            //Aki1 =   BCi1 ^((~BCo1)&  BCu1 );
            bcop32_asm(BCi0, BCo0, BCu0, &Aki0);
            bcop32_asm(BCi1, BCo1, BCu1, &Aki1);

            //Ako0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Ako1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Ako0);
            bcop32_asm(BCo1, BCu1, BCa1, &Ako1);

            //Aku0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Aku1 =   BCu1 ^((~BCa1)&  BCe1 );
            bcop32_asm(BCu0, BCa0, BCe0, &Aku0);
            bcop32_asm(BCu1, BCa1, BCe1, &Aku1);


            //printf("\n\nAka: %04X-%04X\n", Aka1, Aka0);
            //printf("ake: %04X-%04X\n", Ake1, Ake0);
            //printf("aki: %04X-%04X\n", Aki1, Aki0);
            //printf("ako: %04X-%04X\n", Ako1, Ako0);
            //printf("aku: %04X-%04X\n", Aku1, Aku0);

            Ebu1 ^= Du1;
            Ebu0 ^= Du0;
            rol32_asm(Ebu1, Ebu0, 27, &BCa1, &BCa0);   
            
            Ega0 ^= Da0;
            Ega1 ^= Da1;
            rol32_asm(Ega1, Ega0, 36, &BCe1, &BCe0); 
            
            Eke0 ^= De0;
            Eke1 ^= De1;
            rol32_asm(Eke1, Eke0, 10, &BCi1, &BCi0); 
            
            Emi1 ^= Di1;
            Emi0 ^= Di0;
            rol32_asm(Emi1, Emi0, 15, &BCo1, &BCo0); 
            
            Eso0 ^= Do0;
            Eso1 ^= Do1;
            rol32_asm(Eso1, Eso0, 56, &BCu1, &BCu0); 
            
            //Ama0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Ama1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Ama0);
            bcop32_asm(BCa1, BCe1, BCi1, &Ama1);
            
            //Ame0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ame1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Ame0);
            bcop32_asm(BCe1, BCi1, BCo1, &Ame1);
            
            Ami0 =   BCi0 ^((~BCo0)&  BCu0 );
            Ami1 =   BCi1 ^((~BCo1)&  BCu1 );
            //bcop32_asm(BCi0, BCo0, BCu0, &Ami0);
            //bcop32_asm(BCi1, BCo1, BCu1, &Ami1);
            
            Amo0 =   BCo0 ^((~BCu0)&  BCa0 );
            Amo1 =   BCo1 ^((~BCu1)&  BCa1 );
            //bcop32_asm(BCo0, BCu0, BCa0, &Amo0);
            //bcop32_asm(BCo1, BCu1, BCa1, &Amo1);
            
            //Amu0 =   BCu0 ^((~BCa0)&  BCe0 );       
            //Amu1 =   BCu1 ^((~BCa1)&  BCe1 );
            bcop32_asm(BCu0, BCa0, BCe0, &Amu0);
            bcop32_asm(BCu1, BCa1, BCe1, &Amu1);

            //printf("\n\nAma: %04X-%04X\n", Ama1, Ama0);
            //printf("ame: %04X-%04X\n", Ame1, Ame0);
            //printf("ami: %04X-%04X\n", Ami1, Ami0);
            //printf("amo: %04X-%04X\n", Amo1, Amo0);
            //printf("amu: %04X-%04X\n", Amu1, Amu0);

            Ebi0 ^= Di0;
            Ebi1 ^= Di1;
            rol32_asm(Ebi1, Ebi0, 62, &BCa1, &BCa0); 

            Ego1 ^= Do1;
            Ego0 ^= Do0;
            rol32_asm(Ego1, Ego0, 55, &BCe1, &BCe0); 

            Eku1 ^= Du1;
            Eku0 ^= Du0;
            rol32_asm(Eku1, Eku0, 39, &BCi1, &BCi0);

            Ema1 ^= Da1;
            Ema0 ^= Da0;
            rol32_asm(Ema1, Ema0, 41, &BCo1, &BCo0); 

            Ese0 ^= De0;
            Ese1 ^= De1;
            rol32_asm(Ese1, Ese0, 2, &BCu1, &BCu0); 

            //Asa0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Asa1 =   BCa1 ^((~BCe1)&  BCi1 );
            bcop32_asm(BCa0, BCe0, BCi0, &Asa0);
            bcop32_asm(BCa1, BCe1, BCi1, &Asa1);

            //Ase0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ase1 =   BCe1 ^((~BCi1)&  BCo1 );
            bcop32_asm(BCe0, BCi0, BCo0, &Ase0);
            bcop32_asm(BCe1, BCi1, BCo1, &Ase1);

            Asi0 =   BCi0 ^((~BCo0)&  BCu0 );
            Asi1 =   BCi1 ^((~BCo1)&  BCu1 );
            //bcop32_asm(BCi0, BCo0, BCu0, &Asi0);
            //bcop32_asm(BCi1, BCo1, BCu1, &Asi1);

            //Aso0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Aso1 =   BCo1 ^((~BCu1)&  BCa1 );
            bcop32_asm(BCo0, BCu0, BCa0, &Aso0);
            bcop32_asm(BCo1, BCu1, BCa1, &Aso1);

            Asu0 =   BCu0 ^((~BCa0)&  BCe0 );       
            Asu1 =   BCu1 ^((~BCa1)&  BCe1 );
            //bcop32_asm(BCu0, BCa0, BCe0, &Asu0);
            //bcop32_asm(BCu1, BCa1, BCe1, &Asu1);


            //printf("\n\nAsa: %04X-%04X\n", Asa1, Asa0);
            //printf("ase: %04X-%04X\n", Ase1, Ase0);
            //printf("asi: %04X-%04X\n", Asi1, Asi0);
            //printf("aso: %04X-%04X\n", Aso1, Aso0);
            //printf("asu: %04X-%04X\n", Asu1, Asu0);
            
        }

        //copyToState(state, A)
        state[ 0] = Aba0;
        state[ 1] = Aba1;
        state[ 2] = Abe0;
        state[ 3] = Abe1;
        state[ 4] = Abi0;
        state[ 5] = Abi1;
        state[ 6] = Abo0;
        state[ 7] = Abo1;
        state[ 8] = Abu0;
        state[ 9] = Abu1;
        state[10] = Aga0;
        state[11] = Aga1;
        state[12] = Age0;
        state[13] = Age1;
        state[14] = Agi0;
        state[15] = Agi1;
        state[16] = Ago0;
        state[17] = Ago1;
        state[18] = Agu0;
        state[19] = Agu1;
        state[20] = Aka0;
        state[21] = Aka1;
        state[22] = Ake0;
        state[23] = Ake1;
        state[24] = Aki0;
        state[25] = Aki1;
        state[26] = Ako0;
        state[27] = Ako1;
        state[28] = Aku0;
        state[29] = Aku1;
        state[30] = Ama0;
        state[31] = Ama1;
        state[32] = Ame0;
        state[33] = Ame1;
        state[34] = Ami0;
        state[35] = Ami1;
        state[36] = Amo0;
        state[37] = Amo1;
        state[38] = Amu0;
        state[39] = Amu1;
        state[40] = Asa0;
        state[41] = Asa1;
        state[42] = Ase0;
        state[43] = Ase1;
        state[44] = Asi0;
        state[45] = Asi1;
        state[46] = Aso0;
        state[47] = Aso1;
        state[48] = Asu0;
        state[49] = Asu1;

}


void rol32_asm(uint32_t a, uint32_t b, unsigned int offset, uint32_t *result1, uint32_t *result2) {
     
    asm volatile (
        ".insn r 0x6b, 0x01, 0x0, %[rd1],  %[rs2], %[rs1], %[i]\n\t"
        ".insn r 0x6b, 0x01, 0x1, %[rd2],  %[rs2], %[rs1], %[i]\n\t"
        : [rd1] "=r" (*result1), [rd2] "=r" (*result2)
        : [rs1] "r" (a), [rs2] "r" (b), [i] "r" (offset)
        : "memory");
}

void bcop32_asm(uint32_t a, uint32_t b, uint32_t c, uint32_t *result1) {
     
    asm volatile (
        ".insn r 0x6b, 0x01, 0x2, %[rd1],  %[rs1], %[rs2], %[rs3]\n\t"
        : [rd1] "=r" (*result1)
        : [rs1] "r" (a), [rs2] "r" (b), [rs3] "r" (c)
        : "memory");
}

void Keccak32_asm(uint32_t *state){

    uint32_t Aba0, Abe0, Abi0, Abo0, Abu0;
    uint32_t Aba1, Abe1, Abi1, Abo1, Abu1;
    uint32_t Aga0, Age0, Agi0, Ago0, Agu0;
    uint32_t Aga1, Age1, Agi1, Ago1, Agu1;
    uint32_t Aka0, Ake0, Aki0, Ako0, Aku0;
    uint32_t Aka1, Ake1, Aki1, Ako1, Aku1;
    uint32_t Ama0, Ame0, Ami0, Amo0, Amu0;
    uint32_t Ama1, Ame1, Ami1, Amo1, Amu1;
    uint32_t Asa0, Ase0, Asi0, Aso0, Asu0;
    uint32_t Asa1, Ase1, Asi1, Aso1, Asu1;
    uint32_t BCa0, BCe0, BCi0, BCo0, BCu0;
    uint32_t BCa1, BCe1, BCi1, BCo1, BCu1;
    uint32_t Da0, De0, Di0, Do0, Du0;
    uint32_t Da1, De1, Di1, Do1, Du1;
    uint32_t Eba0, Ebe0, Ebi0, Ebo0, Ebu0;
    uint32_t Eba1, Ebe1, Ebi1, Ebo1, Ebu1;
    uint32_t Ega0, Ege0, Egi0, Ego0, Egu0;
    uint32_t Ega1, Ege1, Egi1, Ego1, Egu1;
    uint32_t Eka0, Eke0, Eki0, Eko0, Eku0;
    uint32_t Eka1, Eke1, Eki1, Eko1, Eku1;
    uint32_t Ema0, Eme0, Emi0, Emo0, Emu0;
    uint32_t Ema1, Eme1, Emi1, Emo1, Emu1;
    uint32_t Esa0, Ese0, Esi0, Eso0, Esu0;
    uint32_t Esa1, Ese1, Esi1, Eso1, Esu1;

    //copyFromState(A, state)
        Aba0 = state[ 0];
        Aba1 = state[ 1];
        Abe0 = state[ 2];
        Abe1 = state[ 3];
        Abi0 = state[ 4];
        Abi1 = state[ 5];
        Abo0 = state[ 6];
        Abo1 = state[ 7];
        Abu0 = state[ 8];
        Abu1 = state[ 9];
        Aga0 = state[10];
        Aga1 = state[11];
        Age0 = state[12];
        Age1 = state[13];
        Agi0 = state[14];
        Agi1 = state[15];
        Ago0 = state[16];
        Ago1 = state[17];
        Agu0 = state[18];
        Agu1 = state[19];
        Aka0 = state[20];
        Aka1 = state[21];
        Ake0 = state[22];
        Ake1 = state[23];
        Aki0 = state[24];
        Aki1 = state[25];
        Ako0 = state[26];
        Ako1 = state[27];
        Aku0 = state[28];
        Aku1 = state[29];
        Ama0 = state[30];
        Ama1 = state[31];
        Ame0 = state[32];
        Ame1 = state[33];
        Ami0 = state[34];
        Ami1 = state[35];
        Amo0 = state[36];
        Amo1 = state[37];
        Amu0 = state[38];
        Amu1 = state[39];
        Asa0 = state[40];
        Asa1 = state[41];
        Ase0 = state[42];
        Ase1 = state[43];
        Asi0 = state[44];
        Asi1 = state[45];
        Aso0 = state[46];
        Aso1 = state[47];
        Asu0 = state[48];
        Asu1 = state[49];

        uint32_t Da0a, Da0b;
        uint32_t De0a, De0b;
        uint32_t Di0a, Di0b;
        uint32_t Do0a, Do0b;
        uint32_t Du0a, Du0b;
        uint32_t BCa0a, BCa0b;
        uint32_t BCe0a, BCe0b;
        uint32_t BCi0a, BCi0b;
        uint32_t BCo0a, BCo0b;
        uint32_t BCu0a, BCu0b;


        for(int round = 0; round < 24; round += 2 )
        {
            //printf("\n\n\n\nround=%d\n\n\n\n\n\n", round);

            //    prepareTheta
            BCa0 = Aba0^Aga0^Aka0^Ama0^Asa0;
            //asm volatile (
            //    ".insn r 0x6b, 0x02, 0x2, a3,  %[rs1a], %[rs2a], %[rs3a]\n\t"
            //    ".insn r 0x6b, 0x02, 0x2, %[rda], a3, %[rs2b], %[rs3b]\n\t"
            //    : [rda] "=r" (BCa0)
            //    : [rs1a] "r" (Aba0), [rs2a] "r" (Aga0), [rs3a] "r" (Aka0), [rs2b] "r" (Ama0), [rs3b] "r" (Asa0)
            //    : "a3", "cc");

            BCa1 = Aba1^Aga1^Aka1^Ama1^Asa1;
            
            BCe0 = Abe0^Age0^Ake0^Ame0^Ase0;
            BCe1 = Abe1^Age1^Ake1^Ame1^Ase1;

            BCi0 = Abi0^Agi0^Aki0^Ami0^Asi0;
            BCi1 = Abi1^Agi1^Aki1^Ami1^Asi1;

            BCo0 = Abo0^Ago0^Ako0^Amo0^Aso0;
            BCo1 = Abo1^Ago1^Ako1^Amo1^Aso1;

            BCu0 = Abu0^Agu0^Aku0^Amu0^Asu0;
            BCu1 = Abu1^Agu1^Aku1^Amu1^Asu1;




            //printf("BCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n\n", BCu1, BCu0);

            //thetaRhoPiChiIota(round  , A, E)
            rol32_asm(BCe1, BCe0, 1, &Da0a, &Da0b);
            Da0 = BCu0^Da0b;
            Da1 = BCu1^Da0a;

            rol32_asm(BCi1, BCi0, 1, &De0a, &De0b);
            De0 = BCa0^De0b;
            De1 = BCa1^De0a;

            rol32_asm(BCo1, BCo0, 1, &Di0a, &Di0b);
            Di0 = BCe0^Di0b;
            Di1 = BCe1^Di0a;

            rol32_asm(BCu1, BCu0, 1, &Do0a, &Do0b);
            Do0 = BCi0^Do0b;
            Do1 = BCi1^Do0a;

            rol32_asm(BCa1, BCa0, 1, &Du0a, &Du0b);
            Du0 = BCo0^Du0b;
            Du1 = BCo1^Du0a;

            //printf("Da: %04X-%04X\n", Da1, Da0);
            //printf("De: %04X-%04X\n", De1, De0);
            //printf("Di: %04X-%04X\n", Di1, Di0);
            //printf("Do: %04X-%04X\n", Do1, Do0);
            //printf("Du: %04X-%04X\n", Du1, Du0);

            BCa0 = Aba0 ^ Da0;
            BCa1 = Aba1 ^ Da1;
            //BCa0 = Aba0;
            //BCa1 = Aba1;
            Age0 ^= De0;
            Age1 ^= De1;

            //printf("Aba: %04X-%04X\n", Aba1, Aba0);
            //printf("BCa: %04X-%04X\n", BCa1, BCa0);
            //printf("\nAge: %04X-%04X\n", Age1, Age0);
            rol32_asm(Age1, Age0, 44, &BCe1, &BCe0);
            Aki1 ^= Di1;
            Aki0 ^= Di0;
            //printf("\nAki: %04X-%04X\n", Aki1, Aki0);

            rol32_asm(Aki1, Aki0, 43, &BCi1, &BCi0);
            Amo1 ^= Do1;
            Amo0 ^= Do0;

            //printf("\nAmo: %04X-%04X\n", Amo1, Amo0);
            rol32_asm(Amo1, Amo0, 21, &BCo1, &BCo0);
            Asu0 ^= Du0;
            Asu1 ^= Du1;
            //printf("\nAsu: %04X-%04X\n", Asu1, Asu0);


            rol32_asm(Asu1, Asu0, 14, &BCu1, &BCu0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);


            //Eba0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Eba0 ^= KeccakF1600RoundConstants32[round*2+1];
            //Eba1 =   BCa1 ^((~BCe1)&  BCi1 );
            //Eba1 ^= KeccakF1600RoundConstants32[round*2+0];
            //printf("\n\n\n*******Eba0: %04X-%04X\n", Eba1, Eba0);
            //printf("keccakconst: %04X-%04X\n", KeccakF1600RoundConstants32[round*2+0], KeccakF1600RoundConstants32[round*2+1]);

            //bcop32_asm(BCa0, BCe0, BCi0, &Eba0);
            //bcop32_asm(BCa1, BCe1, BCi1, &Eba1);

            //asm volatile (
            //    ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
            //    ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
            //    : [rda] "=r" (Eba0), [rdb] "=r" (Eba1)
            //    : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0), [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
            //    : "memory");
            //Eba0 ^= KeccakF1600RoundConstants32[round*2+1];
            //Eba1 ^= KeccakF1600RoundConstants32[round*2+0];

            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, a3,  %[rs1a], %[rs2a], %[rs3a]\n\t"
                "xor %[rda], a3, %[keccak1]\n\t"
                ".insn r 0x6b, 0x01, 0x2, a5,  %[rs1b], %[rs2b], %[rs3b]\n\t"
                "xor %[rdb], a5, %[keccak2]\n\t"
                : [rda] "=r" (Eba0), [rdb] "=r" (Eba1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0), [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1), [keccak1] "r" (KeccakF1600RoundConstants32[round*2+1]), [keccak2] "r" (KeccakF1600RoundConstants32[round*2+0])
                : "a3", "a5", "cc");
            
            //Ebe0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ebe1 =   BCe1 ^((~BCi1)&  BCo1 );

            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebe0), [rdb] "=r" (Ebe1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0), [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            //Ebi0 =   BCi0 ^((~BCo0)&  BCu0 );
            //Ebi1 =   BCi1 ^((~BCo1)&  BCu1 );

            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebi0), [rdb] "=r" (Ebi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0), [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            //Ebo0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Ebo1 =   BCo1 ^((~BCu1)&  BCa1 );

            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebo0), [rdb] "=r" (Ebo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0), [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");


            //Ebu0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Ebu1 =   BCu1 ^((~BCa1)&  BCe1 );

            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebu0), [rdb] "=r" (Ebu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0), [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            //printf("Ebe0: %04X-%04X\n", Ebe1, Ebe0);
            //printf("Ebi0: %04X-%04X\n", Ebi1, Ebi0);
            //printf("Ebo0: %04X-%04X\n", Ebo1, Ebo0);
            //printf("Ebu0: %04X-%04X\n", Ebu1, Ebu0);
       
            Abo0 ^= Do0;
            Abo1 ^= Do1;

            rol32_asm(Abo1, Abo0, 28, &BCa1, &BCa0);
            Agu0 ^= Du0;
            Agu1 ^= Du1;

            rol32_asm(Agu1, Agu0, 20, &BCe1, &BCe0);
            Aka1 ^= Da1;
            Aka0 ^= Da0;

            rol32_asm(Aka1, Aka0, 3, &BCi1, &BCi0);
            Ame1 ^= De1;
            Ame0 ^= De0;

            rol32_asm(Ame1, Ame0, 45, &BCo1, &BCo0);
            Asi1 ^= Di1;
            Asi0 ^= Di0;


            rol32_asm(Asi1, Asi0, 61, &BCu1, &BCu0);
            //printf("\n\nAbo: %04X-%04X\n", Abo1, Abo0);
            //printf("Agu: %04X-%04X\n", Agu1, Agu0);
            //printf("Aka: %04X-%04X\n", Aka1, Aka0);
            //printf("Ame: %04X-%04X\n", Ame1, Ame0);
            //printf("Asi: %04X-%04X\n", Asi1, Asi0);

            //printf("\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            // Ega0 = BCa0 ^ ((~BCe0) & BCi0);
            // Ega1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ega0), [rdb] "=r" (Ega1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Ege0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ege1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ege0), [rdb] "=r" (Ege1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Egi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Egi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Egi0), [rdb] "=r" (Egi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Ego0 = BCo0 ^ ((~BCu0) & BCa0);
            // Ego1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ego0), [rdb] "=r" (Ego1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Egu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Egu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Egu0), [rdb] "=r" (Egu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");


            //printf("\n\nEga: %04X-%04X\n", Ega1, Ega0);
            //printf("Ege: %04X-%04X\n", Ege1, Ege0);
            //printf("Egi: %04X-%04X\n", Egi1, Egi0);
            //printf("Ego: %04X-%04X\n", Ego1, Ego0);
            //printf("Egu: %04X-%04X\n", Egu1, Egu0);

            //printf("\n\nAbe before: %04X-%04X\n", Abe1, Abe0);
            //printf("De: %04X-%04X\n", De1, De0);
            Abe1 ^= De1;
            Abe0 ^= De0;
            //printf("Abe: %04X-%04X\n", Abe1, Abe0);

            rol32_asm(Abe1, Abe0, 1, &BCa1, &BCa0);
            //printf("BCa: %04X-%04X\n", BCa1, BCa0);

            //printf("\n\nAgi before: %04X-%04X\n", Agi1, Agi0);
            //printf("Di: %04X-%04X\n", Di1, Di0);
            Agi0 ^= Di0;
            Agi1 ^= Di1;
            //printf("Agi: %04X-%04X\n", Agi1, Agi0);

            rol32_asm(Agi1, Agi0, 6, &BCe1, &BCe0);
            Ako1 ^= Do1;
            Ako0 ^= Do0;
            
            //printf("\n\nAko before: %04X-%04X\n", Ako1, Ako0);
            //printf("Do: %04X-%04X\n", Do1, Do0);
            rol32_asm(Ako1, Ako0, 25, &BCi1, &BCi0);
            //printf("\n\nAko: %04X-%04X\n", Ako1, Ako0);
    
            Amu0 ^= Du0;
            Amu1 ^= Du1;

            rol32_asm(Amu1, Amu0, 8, &BCo1, &BCo0);

            Asa0 ^= Da0;
            Asa1 ^= Da1;

            rol32_asm(Asa1, Asa0, 18, &BCu1, &BCu0);
            //printf("\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            // Eka0 = BCa0 ^ ((~BCe0) & BCi0);
            // Eka1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eka0), [rdb] "=r" (Eka1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Eke0 = BCe0 ^ ((~BCi0) & BCo0);
            // Eke1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eke0), [rdb] "=r" (Eke1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Eki0 = BCi0 ^ ((~BCo0) & BCu0);
            // Eki1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eki0), [rdb] "=r" (Eki1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Eko0 = BCo0 ^ ((~BCu0) & BCa0);
            // Eko1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eko0), [rdb] "=r" (Eko1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Eku0 = BCu0 ^ ((~BCa0) & BCe0);
            // Eku1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eku0), [rdb] "=r" (Eku1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");


            ////printf("Eka: %04X-%04X\n", Eka1, Eka0);
            ////printf("Eke: %04X-%04X\n", Eke1, Eke0);
            ////printf("Eki: %04X-%04X\n", Eki1, Eki0);
            ////printf("Eko: %04X-%04X\n", Eko1, Eko0);
            ////printf("Eku: %04X-%04X\n", Eku1, Eku0);
       
            Abu1 ^= Du1;
            Abu0 ^= Du0;

            rol32_asm(Abu1, Abu0, 27, &BCa1, &BCa0);
            Aga0 ^= Da0;
            Aga1 ^= Da1;

            rol32_asm(Aga1, Aga0, 36, &BCe1, &BCe0);
            Ake0 ^= De0;
            Ake1 ^= De1;

            rol32_asm(Ake1, Ake0, 10, &BCi1, &BCi0);
            Ami1 ^= Di1;
            Ami0 ^= Di0;

            rol32_asm(Ami1, Ami0, 15, &BCo1, &BCo0);
            Aso0 ^= Do0;
            Aso1 ^= Do1;

            rol32_asm(Aso1, Aso0, 56, &BCu1, &BCu0);
            //printf("\n\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            // Ema0 = BCa0 ^ ((~BCe0) & BCi0);
            // Ema1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ema0), [rdb] "=r" (Ema1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Eme0 = BCe0 ^ ((~BCi0) & BCo0);
            // Eme1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eme0), [rdb] "=r" (Eme1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Emi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Emi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Emi0), [rdb] "=r" (Emi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Emo0 = BCo0 ^ ((~BCu0) & BCa0);
            // Emo1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Emo0), [rdb] "=r" (Emo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Emu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Emu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Emu0), [rdb] "=r" (Emu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");


            //printf("\n\nEma: %04X-%04X\n", Ema1, Ema0);
            //printf("Emi: %04X-%04X\n", Eme1, Eme0);
            //printf("Eme: %04X-%04X\n", Emi1, Emi0);
            //printf("Emo: %04X-%04X\n", Emo1, Emo0);
            //printf("Emu: %04X-%04X\n", Emu1, Emu0);
       
            Abi0 ^= Di0;
            Abi1 ^= Di1;

            rol32_asm(Abi1, Abi0, 62, &BCa1, &BCa0);
            Ago1 ^= Do1;
            Ago0 ^= Do0;

            rol32_asm(Ago1, Ago0, 55, &BCe1, &BCe0);
            Aku1 ^= Du1;
            Aku0 ^= Du0;

            rol32_asm(Aku1, Aku0, 39, &BCi1, &BCi0);
            Ama1 ^= Da1;
            Ama0 ^= Da0;

            rol32_asm(Ama1, Ama0, 41, &BCo1, &BCo0);
            Ase0 ^= De0;
            Ase1 ^= De1;

            rol32_asm(Ase1, Ase0, 2, &BCu1, &BCu0);
            //printf("\n\nBca: %04X-%04X\n", BCa1, BCa0);
            //printf("BCi: %04X-%04X\n", BCe1, BCe0);
            //printf("BCe: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            // Esa0 = BCa0 ^ ((~BCe0) & BCi0);
            // Esa1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Esa0), [rdb] "=r" (Esa1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Ese0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ese1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ese0), [rdb] "=r" (Ese1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Esi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Esi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Esi0), [rdb] "=r" (Esi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Eso0 = BCo0 ^ ((~BCu0) & BCa0);
            // Eso1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eso0), [rdb] "=r" (Eso1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Esu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Esu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Esu0), [rdb] "=r" (Esu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");


            //printf("Esa: %04X-%04X\n", Esa1, Esa0);
            //printf("Ese: %04X-%04X\n", Ese1, Ese0);
            //printf("Esi: %04X-%04X\n", Esi1, Esi0);
            //printf("Eso: %04X-%04X\n", Eso1, Eso0);
            //printf("Esu: %04X-%04X\n", Esu1, Esu0);

            //    prepareTheta
            BCa0 = Eba0^Ega0^Eka0^Ema0^Esa0;
            BCa1 = Eba1^Ega1^Eka1^Ema1^Esa1;
            BCe0 = Ebe0^Ege0^Eke0^Eme0^Ese0;
            BCe1 = Ebe1^Ege1^Eke1^Eme1^Ese1;
            BCi0 = Ebi0^Egi0^Eki0^Emi0^Esi0;
            BCi1 = Ebi1^Egi1^Eki1^Emi1^Esi1;
            BCo0 = Ebo0^Ego0^Eko0^Emo0^Eso0;
            BCo1 = Ebo1^Ego1^Eko1^Emo1^Eso1;
            BCu0 = Ebu0^Egu0^Eku0^Emu0^Esu0;
            BCu1 = Ebu1^Egu1^Eku1^Emu1^Esu1;

            //printf("\n\nBCa: %04X-%04X\n", BCa1, BCa0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);
            //printf("BCi: %04X-%04X\n", BCi1, BCi0);
            //printf("BCo: %04X-%04X\n", BCo1, BCo0);
            //printf("BCu: %04X-%04X\n", BCu1, BCu0);

            //thetaRhoPiChiIota(round+1, E, A)
            rol32_asm(BCe1, BCe0, 1, &BCe0a, &BCe0b);
            Da0 = BCe0b ^ BCu0;
            Da1 = BCe0a ^ BCu1;

            rol32_asm(BCi1, BCi0, 1, &BCi0a, &BCi0b);
            De0 = BCi0b ^ BCa0;
            De1 = BCi0a ^ BCa1;

            rol32_asm(BCo1, BCo0, 1, &BCo0a, &BCo0b);
            Di0 = BCo0b ^ BCe0;
            Di1 = BCo0a ^ BCe1;

            rol32_asm(BCu1, BCu0, 1, &BCu0a, &BCu0b);
            Do0 = BCu0b ^ BCi0;
            Do1 = BCu0a ^ BCi1;

            rol32_asm(BCa1, BCa0, 1, &BCa0a, &BCa0b);
            Du0 = BCa0b ^ BCo0;
            Du1 = BCa0a ^ BCo1;

            //printf("\n\nDa: %04X-%04X\n", Da1, Da0);
            //printf("De: %04X-%04X\n", De1, De0);
            //printf("Di: %04X-%04X\n", Di1, Di0);
            //printf("Do: %04X-%04X\n", Do1, Do0);
            //printf("Du: %04X-%04X\n", Du1, Du0);

            Eba0 ^= Da0;
            Eba1 ^= Da1;

            BCa0 = Eba0;
            BCa1 = Eba1;

            Ege0 ^= De0;
            Ege1 ^= De1;

            rol32_asm(Ege1, Ege0, 44, &BCe1, &BCe0);
            Eki1 ^= Di1;
            Eki0 ^= Di0;

            rol32_asm(Eki1, Eki0, 43, &BCi1, &BCi0);
            Emo1 ^= Do1;
            Emo0 ^= Do0;

            rol32_asm(Emo1, Emo0, 21, &BCo1, &BCo0);
            Esu0 ^= Du0;
            Esu1 ^= Du1;

            rol32_asm(Esu1, Esu0, 14, &BCu1, &BCu0);
            //printf("BCe: %04X-%04X\n", BCe1, BCe0);

            Aba0 =   BCa0 ^((~BCe0)&  BCi0 );
            Aba1 =   BCa1 ^((~BCe1)&  BCi1 );
            //printf("\n\nAba: %04X-%04X\n", Aba1, Aba0);

            Aba0 ^= KeccakF1600RoundConstants32[round*2+3];
            Aba1 ^= KeccakF1600RoundConstants32[round*2+2];
            //printf("\n\nAba: %04X-%04X\n", Aba1, Aba0);
            //printf("keccakconst: %04X-%04X\n", KeccakF1600RoundConstants32[round*2+2], KeccakF1600RoundConstants32[round*2+3]);

            // Abe0 = BCe0 ^ ((~BCi0) & BCo0);
            // Abe1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abe0), [rdb] "=r" (Abe1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Abi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Abi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abi0), [rdb] "=r" (Abi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Abo0 = BCo0 ^ ((~BCu0) & BCa0);
            // Abo1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abo0), [rdb] "=r" (Abo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Abu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Abu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abu0), [rdb] "=r" (Abu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");

       
            Ebo0 ^= Do0;
            Ebo1 ^= Do1;
            
            rol32_asm(Ebo1, Ebo0, 28, &BCa1, &BCa0);
            Egu0 ^= Du0;
            Egu1 ^= Du1;

            rol32_asm(Egu1, Egu0, 20, &BCe1, &BCe0);
            Eka1 ^= Da1;
            Eka0 ^= Da0;

            rol32_asm(Eka1, Eka0, 3, &BCi1, &BCi0);
            Eme1 ^= De1;
            Eme0 ^= De0;

            rol32_asm(Eme1, Eme0, 45, &BCo1, &BCo0);
            Esi1 ^= Di1;
            Esi0 ^= Di0;

            rol32_asm(Esi1, Esi0, 61, &BCu1, &BCu0);
            //printf("\n\nBca: %04X-%04X\n", BCa1, BCa0);
            //printf("Bce: %04X-%04X\n", BCe1, BCe0);
            //printf("Bci: %04X-%04X\n", BCi1, BCi0);
            //printf("Bco: %04X-%04X\n", BCo1, BCo0);
            //printf("Bcu: %04X-%04X\n", BCu1, BCu0);

            // Aga0 = BCa0 ^ ((~BCe0) & BCi0);
            // Aga1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aga0), [rdb] "=r" (Aga1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Age0 = BCe0 ^ ((~BCi0) & BCo0);
            // Age1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Age0), [rdb] "=r" (Age1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Agi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Agi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Agi0), [rdb] "=r" (Agi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Ago0 = BCo0 ^ ((~BCu0) & BCa0);
            // Ago1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ago0), [rdb] "=r" (Ago1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Agu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Agu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Agu0), [rdb] "=r" (Agu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");

       
            //printf("\n\nAga: %04X-%04X\n", Aga1, Aga0);
            //printf("age: %04X-%04X\n", Age1, Age0);
            //printf("agi: %04X-%04X\n", Agi1, Agi0);
            //printf("ago: %04X-%04X\n", Ago1, Ago0);
            //printf("agu: %04X-%04X\n", Agu1, Agu0);

            Ebe1 ^= De1;
            Ebe0 ^= De0;

            rol32_asm(Ebe1, Ebe0, 1, &BCa1, &BCa0);
            Egi0 ^= Di0;
            Egi1 ^= Di1;
            
            rol32_asm(Egi1, Egi0, 6, &BCe1, &BCe0);       
            Eko1 ^= Do1;
            Eko0 ^= Do0;
            
            rol32_asm(Eko1, Eko0, 25, &BCi1, &BCi0);
            Emu0 ^= Du0;
            Emu1 ^= Du1;

            rol32_asm(Emu1, Emu0, 8, &BCo1, &BCo0);
            Esa0 ^= Da0;
            Esa1 ^= Da1;

            rol32_asm(Esa1, Esa0, 18, &BCu1, &BCu0);


            // Aka0 = BCa0 ^ ((~BCe0) & BCi0);
            // Aka1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aka0), [rdb] "=r" (Aka1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Ake0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ake1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ake0), [rdb] "=r" (Ake1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Aki0 = BCi0 ^ ((~BCo0) & BCu0);
            // Aki1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aki0), [rdb] "=r" (Aki1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Ako0 = BCo0 ^ ((~BCu0) & BCa0);
            // Ako1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ako0), [rdb] "=r" (Ako1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Aku0 = BCu0 ^ ((~BCa0) & BCe0);
            // Aku1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aku0), [rdb] "=r" (Aku1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");


            //printf("\n\nAka: %04X-%04X\n", Aka1, Aka0);
            //printf("ake: %04X-%04X\n", Ake1, Ake0);
            //printf("aki: %04X-%04X\n", Aki1, Aki0);
            //printf("ako: %04X-%04X\n", Ako1, Ako0);
            //printf("aku: %04X-%04X\n", Aku1, Aku0);

            Ebu1 ^= Du1;
            Ebu0 ^= Du0;

            rol32_asm(Ebu1, Ebu0, 27, &BCa1, &BCa0);            
            Ega0 ^= Da0;
            Ega1 ^= Da1;
            
            rol32_asm(Ega1, Ega0, 36, &BCe1, &BCe0);    
            Eke0 ^= De0;
            Eke1 ^= De1;

            rol32_asm(Eke1, Eke0, 10, &BCi1, &BCi0);
            Emi1 ^= Di1;
            Emi0 ^= Di0;
            
            rol32_asm(Emi1, Emi0, 15, &BCo1, &BCo0);
            Eso0 ^= Do0;
            Eso1 ^= Do1;

            rol32_asm(Eso1, Eso0, 56, &BCu1, &BCu0);
        
            // Ama0 = BCa0 ^ ((~BCe0) & BCi0);
            // Ama1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ama0), [rdb] "=r" (Ama1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");

            // Ame0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ame1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ame0), [rdb] "=r" (Ame1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");

            // Ami0 = BCi0 ^ ((~BCo0) & BCu0);
            // Ami1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ami0), [rdb] "=r" (Ami1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");

            // Amo0 = BCo0 ^ ((~BCu0) & BCa0);
            // Amo1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Amo0), [rdb] "=r" (Amo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");

            // Amu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Amu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Amu0), [rdb] "=r" (Amu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");


            //printf("\n\nAma: %04X-%04X\n", Ama1, Ama0);
            //printf("ame: %04X-%04X\n", Ame1, Ame0);
            //printf("ami: %04X-%04X\n", Ami1, Ami0);
            //printf("amo: %04X-%04X\n", Amo1, Amo0);
            //printf("amu: %04X-%04X\n", Amu1, Amu0);

            Ebi0 ^= Di0;
            Ebi1 ^= Di1;

            rol32_asm(Ebi1, Ebi0, 62, &BCa1, &BCa0);
            Ego1 ^= Do1;
            Ego0 ^= Do0;

            rol32_asm(Ego1, Ego0, 55, &BCe1, &BCe0);
            Eku1 ^= Du1;
            Eku0 ^= Du0;

            rol32_asm(Eku1, Eku0, 39, &BCi1, &BCi0);
            Ema1 ^= Da1;
            Ema0 ^= Da0;

            rol32_asm(Ema1, Ema0, 41, &BCo1, &BCo0);
            Ese0 ^= De0;
            Ese1 ^= De1;

            rol32_asm(Ese1, Ese0, 2, &BCu1, &BCu0);
 
        // Asa0 = BCa0 ^ ((~BCe0) & BCi0);
        // Asa1 = BCa1 ^ ((~BCe1) & BCi1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Asa0), [rdb] "=r" (Asa1)
            : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
            [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
            : "cc");

        // Ase0 = BCe0 ^ ((~BCi0) & BCo0);
        // Ase1 = BCe1 ^ ((~BCi1) & BCo1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Ase0), [rdb] "=r" (Ase1)
            : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
            [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
            : "cc");

        // Asi0 = BCi0 ^ ((~BCo0) & BCu0);
        // Asi1 = BCi1 ^ ((~BCo1) & BCu1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Asi0), [rdb] "=r" (Asi1)
            : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
            [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
            : "cc");

        // Aso0 = BCo0 ^ ((~BCu0) & BCa0);
        // Aso1 = BCo1 ^ ((~BCu1) & BCa1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Aso0), [rdb] "=r" (Aso1)
            : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
            [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
            : "cc");

        // Asu0 = BCu0 ^ ((~BCa0) & BCe0);
        // Asu1 = BCu1 ^ ((~BCa1) & BCe1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Asu0), [rdb] "=r" (Asu1)
            : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
            [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
            : "cc");


            //printf("\n\nAsa: %04X-%04X\n", Asa1, Asa0);
            //printf("ase: %04X-%04X\n", Ase1, Ase0);
            //printf("asi: %04X-%04X\n", Asi1, Asi0);
            //printf("aso: %04X-%04X\n", Aso1, Aso0);
            //printf("asu: %04X-%04X\n", Asu1, Asu0);
            
        }

        //copyToState(state, A)
        state[ 0] = Aba0;
        state[ 1] = Aba1;
        state[ 2] = Abe0;
        state[ 3] = Abe1;
        state[ 4] = Abi0;
        state[ 5] = Abi1;
        state[ 6] = Abo0;
        state[ 7] = Abo1;
        state[ 8] = Abu0;
        state[ 9] = Abu1;
        state[10] = Aga0;
        state[11] = Aga1;
        state[12] = Age0;
        state[13] = Age1;
        state[14] = Agi0;
        state[15] = Agi1;
        state[16] = Ago0;
        state[17] = Ago1;
        state[18] = Agu0;
        state[19] = Agu1;
        state[20] = Aka0;
        state[21] = Aka1;
        state[22] = Ake0;
        state[23] = Ake1;
        state[24] = Aki0;
        state[25] = Aki1;
        state[26] = Ako0;
        state[27] = Ako1;
        state[28] = Aku0;
        state[29] = Aku1;
        state[30] = Ama0;
        state[31] = Ama1;
        state[32] = Ame0;
        state[33] = Ame1;
        state[34] = Ami0;
        state[35] = Ami1;
        state[36] = Amo0;
        state[37] = Amo1;
        state[38] = Amu0;
        state[39] = Amu1;
        state[40] = Asa0;
        state[41] = Asa1;
        state[42] = Ase0;
        state[43] = Ase1;
        state[44] = Asi0;
        state[45] = Asi1;
        state[46] = Aso0;
        state[47] = Aso1;
        state[48] = Asu0;
        state[49] = Asu1;

}
