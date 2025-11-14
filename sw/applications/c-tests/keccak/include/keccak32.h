#ifndef KECCAK32_H
#define KECCAK32_H

#include <stdint.h>

#define NROUNDS 24
#define SIZE 50


void rol32(uint32_t a, uint32_t b, unsigned int offset, uint32_t *result1, uint32_t *result2);
void Keccak32(uint32_t *state);


#endif