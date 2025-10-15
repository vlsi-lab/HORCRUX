///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DILITHIUM5_INSTRUCTIONS_H
#define DILITHIUM5_INSTRUCTIONS_H

/* Set to 1 to enable the hardware-accelerated/custom version,
   or 0 to fall back to the reference (“clean”) implementation. */
#define ENABLE_DILITHIUM_MONTG         1
#define ENABLE_DILITHIUM_REDUCE32      1  
#define ENABLE_DILITHIUM_INVNTT        1
#define ENABLE_DILITHIUM_NTT           1
#define ENABLE_KECCAK                  1
#define ENABLE_REJ_UNIFORM             1

#define ENABLE_DILITHIUM_POWER2ROUND   0

#endif /* dilithium5_INSTRUCTIONS_H */
