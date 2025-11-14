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

#ifndef DILITHIUM2_INSTRUCTIONS_H
#define DILITHIUM2_INSTRUCTIONS_H

/* Set to 1 to enable the hardware-accelerated/custom version,
   or 0 to fall back to the reference (“clean”) implementation. */
#define ENABLE_DILITHIUM_MONTG         0
#define ENABLE_DILITHIUM_REDUCE32      0  
#define ENABLE_DILITHIUM_INVNTT        0
#define ENABLE_DILITHIUM_NTT           0
#define ENABLE_KECCAK                  0
#define ENABLE_DILITHIUM_CADDQ         0
#define ENABLE_REJ_UNIFORM             0

#define ENABLE_DILITHIUM_POWER2ROUND   0

#define PROFILING_SIGN                 1

#endif /* dilithium2_INSTRUCTIONS_H */
