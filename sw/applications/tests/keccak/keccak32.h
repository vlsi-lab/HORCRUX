#ifndef KECCAK32_H
#define KECCAK32_H

#include <stdint.h>

#define NROUNDS 24Add commentMore actions
#define SIZE 50


void rol32(uint32_t a, uint32_t b, unsigned int offset, uint32_t *result1, uint32_t *result2);
void Keccak32(uint32_t *state);

void rol32_asm(uint32_t a, uint32_t b, unsigned int offset, uint32_t *result1, uint32_t *result2);
void bcop32_asm(uint32_t a, uint32_t b, uint32_t c, uint32_t *result1) __attribute__((used));
void bcop32_const_asm(uint32_t a, uint32_t b, uint32_t c, uint32_t keccak_const, uint32_t *result1) ;
void Keccak32_asm(uint32_t *state);

#endif